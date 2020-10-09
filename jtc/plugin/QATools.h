#ifndef QATools_H
#define QATools_H

#include "TMath.h"
#include "myProcesses/liteFrame/plugin/xTagger.h"
#include "myProcesses/liteFrame/plugin/histManager.h"
#include "myProcesses/liteFrame/plugin/toolkit.h"
#include "myProcesses/jtc/plugin/jtcTH1Player.h"
#include "myProcesses/jtc/plugin/plotLib.h"
#include "myProcesses/jtc/plugin/jtcUti.h"

template <typename th1>
class qaTSetFlavor{
	public :
		qaTSetFlavor(){}
		qaTSetFlavor(TString name, int ncent, TFile *f, bool ismc){
			load(name, ncent, f, ismc);
		}
		qaTSetFlavor(TString name, TString title, histManager *hm, int tag, int ncent, const float *centbin, int nbin, float min, float max, bool ismc= 0){
			init(tag, ncent, centbin, hm, ismc); 
			initHist(name,title, nbin, min, max); 
		}
		qaTSetFlavor(TString name, TString title, histManager *hm, int tag, int ncent, const float *centbin, int nbin, float *bins, bool ismc= 0){
			init(tag, ncent, centbin, hm, ismc); 
			initHist(name,title, nbin, bins); 
		}
		~qaTSetFlavor(){}

		void normalize(){
			if(onMC) hist_mc = new th1*[ncent];
			for(int i=0; i<ncent;i++){
				if(onMC){
					hist_mc[i] = (th1*)hist_l[i]->Clone(hist_l[i]->GetName()+"_sum");
					hist_mc[i] ->Add(hist_c[i]);
					hist_mc[i] ->Add(hist_b[i]);
					hist_l[i]->Scale(1.0/hist_mc[i]->Integral());
					hist_c[i]->Scale(1.0/hist_mc[i]->Integral());
					hist_b[i]->Scale(1.0/hist_mc[i]->Integral());
				}
				if(onData) hist_data[i]->Scale(1.0/hist_data[i]->Integral());
			}
		}
		void load(TString name, int ncent0, TFile *f, bool ismc){
			if(ismc) onMC = 1;
			else onData=1;
			ncent = ncent0;
			if(ismc){
				hist_l = new th1*[ncent];
				hist_c = new th1*[ncent];
				hist_b = new th1*[ncent];
			}else{
				hist_data = new th1*[ncent];	
			}
			for(int i=0; i<ncent;i++){
				if(ismc){
					hist_l[i]  = (th1*) f->Get("bTaggingQA/"+name+Form("_udsg_C%d",i));
					hist_c[i]  = (th1*) f->Get("bTaggingQA/"+name+Form("_c_C%d",i));
					hist_b[i]  = (th1*) f->Get("bTaggingQA/"+name+Form("_b_C%d",i));
				}else {
					hist_data[i]= (th1*) f->Get("bTaggingQA/"+name+Form("_data_C%d",i));
				}
			}
		}

		void init(int tag0, int ncent0, const float *centbin, histManager*hm0, bool ismc =0 ){
			hm = hm0;
			if(ismc) onMC = 1;
			else onData=1;
			ncent = ncent0;
			ax = new xAxis(ncent, centbin);
			centLabel=toolkit::makeCentLabels(ncent, centbin);
			if(ismc){
				hist_l = new th1*[ncent];
				hist_c = new th1*[ncent];
				hist_b = new th1*[ncent];
			}else{
				hist_data = new th1*[ncent];	
			}
		}
		void initHist(TString name, TString title, int nbin, float min, float max){
			for(int i=0; i<ncent;i++){
				if(onMC){
					hist_l[i] = hm->regHist<th1>(Form(name+"_udsg_C%d",i), title+":"+centLabel[i], nbin, min, max);
					hist_c[i] = hm->regHist<th1>(Form(name+"_c_C%d",i), title+":"+centLabel[i], nbin, min, max);
					hist_b[i] = hm->regHist<th1>(Form(name+"_b_C%d",i), title+":"+centLabel[i], nbin, min, max);
				}else {
					hist_data[i] = hm->regHist<th1>(Form(name+"_data_C%d",i), title+":"+centLabel[i], nbin, min, max);
				}
			}
		}

		void initHist(TString name, TString title, int nbin, float* bins){
			for(int i=0; i<ncent;i++){
				if(onMC){
					hist_l[i] = hm->regHist<th1>(Form(name+"_udsg_C%d",i), title+":"+centLabel[i], nbin, bins);
					hist_c[i] = hm->regHist<th1>(Form(name+"_c_C%d",i), title+":"+centLabel[i], nbin, bins);
					hist_b[i] = hm->regHist<th1>(Form(name+"_b_C%d",i), title+":"+centLabel[i], nbin, bins);
				}else {
					hist_data[i] = hm->regHist<th1>(Form(name+"_data_C%d",i), title+":"+centLabel[i], nbin, bins);
				}
			}
		}

		void fill(xTagger &bit, float hibin, float data, float weight = 1, int flavor=0){
			if(!(bit.select(tag))) return;
			int jcent = ax->find_bin_in_range(hibin);
			if(jcent<0) return;
			if(!onMC && flavor == 0){
				hist_data[jcent]->Fill(data, weight);
			}
			else if(flavor == 1) hist_l[jcent]->Fill(data, weight);
			else if(flavor == 2) hist_c[jcent]->Fill(data, weight);
			else if(flavor == 3) hist_b[jcent]->Fill(data, weight);
			return;
		}


		int ncent;
		th1 **hist_l,  **hist_c, **hist_b, **hist_mc, **hist_data;
		xTagger tag;
		xAxis * ax;
		bool onMC = 0 , onData =0;
		TString *centLabel;
		histManager *hm;
};

class jetQASet {
	public :
		jetQASet(){}
		jetQASet(TString dirname, TString name0, int tag0, int ncent, histManager *hm){
			init(dirname, name0, tag0, ncent, hm);
		}
		void init(TString dirname, TString name0, int tag0, int ncent0, histManager *hm){
			ncent = ncent0;
			jetpt  = new TH1D*[ncent];
			jeteta = new TH1D*[ncent];
			jetphi = new TH1D*[ncent];
			bit.setTag(tag0);
			for(int i=0; i<ncent; i++){
				jetpt [i] = hm->regHist<TH1D>(dirname+"_jetQASet/"+name0+Form("_pt_%d",i), "",400, 100, 500);
				jeteta[i] = hm->regHist<TH1D>(dirname+"_jetQASet/"+name0+Form("_eta_%d",i),"",200, -2,2);
				jetphi[i] = hm->regHist<TH1D>(dirname+"_jetQASet/"+name0+Form("_phi_%d",i),"",200, -TMath::Pi(),TMath::Pi());
			}
		}
		~jetQASet(){};

		void load(TFile *f, TString dirname, TString name0, int tag0, int ncent0){
			ncent = ncent0;
			jetpt  = new TH1D*[ncent];
			jeteta = new TH1D*[ncent];
			jetphi = new TH1D*[ncent];
			for(int i=0; i<ncent; i++){
				jetpt [i] =(TH1D*) f->Get(dirname+"_jetQASet/"+name0+Form("_pt_%d",i));
				jeteta[i] =(TH1D*) f->Get(dirname+"_jetQASet/"+name0+Form("_eta_%d",i));
				jetphi[i] =(TH1D*) f->Get(dirname+"_jetQASet/"+name0+Form("_phi_%d",i));
			}

		}

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
