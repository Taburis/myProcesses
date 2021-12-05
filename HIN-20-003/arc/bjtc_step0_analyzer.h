#include "myProcesses/jtc/plugin/workflow.h"
//#include "myProcesses/jtc/plugin/Utility.h"
//#include "myProcesses/jtc/plugin/plotLib.h"
#include "myProcesses/liteFrame/plugin/plots/plotManager.h"
#include "myProcesses/jtc/plugin/jtcTH1Player.h"

class bjtc_step0_analyzer: public analyzer {
	public :
		bjtc_step0_analyzer(TString name, workflow &b0, ParaSet &ps0) : analyzer(name,b0, ps0){
			npt = ps0.getPara<int>("npt");
			ncent = ps0.getPara<int>("ncent");
		}
		~bjtc_step0_analyzer(){}
		void pullCentWeight(TString name, TString smc, TString sdata);
		void pullVzWeight(TString name, TString smc, TString sdata);
		void jetPtComparison(TString name, TString f1, TString f2);
		void jetPtWeight(TString name, TString smc, TString sdata);
		void jet_spectra_check(TString name, TString st1, TString st2, TString fs1, TString fs2);
		void bin_shift(TH1*, int);
		void jet_spectra_check2(TString name, TString st1, TString fs1);
		void MCweight_check(TString , TString, TString st1, TString fs1, bool doshift = 0);
		TString format = ".jpg";
		virtual void analyze();
		int ncent , npt;
};
void bjtc_step0_analyzer::jet_spectra_check(TString name, TString st1, TString st2, TString fs1, TString fs2){
	auto f1 = TFile::Open(fs1);
	auto f2 = TFile::Open(fs2);
	auto j1 = new jtcTH1Player(st1, ncent, 3);
	auto j2 = new jtcTH1Player(st2, ncent, 3);
	auto c = new plotManager();
	c->initOverlayPad("c_spectra", "", 3, ncent);
	for(int i=0; i<ncent; ++i){
		auto h = (TH1*) f1->Get(Form("jetQASets/incl_RecoLevel_pt_C%d",i));
		h->SetTitle("p_T^{jet}");
		j1->add(h, 0, i);
		h = (TH1*) f1->Get(Form("jetQASets/incl_RecoLevel_eta_C%d",i));
		h->SetTitle("#eta^{jet}");
		j1->add(h, 1, i);
		h = (TH1*) f1->Get(Form("jetQASets/incl_RecoLevel_phi_C%d",i));
		h->SetTitle("#phi^{jet}");
		j1->add(h, 2, i);
		h = (TH1*) f2->Get(Form("jetQASets/incl_RecoLevel_pt_C%d",i));
		j2->add(h, 0, i);
		h = (TH1*) f2->Get(Form("jetQASets/incl_RecoLevel_eta_C%d",i));
		j2->add(h, 1, i);
		h = (TH1*) f2->Get(Form("jetQASets/incl_RecoLevel_phi_C%d",i));
		j2->add(h, 2, i);
	}

	c->addm2TH1(j1, st1);
	c->addm2TH1(j2, st2);
	c->draw();
	c->drawLegend();
	c->save(fig_output+"/"+name+format);
}
/*
void bjtc_step0_analyzer::jet_spectra_check2(TString name, TString st1, TString fs1){
	st1 = "jetQASets/"+st1;
	auto f1 = TFile::Open(fs1);
	auto j1 = new jtcTH1Player(st1+"_0", 1, 3);
	auto j2 = new jtcTH1Player(st1+"_1", 1, 3);
	auto h = (TH1*) f1->Get(st1+Form("_pt_C%d",0))->Clone(Form(st1+"_pt_clone_C%d",0));
	h->Scale(1.0/h->Integral());
	j1->add(h, 0, 0);
	h = (TH1*) f1->Get(st1+Form("_pt_C%d",1))->Clone(Form(st1+"_pt_clone_C%d",1));
	h->Scale(1.0/h->Integral());
	j2->add(h, 0, 0);
	h = (TH1*) f1->Get(st1+Form("_eta_C%d",0))->Clone(Form(st1+"_eta_clone_C%d",0));
	h->Scale(1.0/h->Integral());
	j1->add(h, 0, 1);
	h = (TH1*) f1->Get(st1+Form("_eta_C%d",1))->Clone(Form(st1+"_eta_clone_C%d",1));
	h->Scale(1.0/h->Integral());
	j2->add(h, 0, 1);
	h = (TH1*) f1->Get(st1+Form("_phi_C%d",0))->Clone(Form(st1+"_phi_clone_C%d",0));
	h->Scale(1.0/h->Integral());
	j1->add(h, 0, 2);
	h = (TH1*) f1->Get(st1+Form("_phi_C%d",1))->Clone(Form(st1+"_phi_clone_C%d",1));
	h->Scale(1.0/h->Integral());
	j2->add(h, 0, 2);
	auto c = new multi_pads<overlay_pad>("c_spectra", "", 1, 3);
	c->doAutoYrange = 1;
	c->setRatioYrange(0.5, 1.5);
	c->addm2TH1(j1);
	c->addm2TH1(j2);
	c->addLegend("upperright");
	c->labelHist("Cent: 0-30%",0);
	c->labelHist("Cent: 30-90%",1);
	c->draw();
	c->SaveAs(fig_output+"/"+name+format);
}
*/
void bjtc_step0_analyzer::bin_shift(TH1* h, int n){
	//shift the bin content to the left by n bins
	for(auto i=1; i<h->GetNbinsX()+1-n; i++){
		double c = h->GetBinContent(i+n);
		double e = h->GetBinError(i+n);
		h->SetBinContent(i,c);
		h->SetBinError(i,e);
	}	
}
void bjtc_step0_analyzer::MCweight_check(TString name1, TString name2, TString s1, TString s2, bool shift){
	//f1 is suppose to be MC and f2 is suppose to be Data
	auto f1 = TFile::Open(s1);
	auto f2 = TFile::Open(s2);
	TH1* hvz1 = (TH1*) f1->Get("vzInfo"); hvz1->Scale(1.0/hvz1->Integral("width"));
	TH1* hvz2 = (TH1*) f2->Get("vzInfo"); hvz2->Scale(1.0/hvz2->Integral("width"));
	TH1* hc1 = (TH1*) f1->Get("centInfo"); hc1->Scale(1.0/hc1->Integral("width"));
	TH1* hc2 = (TH1*) f2->Get("centInfo"); hc2->Scale(1.0/hc2->Integral("width"));
	TH1* hpt = (TH1*) f1->Get("pthatInfo");
	hvz1->GetXaxis()->SetTitle("v_{z}");
	hc1->GetXaxis()->SetTitle("hiBin");
	auto evt1 = new jtcTH1Player("evtInfoMC_"+name1,  1, 2);
	auto evt2 = new jtcTH1Player("evtInfoRef_"+name2,  1, 2);
	if(shift) bin_shift(hc1,2);
	evt1->add(hvz1, 0, 0);	evt1->add(hc1, 0, 1);
	evt2->add(hvz2, 0, 0);	evt2->add(hc2, 0, 1);
//	auto c = new multi_pads<overlay_pad>("event_info_"+name1+"_"+name2, "", 1, 2);	
	auto c = new plotManager();
	c->initSquarePad("evet_info"+name1+"_"+name2, "", 1, 2);	
	//c->initOverlayPad("evet_info"+name1+"_"+name2, "", 1, 2);	
	c->addHist(hvz1, 0, 0, name1, "pdf");
	c->addHist(hvz2, 0, 0, name2, "pdf");
	c->addHist(hc1, 0, 1, name1, "pf");
	c->addHist(hc2, 0, 1, name2, "pf");
	c->draw();
	c->drawLegend();
	c->c->SaveAs(fig_output+"/evtInfo_"+name1+"_vs_"+name2+format);
}
/*
void bjtc_step0_analyzer::pullCentWeight(TString name, TString smc, TString sdata){
	auto f1 = TFile::Open(smc);
	auto f2 = TFile::Open(sdata);
	auto hc1 =(TH1*) f1->Get("centInfo"); hc1->Scale(1.0/hc1->Integral());
	auto wf = TFile::Open("weight.root","recreate");
	wf->cd();
	auto hweight =(TH1*) f2->Get("centInfo")->Clone("centWeight_extra"); hweight->Scale(1.0/hweight->Integral());
	auto hc2 =(TH1*) f2->Get("centInfo"); hc2->Scale(1.0/hc2->Integral());
	auto c = new multi_pads<base_pad>("cweight_"+name, "", 1, 2);
	c->CD(0,0);
	hweight->Divide(hc1);
	hweight->Fit("pol5","", "", 0,180);
	c->CD(0,1);
	hc1->Draw();
	hc2->Draw("same");
	c->SaveAs(fig_output+"/centWeight_"+name+format);
	hweight->Write();
	wf->Close();	
}

void bjtc_step0_analyzer::jetPtWeight(TString name, TString smc, TString sdata){
	auto f1 = TFile::Open(sdata);
	auto f2 = TFile::Open(smc);
	auto j1 = new jtcTH1Player("jetptData",  1, ncent);
	auto j2 = new jtcTH1Player("jetptMC", 1, ncent);
	auto jr = new jtcTH1Player("jetptRatio",  1, ncent);
	for(int i=0; i< ncent; i++){
		auto h = (TH1*) f1->Get(Form("inclJet_corrpt_%d",i));
		h->GetXaxis()->SetRangeUser(100, 400);
		h->Scale(1.0/h->Integral("w"));
		j1->add(h, 0, i);
		auto h2 = (TH1*) h->Clone("inclJet_ratio");
		jr->add(h2,0,i);

		h = (TH1*) f2->Get(Form("jetQASets/incl_RecoLevel_pt_C%d",i));
		h->GetXaxis()->SetRangeUser(100, 400);
		h->Scale(1.0/h->Integral("w"));
		j2->add(h, 0, i);
		
		jr->at(0,i)->Divide(h);	
	}
	
	auto c = new multi_pads<overlay_pad>("cjetpt_"+name, "", 1, ncent);
	c->addLegend("upperright");
	c->addm2TH1(j1);
	c->addm2TH1(j2);
	c->labelHist("Data",0);
	c->labelHist("MC",  1);
	c->draw();
	c->SaveAs(fig_output+"/jetPt_"+name+format);
	auto c1 = new multi_pads<base_pad>("cjetweight_"+name, "", 1, ncent);
	c1->CD(0,0);
	jr->at(0,0)->Fit("pol5", "", "", 120, 400);
	c1->SaveAs(fig_output+"/jetPtWeight_"+name+format);
}

void bjtc_step0_analyzer::pullVzWeight(TString name, TString smc, TString sdata){
	auto f1 = TFile::Open(smc);
	auto f2 = TFile::Open(sdata);
	auto hvz1 =(TH1*) f1->Get("vzInfo"); hvz1->Scale(1.0/hvz1->Integral());
//	auto wf = TFile::Open("weight.root","recreate");
//	wf->cd();
	auto hweight =(TH1*) f2->Get("vzInfo")->Clone("vzWeight_extra"); hweight->Scale(1.0/hweight->Integral());
	auto hvz2 =(TH1*) f2->Get("vzInfo"); hvz2->Scale(1.0/hvz2->Integral());
	hvz2->SetLineColor(kRed);
	hvz2->SetMarkerColor(kRed);
	auto c = new multi_pads<base_pad>("cvzweight_"+name, "", 1, 2);
	c->CD(0,0);
	hweight->Divide(hvz1);
	//hweight->Draw();
	hweight->Fit("pol5","", "", -15,15);
	c->CD(0,1);
	hvz1->Draw();
	hvz2->Draw("same");
	c->SaveAs(fig_output+"/vzWeight_"+name+format);
//	hweight->Write();
//	wf->Close();	

}
*/
void bjtc_step0_analyzer::analyze(){
}
