
#include <cstdlib>
#include "myProcesses/hiforest/plugin/simpleReader.h"
//#include "myProcesses/btagger/plugin/bTaggerAnalyzer.h"
#include "myProcesses/btagger/plugin/bTaggerLib.h"
#include "myProcesses/jtc/plugin/jtcTH1Player.h"
#include "myProcesses/jtc/plugin/PLOTLIB/multi_pads.h"
#include "TProfile.h"

using namespace btagger_utility;
class bTaggerStep2Analyzer{
	public : bTaggerStep2Analyzer(TString n): name(n){
		 }
		 void init(){
			 TString folder = folderPath+name+"_QAs/";
			 const int dir_err = system("mkdir -p "+folder);
		 }
		 ~bTaggerStep2Analyzer(){}
		 void loadMC(TFile *f){srmc.load(f); _mcf = f;}
		 void loadData(TFile *f){srdata.load(f); _dataf = f;}
		 void linkCentralityHelper( centralityHelper *ch ) {
			 cent = ch; ncent = cent->nbins; cent->makeLabels();
		 }
		 void tagBias();

		 void JEC(TString name, TString);
		 void calculateSF(int ncsv, float xmin, float xmax){
			 calculateSF_MC(ncsv, xmin, xmax);
			 calculateSF_Data(ncsv, xmin, xmax);
			 calculateSF_final(ncsv, xmin, xmax);
		 }
		 void calculateSF_MC(int ncsv, float xmin, float xmax);
		 void calculateSF_Data(int ncsv, float xmin, float xmax);
		 void calculateSF_final(int ncsv,float xmin,float xmax);
		 multi_pads<stack_pad>* flavorFracPlot(TString name);

		 void stackStyle(THStack* h, TString xtitle){
			 h->GetXaxis()->SetTitle(xtitle);
		 }

		 void pullJetPtWeight(TString name);

		 multi_pads<stack_pad>* addStackPlot(TString name, int i = 1);

		 void addCentLabel(TCanvas* c);
		 void drawQAs();

		 TString name;
		 int ncent;
		 centralityHelper *cent;
		 simpleReader srmc, srdata;
		 TString folderPath = "./";
		 std::vector<TH1*> cleanList;
		 std::vector<histBundle> cleanList_bundle;
		 TFile*_mcf,*_dataf;
		 matrixTH1Ptr *m2mis, *m2neg, *m2R, *m2sf, *m2mis_data, *m2neg_data;
		 TString format = ".png";
		 TFile *wf = nullptr;
		 TString output="./";
		 //		std::unordered_map<TString, multi_pads<stack_pads> *> stack_figs;
};

void bTaggerStep2Analyzer::pullJetPtWeight(TString name){
	TH1D** hweight = new TH1D*[ncent];
	TH1D** hweight2= new TH1D*[ncent];
	auto c = new multi_pads<fast_pad>("jtW_canvas", "jet weight", 1, ncent);
	for(auto i = 0; i< ncent; ++i){
		hweight[i] = (TH1D*) ((TH2*)srdata[Form("jtpt_C%d", i)])->ProjectionX(Form("jtptWeight_C%d",i),4,4);
		hweight[i]->Scale(1.0/hweight[i]->Integral());
		auto h = (TH1D*) ((TH2*)srmc[Form("jtpt_C%d", i)])->ProjectionX(Form("hmc_C%d",i));
		h->Scale(1.0/h->Integral());
		hweight[i] ->Divide(h);
		hweight2[i]=(TH1D*)hweight[i]->Clone(Form("jtptW_smth_C%d",i));
		hweight2[i]->Smooth();
		c->add(hweight[i], 0, ncent-i-1);
		c->add(hweight2[i], 0, ncent-i-1);
	}
	c->draw();
	TString folder = folderPath;
	c->SaveAs(folder+"jtptWeight"+format);
	auto wf = TFile::Open(name, "recreate");
	for(auto i = 0; i< ncent; ++i){
		hweight[i]->Write();
		hweight2[i]->Write();
	}
	wf->Close();
}

void bTaggerStep2Analyzer::JEC(TString cname, TString dir){
	TString hname_b  = dir+cname+"_jec_b_C%d";
	TString hname_tg = dir+cname+"_jec_tag_C%d";
	auto c = new multi_pads<fast_pad>(cname+"_canvas", "JEC closure", 1, ncent);
	c->doAutoYrange= 0;
	c->addhLine(1);
	c->addLegend("upperright");
	for(int i=0; i< ncent; ++i){
		TString title = "JEC closure: "+cent->centLabel[i];
		auto hb  = ((TH2*) srmc[Form(hname_b,i)] )->ProfileX(Form(hname_b, i)+TString("_profX"));
		auto htg = ((TH2*) srmc[Form(hname_tg,i)])->ProfileX(Form(hname_tg, i)+TString("_profX"));
		htg->SetTitle(title);
		htg->GetXaxis()->SetTitle ("p_{T}^{Ref}");
		htg->GetYaxis()->SetTitle("<p_{T}^{reco}/p_{T}^{Ref}>");
		if(i==3){
			c->legend->AddEntry(htg, "tag-jet");
			c->legend->AddEntry(hb, "tagged b jet");
		}
		htg->SetAxisRange(0.8, 1.2, "Y");
		c->add(htg, 0, ncent-i-1);
		c->add(hb, 0, ncent-i-1);
		cleanList.emplace_back((TH1*)hb);
		cleanList.emplace_back((TH1*)htg);
	}
	c->xtitle="p_{T}^{Reco}";
	c->setYrange(0.8, 1.2);
	c->draw();
	TString folder = folderPath+name+"_QAs/"+cname;
	c->SaveAs(folder+"_JEC"+format);
}
/*
void bTaggerStep2Analyzer::JER(TString cname){
	TString hname_b  = cname+"jer_C%d";
	TString hname_tg = cname+"jer_C%d";
	auto c = new multi_pads<fast_pad>(cname+"_canvas", "JEC closure", 1, ncent);
	c->doAutoYrange= 0;
	c->addhLine(1);
	c->addLegend("upperright");
	for(int i=0; i< ncent; ++i){
		TString title = "JEC closure: "+cent->centLabel[i];
		auto hb  = ((TH2*) srmc[Form(hname_b,i)] )->ProfileX(Form(hname_b, i)+TString("_profX"));
		auto htg = ((TH2*) srmc[Form(hname_tg,i)])->ProfileX(Form(hname_tg, i)+TString("_profX"));
		htg->SetTitle(title);
		htg->GetXaxis()->SetTitle ("p_{T}^{Ref}");
		htg->GetYaxis()->SetTitle("<p_{T}^{reco}/p_{T}^{Ref}>");
		if(i==3){
			c->legend->AddEntry(htg, "tag-jet");
			c->legend->AddEntry(hb, "b-jet");
		}
		htg->SetAxisRange(0.8, 1.2, "Y");
		c->add(htg, 0, ncent-i-1);
		c->add(hb, 0, ncent-i-1);
		cleanList.emplace_back((TH1*)hb);
		cleanList.emplace_back((TH1*)htg);
	}
	c->xtitle="p_{T}^{Reco}";
	c->setYrange(0.8, 1.2);
	c->draw();
	TString folder = folderPath+name+"_QAs/"+cname;
	c->SaveAs(folder+"_JEC"+format);
}
*/

void bTaggerStep2Analyzer::calculateSF_Data(int ncsv, float xmin, float xmax){
	matrixTH1Ptr *m2stat = new matrixTH1Ptr("scaleFactor/m2Stat_S*_C*", 1, ncent);
	matrixTH1Ptr *m2datan = new matrixTH1Ptr("scaleFactor/m2negativeTagger_CSV*_C*", ncsv, ncent);
	m2stat->autoLoad(_dataf);
	m2datan->autoLoad(_dataf);
	m2neg_data = new matrixTH1Ptr("misTagRate_data_CSV*_C*", ncsv, ncent);
	for(int j=0; j<ncent; j++){
		auto htot = ((TH2*)m2stat->at(0,j))->ProjectionX(Form("allJets_data_C%d",j));
		auto hlight = ((TH2*)m2stat->at(0,j))->ProjectionX(Form("lightJets_data_C%d",j),flavorID::udsg+1, flavorID::udsg+1);
		for(int i=0; i<ncsv; ++i){
			auto h2 = ((TH2*)m2datan->at(i,j))->ProjectionX(Form("negTagRate_data_CSV%d_C%d",i,j));
			h2->Divide(h2, htot,1, 1, "B");
			h2->GetXaxis()->SetTitle("p_{T}^{Jet}");
			h2->GetYaxis()->SetTitle("#epsilon^{-}_{data}");
			m2neg_data->add(h2,i,j);
		}
	}
	//m2sf = m2neg_data->clone("m2SF");
	auto c = new multi_pads<fast_pad>(name+"_data_neg", "", ncsv, ncent);
	c->markerSize = 0.8;
	c->marker = 20;
	c->setXrange(xmin, xmax);
	c->doHIarrange=true;
	c->setYrange(0., 1.01);
	c->addm2TH1(m2neg_data);
	c->xtitle="p_{T}^{jet}";
	c->draw("E");
	TString folder = folderPath+name+"_QAs/";
	c->SaveAs(folder+"negTagRate_Data"+format);
	//wf->cd();
	//m2neg_data->write();
}

void bTaggerStep2Analyzer::calculateSF_final(int ncsv, float xmin, float xmax){
	m2sf = m2neg_data->divide(*m2neg);
	//m2mis_data = m2sf;
	m2mis_data = m2sf->multiply(*m2mis);
	for(int j=0; j<ncent; j++){
		for(int i=0; i<ncsv; ++i){
			m2sf->at(i,j)->GetYaxis()->SetTitle("SF_{light}");
			m2mis_data->at(i,j)->GetYaxis()->SetTitle("#epsilon_{data}^{mis}");
		}
	}
	TString folder = folderPath+name+"_QAs/";
	//auto c1 = new multi_pads<base_pad>(name+"_SF", "", ncsv, ncent);
	auto c1 = new multi_pads<fast_pad>(name+"_SF", "", ncsv, ncent);
	c1->setXrange(xmin, xmax);
	c1->doHIarrange=true;
	c1->setYrange(.0, 2.0);
	c1->addm2TH1(m2sf);
	c1->xtitle="p_{T}^{jet}";
	c1->draw();
	c1->SaveAs(folder+"Slight"+format);
	auto c2 = new multi_pads<fast_pad>(name+"_misTag_data", "", ncsv, ncent);
	c2->setXrange(xmin, xmax);
	c2->doHIarrange=true;
	c2->setYrange(.0, 1.0);
	c2->addm2TH1(m2mis_data);
	c2->xtitle="p_{T}^{jet}";
	c2->draw();
	c2->SaveAs(folder+"misTagRate_data"+format);
	wf->cd();
	m2sf->setName("SlightFactor");
	m2sf->write();
	m2mis_data->setName("misTagRate_Data");
	m2mis_data->write();
	wf->Close();
}

void bTaggerStep2Analyzer::calculateSF_MC(int ncsv, float xmin, float xmax){
	matrixTH1Ptr *m2mcn = new matrixTH1Ptr("scaleFactor/m2negativeTagger_CSV*_C*", ncsv, ncent);
	matrixTH1Ptr *m2mcw = new matrixTH1Ptr("scaleFactor/m2workingTagger_CSV*_C*", ncsv, ncent);
	matrixTH1Ptr *m2stat = new matrixTH1Ptr("scaleFactor/m2Stat_S*_C*", 1, ncent);
	m2stat->autoLoad(_mcf);
	m2mcn->autoLoad(_mcf);
	m2mcw->autoLoad(_mcf);
	m2mis = new matrixTH1Ptr("misTagRate_MC_CSV*_C*", ncsv, ncent);
	m2neg = new matrixTH1Ptr("negTagRate_MC_CSV*_C*", ncsv, ncent);
	m2R = new matrixTH1Ptr("Rlight_CSV*_C*", ncsv, ncent);
	for(int j=0; j<ncent; j++){
		auto htot = ((TH2*)m2stat->at(0,j))->ProjectionX(Form("allJets_C%d",j));
		auto hlight = ((TH2*)m2stat->at(0,j))->ProjectionX(Form("lightJets_C%d",j),flavorID::udsg+1, flavorID::udsg+1);
		for(int i=0; i<ncsv; ++i){
			auto h1 = ((TH2*)m2mcw->at(i,j))->ProjectionX(Form("misTagged_CSV%d_C%d",i,j), flavorID::udsg+1, flavorID::c+1);
			auto htag = ((TH2*)m2mcw->at(i,j))->ProjectionX(Form("tagged_CSV%d_C%d",i,j), flavorID::udsg+1, flavorID::b+1);
			h1->GetXaxis()->SetTitle("p_{T}^{Jet}");
			h1->GetYaxis()->SetTitle("#epsilon^{miss}_{MC}");
			h1->Divide(h1, htag, 1, 1, "B");
			m2mis->add(h1,i,j);
			auto h2 = ((TH2*)m2mcn->at(i,j))->ProjectionX(Form("negTagRate_CSV%d_C%d",i,j));
			h2->GetXaxis()->SetTitle("p_{T}^{Jet}");
			h2->GetYaxis()->SetTitle("#epsilon^{-}_{MC}");
			h2->Divide(h2, htot,1, 1, "B");
			m2neg->add(h2,i,j);
			auto h3 = (TH1D*)h1->Clone(Form("Rlight_CSV%d_C%d",i,j));
			h3->Divide(h2);
			h3->GetXaxis()->SetTitle("p_{T}^{Jet}");
			h3->GetYaxis()->SetTitle("R_{light}");
			m2R->add((TH1*) h3,i,j);
		}
	}
	auto c = new multi_pads<base_pad>(name, "", ncsv, ncent);
	c->setXrange(xmin, xmax);
	c->doHIarrange=true;
	c->setYrange(0., 2);
	c->addm2TH1(m2R);
	c->xtitle="p_{T}^{jet}";
	c->draw();
	TString folder = folderPath+name+"_QAs/";
	c->SaveAs(folder+"Rlight"+format);
	auto c2 = new multi_pads<fast_pad>(name+"_mis_MC", "", ncsv, ncent);
	c2->setXrange(xmin, xmax);
	c2->doHIarrange=true;
	c2->setYrange(0., 1.01);
	c2->addm2TH1(m2mis);
	c2->xtitle="p_{T}^{jet}";
	c2->draw();
	c2->SaveAs(folder+"misTagRate_MC"+format);
	auto c3 = new multi_pads<fast_pad>(name+"_neg_MC", "", ncsv, ncent);
	c3->setXrange(xmin, xmax);
	c3->doHIarrange=true;
	c3->setYrange(0., 1.01);
	c3->addm2TH1(m2neg);
	c3->xtitle="p_{T}^{jet}";
	c3->draw();
	c3->SaveAs(folder+"negTagRate_MC"+format);
	if(wf!= nullptr) wf->Close();
	wf= TFile::Open(output+"/scaleFactor.root","recreate");
	wf->cd();
	m2R->write();
	m2mis->write();
	m2neg->write();
}

multi_pads<stack_pad>* bTaggerStep2Analyzer::flavorFracPlot(TString name){
	ncent = cent->nbins;
	TString name0  = name;
	name0.ReplaceAll("/","_");
	name0 = name0.ReplaceAll("*","_flavorFrac_stack");
	auto c = new multi_pads<stack_pad>(name0, "", 1, ncent);
	TString hname0 = name.ReplaceAll("*","%d");
	for(int i=0; i< ncent; ++i){
		TString hname = Form(hname0,i);
		//		cent->addCentLabel(i);
		//		cout<<srmc[hname.Data()]->GetName()<<endl;
		TString name1 = "flavor_"+hname;
		auto h = (TH2*) srmc[hname.Data()]->Clone(name1);
		for(int i= 1; i<h->GetNbinsX()+1; ++i){
			int njet = 0;
			for(int j=1; j<h->GetNbinsY()+1; ++j){	
				njet += h->GetBinContent(i,j);	
			}
			for(int j=1; j<h->GetNbinsY()+1; ++j){	
				h->SetBinContent(i,j,h->GetBinContent(i,j)/njet);	
			}
		}
		c->at(0,ncent-1-i)->projection((TH2*) h, "x", 1,3);
		if(i==ncent-1) c->at(0,i)->sp->addLegend();
//		auto h = ((TH2*)srdata[hname.Data()])->ProjectionX();
//		c->at(0,ncent-1-i)->addReference(h, "data");
		//c->at(0,ncent-1-i)->doNorm = 1;
	}
	c->doLegend = 1;
	c->at(0,0)->sp->addLabel(0, "usdg");
	c->at(0,0)->sp->addLabel(1, "c");
	c->at(0,0)->sp->addLabel(2, "b");
	return c;
}

multi_pads<stack_pad>* bTaggerStep2Analyzer::addStackPlot(TString name, int rebin){
	ncent = cent->nbins;
	TString name0  = name;
	name0.ReplaceAll("/","_");
	name0 = name0.ReplaceAll("*","_stack");
	auto c = new multi_pads<stack_pad>(name0, "", 1, ncent);
	TString hname0 = name.ReplaceAll("*","%d");
	for(int i=0; i< ncent; ++i){
		TString hname = Form(hname0,i);
		//		cent->addCentLabel(i);
		//		cout<<srmc[hname.Data()]->GetName()<<endl;
		if(rebin > 1){
			((TH2*)srmc  [hname.Data()])->Rebin2D(rebin, 1);
			((TH2*)srdata[hname.Data()])->Rebin2D(rebin, 1);
		}
		c->at(0,ncent-1-i)->projection((TH2*) srmc[hname.Data()], "x", 1,3);
		if(i==ncent-1) c->at(0,i)->sp->addLegend();
		auto h = ((TH2*)srdata[hname.Data()])->ProjectionX();
		c->at(0,ncent-1-i)->addReference(h, "data");
		c->at(0,ncent-1-i)->doNorm = 1;
		c->at(0,ncent-1-i)->ratio_title = "MC/Data";
		c->at(0,ncent-1-i)->doRatio = 1;
	}
	c->doLegend = 1;
	c->at(0,0)->sp->addLabel(0, "usdg");
	c->at(0,0)->sp->addLabel(1, "c");
	c->at(0,0)->sp->addLabel(2, "b");
	return c;
}

void bTaggerStep2Analyzer::addCentLabel(TCanvas* c){
	TLatex text;
	for(int i=0; i<ncent; ++i){
		c->cd(ncent-i);
		text.DrawLatexNDC(.22, .9, cent->centLabel[i]);
		//cent->addCentLabel(i,.25, .9);
	}
}

void bTaggerStep2Analyzer::tagBias(){
	auto hbjet_eta     = new jtcTH1Player("hbjet_eta", 1, ncent);
	auto hjet_eta      = new jtcTH1Player("hjet_eta", 1, ncent);
	auto hbjetTag_eta  = new jtcTH1Player("hbjetTag_eta", 1, ncent);
	auto hbjet_phi     = new jtcTH1Player("hbjet_phi", 1, ncent);
	auto hjet_phi      = new jtcTH1Player("hjet_phi", 1, ncent);
	auto hbjetTag_phi  = new jtcTH1Player("hbjetTag_phi", 1, ncent);
	auto hbjet_pt     = new jtcTH1Player("hbjet_pt", 1, ncent);
	auto hjet_pt      = new jtcTH1Player("hjet_pt", 1, ncent);
	auto hbjetTag_pt  = new jtcTH1Player("hbjetTag_pt", 1, ncent);
	for(int i=0; i< ncent; ++i){
		TString hname = Form("hjet_eta_C%d",i);
		srmc[hname.Data()]->Scale(1.0/srmc[hname.Data()]->Integral());
		hjet_eta->add(srmc[hname.Data()], 0, i);
		hname = Form("hbjet_eta_C%d",i);
		srmc[hname.Data()]->Scale(1.0/srmc[hname.Data()]->Integral());
		hbjet_eta->add(srmc[hname.Data()], 0, i);
		hname = Form("hbjetTag_eta_C%d",i);
		srmc[hname.Data()]->Scale(1.0/srmc[hname.Data()]->Integral());
		hbjetTag_eta->add(srmc[hname.Data()], 0, i);

		hname = Form("hbjet_phi_C%d",i);
		srmc[hname.Data()]->Scale(1.0/srmc[hname.Data()]->Integral());
		hbjet_phi->add(srmc[hname.Data()], 0, i);
		hname = Form("hjet_phi_C%d",i);
		srmc[hname.Data()]->Scale(1.0/srmc[hname.Data()]->Integral());
		hjet_phi->add(srmc[hname.Data()], 0, i);
		hname = Form("hbjetTag_phi_C%d",i);
		srmc[hname.Data()]->Scale(1.0/srmc[hname.Data()]->Integral());
		hbjetTag_phi->add(srmc[hname.Data()], 0, i);

		hname = Form("hbjet_pt_C%d",i);
		srmc[hname.Data()]->Scale(1.0/srmc[hname.Data()]->Integral());
		hbjet_pt->add(srmc[hname.Data()], 0, i);
		hname = Form("hjet_pt_C%d",i);
		srmc[hname.Data()]->Scale(1.0/srmc[hname.Data()]->Integral());
		hjet_pt->add(srmc[hname.Data()], 0, i);
		hname = Form("hbjetTag_pt_C%d",i);
		srmc[hname.Data()]->Scale(1.0/srmc[hname.Data()]->Integral());
		hbjetTag_pt->add(srmc[hname.Data()], 0, i);
	}
	TString folder = folderPath+name+"_QAs/";
	auto c = new multi_pads<base_pad>("biasStudy_jet_pt", "", 1, ncent);
	c->doHIarrange = 1;
	c->xtitle = "p_{T}^{jet}";
	c->ytitle = "#frac{dN}{dp_{T}}";
	c->addm2TH1(hjet_pt);
	c->addm2TH1(hbjet_pt);
	c->addm2TH1(hbjetTag_pt);
	c->addLegend("upperright");
	c->labelHist("incl. jet",0);
	c->labelHist("b jet", 1);
	c->labelHist("tagged b jet",2);
	c->doLogy = 1;
	c->draw();
	c->SaveAs(folder+"biasStudy_jetpt"+format);

	auto c2 = new multi_pads<fast_pad>("biasStudy_jet_phi", "", 1, ncent);
	c2->doHIarrange = 1;
	c2->xtitle = "#phi^{jet}";
	c2->ytitle = "#frac{dN}{d#phi^{jet}}";
	c2->setYrange(0,0.1);
	c2->addm2TH1(hjet_phi);
	c2->addm2TH1(hbjet_phi);
	c2->addm2TH1(hbjetTag_phi);
	c2->addLegend("upperright");
	c2->labelHist("incl. jet",0);
	c2->labelHist("b jet", 1);
	c2->labelHist("tagged b jet",2);
	c2->draw();
	c2->SaveAs(folder+"biasStudy_jetphi"+format);

	c2 = new multi_pads<fast_pad>("biasStudy_jet_eta", "", 1, ncent);
	c2->doHIarrange = 1;
	c2->xtitle = "#eta^{jet}";
	c2->ytitle = "#frac{dN}{d#eta^{jet}}";
	c2->setYrange(0,0.1);
	c2->addm2TH1(hjet_eta);
	c2->addm2TH1(hbjet_eta);
	c2->addm2TH1(hbjetTag_eta);
	c2->addLegend("upperright");
	c2->labelHist("incl. jet",0);
	c2->labelHist("b jet", 1);
	c2->labelHist("tagged b jet",2);
	c2->draw();
	c2->SaveAs(folder+"biasStudy_jeteta"+format);

}

void bTaggerStep2Analyzer::drawQAs(){
	TString folder = folderPath+name+"_QAs/";
	const int dir_err = system("mkdir -p "+folder);
	auto c = addStackPlot("jtpt_C*");
	c->doLogy = 1; c->xtitle = "p_{T}^{jet}"; c->ytitle="#frac{1}{N} #frac{dN}{dx}";
	c->setYrange(1e-7,1e0);
	c->setXrange(120.,499);
	c->setRatioYrange(0,2);
	c->draw();  addCentLabel(c);
	c->SaveAs(folder+"jtpt"+format);

	c = addStackPlot("jteta_C*");
	c->doLogy = 0; c->xtitle = "#eta^{jet}"; c->ytitle="#frac{1}{N} #frac{dN}{dx}";
	c->setXrange(-2.,2);
	c->setYrange(1e-1,0.5);
	c->draw();  addCentLabel(c);
	c->SaveAs(folder+"jteta"+format);

	c = addStackPlot("jtphi_C*");
	c->doLogy = 0; c->xtitle = "#phi^{jet}"; c->ytitle="#frac{1}{N} #frac{dN}{dx}";
	c->setYrange(.0,0.5);
	c->draw();  addCentLabel(c);
	c->SaveAs(folder+"jtphi"+format);

	c = addStackPlot("wTagger_C*");
	c->doLogy = 1; c->xtitle = "CSV_{jet}"; c->ytitle="#frac{1}{N} #frac{dN}{dx}";
	c->setYrange(1e-2, 5e3);
	c->setRatioYrange(0,2);
	c->draw();  addCentLabel(c);
	c->SaveAs(folder+"wTagger"+format);

	c = addStackPlot("nTagger_C*");
	c->doLogy = 1; c->xtitle = "neg CSV_{jet}"; c->ytitle="#frac{1}{N} #frac{dN}{dx}";
	c->setYrange(1e-2, 5e3);
	c->setRatioYrange(0,2);
	c->draw();  addCentLabel(c);
	c->SaveAs(folder+"nTagger"+format);

	c = addStackPlot("pTagger_C*");
	c->doLogy = 1; c->xtitle = "pos CSV_{jet}"; c->ytitle="#frac{1}{N} #frac{dN}{dx}";
	c->setYrange(1e-2, 5e3);
	c->setRatioYrange(0,2);
	c->draw();  addCentLabel(c);
	c->SaveAs(folder+"pTagger"+format);

	c = addStackPlot("QAs/hsvtxm_C*", 4);
	c->doLogy = 1; c->xtitle = "SV mass"; c->ytitle="#frac{1}{N} #frac{dN}{dx}";
	c->setYrange(1e-7, 11);
	c->setRatioYrange(0,2);
	c->draw();  addCentLabel(c);
	c->SaveAs(folder+"QAsvtmass"+format);

	c = addStackPlot("QAs/hsvtxdls_C*", 4);
	c->doLogy = 1; c->xtitle = "SV distance significance"; c->ytitle="#frac{1}{N} #frac{dN}{dx}";
	c->setYrange(1e-7, 1e1);
	c->setRatioYrange(0,3);
	c->draw();  addCentLabel(c);
	c->SaveAs(folder+"QAsvtxdls"+format);

	c = addStackPlot("QAs/htrk3dIP_C*");
	c->doLogy = 1; c->xtitle = "track 3D IP"; c->ytitle="#frac{1}{N} #frac{dN}{dx}";
	c->setYrange(5e-3, 1e4);
	c->setRatioYrange(0,2);
	c->draw();  addCentLabel(c);
	c->SaveAs(folder+"QAtrk3dIP"+format);

	c = addStackPlot("QAs/htrk3dIPSig_C*");
	c->doLogy = 1; c->xtitle = "track 3D IP Significance"; c->ytitle="#frac{1}{N} #frac{dN}{dx}";
	c->setYrange(1e-5, 1e1);
	c->setRatioYrange(0,2);
	c->draw();  addCentLabel(c);
	c->SaveAs(folder+"QAtrk3dIPSig"+format);

	c = addStackPlot("QAs/htrkPtRel_C*");
	c->doLogy = 1; c->xtitle = "p_{T}^{Rel}"; c->ytitle="#frac{1}{N} #frac{dN}{dx}";
	c->setYrange(1e-5, 1e1);
	c->setRatioYrange(0,2);
	c->draw();  addCentLabel(c);
	c->SaveAs(folder+"QAtrkPtRel"+format);

	c = addStackPlot("QAs/htrkMul_C*");
	c->doLogy = 1; c->xtitle = "n_{trk}^{jet}"; c->ytitle="#frac{1}{N} #frac{dN}{dx}";
	c->setYrange(1e-5, 1e1);
	c->setRatioYrange(0,2);
	c->draw();  addCentLabel(c);
	c->SaveAs(folder+"QAtrkMul"+format);

	c = addStackPlot("QAs/htrkDist_C*");
	c->doLogy = 1; c->xtitle = "track distant"; c->ytitle="#frac{1}{N} #frac{dN}{dx}";
	c->setYrange(1e-5, 1e1);
	c->setRatioYrange(0,2);
	c->draw();  addCentLabel(c);
	c->SaveAs(folder+"QAtrkDist"+format);

	c = addStackPlot("QAs/htrkMomentum_C*");
	c->doLogy = 1; c->xtitle = "p^{trk}"; c->ytitle="#frac{1}{N} #frac{dN}{dx}";
	c->setYrange(1e-2, 1e1);
	c->setRatioYrange(0.5,1.5);
	c->draw();  addCentLabel(c);
	c->SaveAs(folder+"QAtrkMomentum"+format);

	c = addStackPlot("QAs/htrkDr_C*");
	c->doLogy = 1; c->xtitle = "#Deltar"; c->ytitle="#frac{1}{N} #frac{dN}{dx}";
	c->setYrange(3e1, 3e2);
	c->setRatioYrange(0.5,1.5);
	c->setXrange(0, .5);
	c->draw();  addCentLabel(c);
	c->SaveAs(folder+"QAtrkDr"+format);

	c = flavorFracPlot("jtpt_C*");
	c->doLogy = 0; c->xtitle = "p_{T}^{jet}"; c->ytitle="#frac{1}{N} #frac{dN}{dx}";
	c->setYrange(0,1);
	c->setRatioYrange(0,2);
	c->draw();  addCentLabel(c);
	c->SaveAs(folder+"flavorFrac_pt"+format);

}

