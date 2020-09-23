
#ifndef MVATREE_H
#define MVATREE_H

#include "myProcesses/jtc/plugin/histManager.h"
#include "myProcesses/jtc/plugin/plotLib.h"

const Double_t trkPtBin[] = {0.5, 1, 2, 3, 4, 8, 12, 30};
const int ntrkPtBin = 7;

class mvaTree {
	public :
		mvaTree(){}
		mvaTree(TString name): pro_name(name) {}
		~mvaTree(){}
		void init(TFile *f, bool test = 1){
			TString tname = "TestTree";
			if(!test) tname = "TrainTree";
			t=(TTree*) f->Get("dataset/"+tname);
			t->SetBranchAddress("classID",&classID);	
			t->SetBranchAddress("BDTG",&BDTGValue);	
			t->SetBranchAddress("pt",&pt);	
			t->SetBranchAddress("eta",&eta);	
		}


		void fillQA(){
			hm = new histManager();
			heta_sig = hm->regHist<TH2D>(pro_name+"/heta_sig", "true track eta distribution", 50, -2.5, 2.5, 220, -1.1, 1.1);	
			heta_bkg = hm->regHist<TH2D>(pro_name+"/heta_bkg", "fake track eta distribution", 50, -2.5, 2.5, 220,-1.1, 1.1);	
			hpt_sig = hm->regHist<TH2D>(pro_name+"/hpt_sig", "true track pt distribution", ntrkPtBin, trkPtBin, 220, -1.1, 1.1);	
			hpt_bkg = hm->regHist<TH2D>(pro_name+"/hpt_bkg", "fake track pt distribution", ntrkPtBin, trkPtBin, 220,-1.1, 1.1);	

			hm->sumw2();

			Long64_t nevt = t->GetEntries();
			for(auto jevt = 0; jevt< nevt; jevt++){
				t->GetEntry(jevt);
				if(fabs(eta) > etamax) continue;

				if(pt < trkPtBin[0]) continue;
				if( pt > trkPtBin[ntrkPtBin]) pt = 29;
				if(classID == 0){
					hpt_sig->Fill(pt, BDTGValue);
				}else {
					hpt_bkg->Fill(pt, BDTGValue);
				}
				if(pt < ptmin) continue;
				if(classID == 0){
					heta_sig->Fill(eta, BDTGValue);
				}else {
					heta_bkg->Fill(eta, BDTGValue);
				}
			}
		}

		void write(TString output="mvaQAScan.root"){
			auto wf = TFile::Open(output,"recreate");
			wf->cd();
			hm->write(wf);
			wf->Close();
		}

		void analyze(float loose, float tight, float highPurity, TString output ="."){
			system("mkdir -p "+output+"/"+pro_name);
			TString path = output+"/"+pro_name;
			hbdt_sig = (TH1D*) heta_sig->ProjectionY();
			hbdt_bkg = (TH1D*) heta_bkg->ProjectionY();
			heta_tot = (TH1D*) heta_sig->ProjectionX();
			auto h  = (TH1D*) heta_bkg->ProjectionX();
			heta_tot->Add(h);
			float nn = heta_tot->Integral();
			hbdt_sig->Scale(1.0/nn);
			hbdt_bkg->Scale(1.0/nn);
			//hbdt_sig->Scale(1.0/hbdt_sig->Integral());
			//hbdt_bkg->Scale(1.0/hbdt_bkg->Integral());

			hpt_tot = (TH1D*) hpt_sig->ProjectionX();
			h = hpt_bkg->ProjectionX();
			hpt_tot->Add(h);

			project(loose);
			project(tight);
			project(highPurity);
			plot(path);
		}

		void project(float cut = -1.0){
			//projection with bdtg cut
			int bound = heta_sig->GetYaxis()->FindBin(cut);
			int max = heta_sig->GetNbinsY();
			int n = heff_eta.size();
			TH1D* h = heta_sig->ProjectionX(pro_name+Form("_heff_eta_%d",n), bound, max);
			h->SetTitle(Form("Efficiency: MVA value > %f",cut));
			h->Divide(h,heta_tot, 1,1, "B");
			h->GetXaxis()->SetTitle("#eta");
			heff_eta.emplace_back(h);
			h = heta_bkg->ProjectionX(pro_name+Form("_hfake_eta_%d",n), bound, max);
			h->Divide(h,heta_tot, 1,1, "B");
			h->SetTitle(Form("Fake rate: MVA value > %f",cut));
			h->GetXaxis()->SetTitle("#eta");
			hfake_eta.emplace_back(h);

			bound = hpt_sig->GetYaxis()->FindBin(cut);
			n = heff_pt.size();
			h = hpt_sig->ProjectionX(pro_name+Form("_heff_pt_%d",n), bound, max);
			h->SetTitle(Form("Efficiency: MVA value > %f",cut));
			h->Divide(h, hpt_tot, 1,1 ,"B");
			h->GetXaxis()->SetTitle("p_{T}");
			heff_pt.emplace_back(h);
			h = hpt_bkg->ProjectionX(pro_name+Form("_hfake_pt_%d",n), bound, max);
			h->Divide(h, hpt_tot, 1, 1, "B");
			h->SetTitle(Form("Fake rate: MVA value > %f",cut));
			h->GetXaxis()->SetTitle("p_{T}");
			hfake_pt.emplace_back(h);

		}

		void plot(TString path){
			auto csum_eta = new multi_pads<base_pad>(pro_name+"_csum_eta","", 1, 2);
			auto csum_pt = new multi_pads<base_pad>(pro_name+"_csum_pt","", 1, 2);
			auto msum_eta_1 = new matrixTH1Ptr("tracking_eta_sum_loose", 1, 2);
			auto msum_eta_2 = new matrixTH1Ptr("tracking_eta_sum_tight", 1, 2);
			auto msum_eta_3 = new matrixTH1Ptr("tracking_eta_sum_hp", 1, 2);
			auto msum_pt_1 = new matrixTH1Ptr("tracking_pt_sum_loose", 1, 2);
			auto msum_pt_2 = new matrixTH1Ptr("tracking_pt_sum_tight", 1, 2);
			auto msum_pt_3 = new matrixTH1Ptr("tracking_pt_sum_hp", 1, 2);
			/*
			   auto cfak = new multi_pads<base_pad>(pro_name+"_cfake","", 1, 3);
			   auto ceff = new multi_pads<base_pad>(pro_name+"_ceff","", 1, 3);
			   auto meff = new matrixTH1Ptr("tracking_eff", 1, 3);
			   auto mfak = new matrixTH1Ptr("tracking_fak", 1, 3);
			   for(int i=0; i<3; i++){
			   meff->add(heff_eta[i], 0,i);
			   mfak->add(hfake_eta[i], 0,i);
			   }
			   ceff->addm2TH1(meff);
			   ceff->xtitle = "#eta";
			   ceff->draw();
			   ceff->SaveAs(path+"/heff_BDTG.png");
			   cfak->addm2TH1(mfak);
			   cfak->xtitle = "#eta";
			   cfak->draw();
			   cfak->SaveAs(path+"/hfake_BDTG.png");
			   */
			// effeicieny and fake distribution in Eta
			heff_eta [0]->SetTitle(Form("Efficiency: track p_{T} > %f GeV",ptmin));
			hfake_eta [0]->SetTitle(Form("Fake rate: track p_{T} > %f GeV",ptmin));
			msum_eta_1->add(heff_eta [0], 0,0);
			msum_eta_1->add(hfake_eta[0], 0,1);
			msum_eta_2->add(heff_eta [1], 0,0);
			msum_eta_2->add(hfake_eta[1], 0,1);
			msum_eta_3->add(heff_eta [2], 0,0);
			msum_eta_3->add(hfake_eta[2], 0,1);

			csum_eta->doAutoYrange=1;
			csum_eta->addm2TH1(msum_eta_1);
			csum_eta->addm2TH1(msum_eta_2);
			csum_eta->addm2TH1(msum_eta_3);
			csum_eta->addLegend("bottommiddle");
			csum_eta->labelHist("loose",0);
			csum_eta->labelHist("tight",1);
			csum_eta->labelHist("HP",2);
			csum_eta->xtitle = "#eta";
			csum_eta->draw("pe");
			csum_eta->SaveAs(path+"/hsummary_eta.png");

			// effeicieny and fake distribution in PT
			heff_pt [0]->SetTitle(Form("Efficiency: track |#eta| < %f",etamax));
			hfake_pt [0]->SetTitle(Form("Fake rate: track |#eta| < %f",etamax));
			msum_pt_1->add(heff_pt [0], 0,0);
			msum_pt_1->add(hfake_pt[0], 0,1);
			msum_pt_2->add(heff_pt [1], 0,0);
			msum_pt_2->add(hfake_pt[1], 0,1);
			msum_pt_3->add(heff_pt [2], 0,0);
			msum_pt_3->add(hfake_pt[2], 0,1);

			csum_pt->doAutoYrange=1;
			csum_pt->setXrange(0.7,29);
			csum_pt->addm2TH1(msum_pt_1);
			csum_pt->addm2TH1(msum_pt_2);
			csum_pt->addm2TH1(msum_pt_3);
			csum_pt->addLegend("upperright");
			csum_pt->labelHist("loose",0);
			csum_pt->labelHist("tight",1);
			csum_pt->labelHist("HP",2);
			csum_pt->xtitle = "p_{T}";
			csum_pt->draw();
			csum_pt->SaveAs(path+"/hsummary_pt.png");

			// BDTG value distribution
			hbdt_bkg->SetFillStyle(1001);
			hbdt_bkg->SetFillColorAlpha(kRed,0.4);
			hbdt_sig->SetFillStyle(1001);
			hbdt_sig->SetFillColorAlpha(kBlue,0.5);
			auto c = new TCanvas(pro_name+"_cbdtg", "", 500, 500);
			c->SetLeftMargin(0.15);
			c->SetRightMargin(0.04);
			auto l = new TLegend(0.4, 0.65,0.7,0.8);
			l->SetLineColor(kWhite);
			hbdt_sig->SetTitle("BDTG performance");
			hbdt_sig->GetXaxis()->SetTitle("BDTG value");
			hbdt_sig->GetYaxis()->SetTitle("Fraction");
			hbdt_sig->Draw("hist");
			hbdt_bkg->Draw("histsame");
			l->AddEntry(hbdt_sig, "True track", "f");
			l->AddEntry(hbdt_bkg, "Fake track", "f");
			l->Draw();
			gPad->SetLogy();
			c->SaveAs(path+"/BDTG_value.png");

		}

		TH2D *heta_sig, *heta_bkg, *hpt_sig, *hpt_bkg; 
		TH1D* hbdt_bkg, *hbdt_sig, *heta_tot, *hpt_tot;
		std::vector<TH1D*> heff_eta, hfake_eta, heff_pt, hfake_pt;
		histManager *hm;
		TTree* t;
		TString pro_name;
		int classID;
		float BDTGValue, pt, eta, ptmin = 1, etamax = 2.5;
};

#endif
