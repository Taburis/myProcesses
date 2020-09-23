#ifndef QATools_H
#define QATools_H

#include "TMath.h"
#include "myProcesses/jtc/plugin/xTagger.h"
#include "myProcesses/jtc/plugin/histManager.h"
#include "myProcesses/jtc/plugin/jtcTH1Player.h"
#include "myProcesses/jtc/plugin/plotLib.h"
#include "myProcesses/jtc/plugin/jtcUti.h"
#include "myProcesses/jtc/plugin/toolkit.h"

class jetQASet {
	public :
		jetQASet(){}
		jetQASet(TString name0, int tag0, int ncent, histManager *hm){
			init(name0, tag0, ncent, hm);
		}
		void init(TString name0, int tag0, int ncent0, histManager *hm){
			ncent = ncent0;
			jetpt  = new TH1D*[ncent];
			jeteta = new TH1D*[ncent];
			jetphi = new TH1D*[ncent];
			bit.setTag(tag0);
			for(int i=0; i<ncent; i++){
				jetpt [i] = hm->regHist<TH1D>(name0+"_jetQASet/"+name0+Form("_pt_%d",i),"",400, 100, 500);
				jeteta[i] = hm->regHist<TH1D>(name0+"_jetQASet/"+name0+Form("_eta_%d",i),"",200, -2,2);
				jetphi[i] = hm->regHist<TH1D>(name0+"_jetQASet/"+name0+Form("_phi_%d",i),"",200, -TMath::Pi(),TMath::Pi());
			}
		}
		~jetQASet();

		void fillHist(xTagger &tag, int jcent, float pt, float eta, float phi, float weight=1){
			if(!(tag.select(bit))) return;
			//cout<<"filling "<<bit.tag<<" set"<<endl;
			jetpt [jcent]->Fill(pt ,weight);
			jeteta[jcent]->Fill(eta,weight);
			jetphi[jcent]->Fill(phi,weight);
		}

		//		mulit_pads<overlay_pad> * plot_kinematic(TString setname){
		//			auto c = new multi_pads<base_pad>("c_"+setname, "", 3, ncent);
		//		}


		xTagger bit; // the type bit consumed as a selection
		TString name, title;
		TH1D **jetpt, **jeteta, **jetphi;
		int ncent;
};


namespace qaTools {
	multi_pads<overlay_pad> * fig_jet_kinematic(TString set1, TFile *f1, TString set2, TFile *f2, int ncent = 1){
		auto j1 = new jtcTH1Player(set1+"_jetQASet/"+set1, 3, ncent);
		auto j2 = new jtcTH1Player(set2+"_jetQASet/"+set2, 3, ncent);
		for(int i =0; i<ncent; ++i){
			auto h = (TH1D*) f1->Get(set1+"_jetQASet/"+set1+Form("_pt_%d",i));
			
			j1->add(h,0,i);
			h = (TH1D*) f2->Get(set2+"_jetQASet/"+set2+Form("_pt_%d",i));
			j2->add(h,0,i);
			h = (TH1D*) f1->Get(set1+"_jetQASet/"+set1+Form("_eta_%d",i));
			j1->add(h,1,i);
			h = (TH1D*) f2->Get(set2+"_jetQASet/"+set2+Form("_eta_%d",i));
			j2->add(h,1,i);
			h = (TH1D*) f1->Get(set1+"_jetQASet/"+set1+Form("_phi_%d",i));
			j1->add(h,2,i);
			h = (TH1D*) f2->Get(set2+"_jetQASet/"+set2+Form("_phi_%d",i));
			j2->add(h,2,i);
		}
		auto c = new multi_pads<overlay_pad>("c_"+set1+"_vs_"+set2,"", 3, ncent);
		c->doHIarrange = 1;
		c->addLegend("upperright");
		c->addm2TH1(j1);
		c->labelHist(set1,0);
		c->addm2TH1(j2);
		c->labelHist(set2,1);
		c->setRatioYrange(0.0, 1.2);
		for(int i=0; i<ncent; i++){
			//c->fpads.at(i,0)->doLogy=1;	
			c->setLogy(0,i);
		}
		return c;
	}

	multi_pads<base_pad> *fig_evtInfo_vz_cent(TFile *fdata, TFile *fmc){
		auto j1 = new jtcTH1Player("evtInfo_data", 1, 2);
		auto j2 = new jtcTH1Player("evtInfo_mc", 1, 2);
		for(int i=0; i<2; ++i){
			auto h = (TH1D*) fdata->Get("vzInfo");
			int n1 = h->FindBin(-15);
			int n2 = h->FindBin(14.99);
			h->Scale(1.0/h->Integral(n1,n2));
			h->GetXaxis()->SetTitle("V_z");
			j1->add(h,0,1);
			h = (TH1D*) fmc->Get("hvz");
			h->Scale(1.0/h->Integral(n1,n2));
			j2->add(h,0,1);
			h = (TH1D*) fdata->Get("centInfo");
			h->GetXaxis()->SetTitle("hiBin");
			h->Scale(1.0/h->Integral());
			toolkit::divide_bin_size(h);
			j1->add(h,0,0);
			h = (TH1D*) fmc->Get("hcent");
			h->Scale(1.0/h->Integral());
			toolkit::divide_bin_size(h);
			j2->add(h,0,0);
		}
		auto c = new multi_pads<base_pad>("c_evtInfo", "", 1, 2);
		c->addm2TH1(j1);
		c->addm2TH1(j2);
		c->addLegend("upperright");
		c->labelHist("Data",0);
		c->labelHist("MC" ,1);
		return c;
	
	}
}


#endif
