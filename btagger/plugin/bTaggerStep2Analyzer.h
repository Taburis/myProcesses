
#include "myProcesses/hiforest/plugin/simpleReader.h"
#include "myProcesses/btagger/plugin/bTaggerAnalyzer.h"
#include "myProcesses/btagger/plugin/bTaggerLib.h"
#include "myProcesses/jtc/plugin/PLOTLIB/multi_pads.h"
#include <cstdlib>

using namespace btagger_utility;
class bTaggerStep2Analyzer{
	public : bTaggerStep2Analyzer(TString n): name(n){}
		 ~bTaggerStep2Analyzer(){}
		 void loadMC(TFile *f){srmc.load(f); _mcf = f;}
		 void loadData(TFile *f){srdata.load(f); _dataf = f;}
		 void linkCentralityHelper( centralityHelper *ch ) {
			 cent = ch; ncent = cent->nbins; cent->makeLabels();
		 }

		 void JEC(TString name, TString);
		 void scaleFactorPlot(TString name,TString dir, int ,int );
		 void calculateSF(int ncsv, float xmin, float xmax){
			 calculateSF_MC(ncsv, xmin, xmax);
			 calculateSF_Data(ncsv, xmin, xmax);
		 }
		 void calculateSF_MC(int ncsv, float xmin, float xmax);
		 void calculateSF_Data(int ncsv, float xmin, float xmax);

		 void stackStyle(THStack* h, TString xtitle){
			 h->GetXaxis()->SetTitle(xtitle);
		 }
		 stackPlot* addHistBundle(histBundle &hb){
			 auto sh = new stackPlot("stack_"+hb.name);
			 if(hb.kMC){
				 sh->addLegend();
				 sh->add(hb.hudsg, "udsg");
				 sh->add(hb.hc, "c jet");
				 sh->add(hb.hb, "b jet");
				 sh->defaultColor();
			 }
			 if(hb.kData) sh->addTopHist(hb.hdata, "data");
			 return sh;
		 }
		 multi_pads<fast_pad> *drawQA(TString name, TString hname, TString xtitle, float x, float y, int rebin= 1, bool logy=0){
			 TString name0 = name.ReplaceAll("/","_");
			 ncent = cent->nbins;
			 auto c = new multi_pads<fast_pad>(name0, "", 1, ncent);
			 histBundle hb[4];
			 TString hname2 = hname.ReplaceAll("*","%d");
			 for(int i=0; i< ncent; ++i){
				 c->CD(0, ncent-i-1);
				 if(!srmc.kLoaded && !srdata.kLoaded) continue; 
				 hb[i] = prepareBundle(Form(hname2,i), rebin);
				 auto sh = drawBundle(hb[i], x, y, xtitle, logy);
				 cent->addCentLabel(i);
				 if(i == ncent-1) sh->legend->Draw();
			 }
			 return c;
		 }

		 histBundle prepareBundle(TString hname, int rebin=1){
			 histBundle hb;
			 TH2* hmc;
			 TH2* hda;
			 if(srmc.kLoaded){
				 hmc =(TH2*) srmc[std::string(hname)];
				 projFlavor(hb, hmc,0); hb.kMC =1;}
			 if(srdata.kLoaded){ 
				 hda =(TH2*) srdata[std::string(hname)];
				 projFlavor(hb, hda,1); hb.kData =1;}
			 rebinBundle(hb, rebin);
			 return hb;
		 }

		 stackPlot* drawBundle(histBundle&hb, float x, float y, TString xtitle, bool logy){
			 if(hb.kData) topHist_style(hb.hdata);
			 stackPlot* sh;
			 if(hb.kMC){
				 sh = addHistBundle(hb);
				 sh->normalizeStack();
				 sh->Draw("hist");
				 gPad->SetLogy(logy);
				 sh->GetHistogram()->SetAxisRange(x,y,"X");
				 stackStyle(sh, xtitle);
				 sh->Draw("hist");
			 }
			 if(hb.kData){
				 sh->normalizeOverlayHists();
				 sh->drawHists();
			 }
			 return sh;
		 }

		 void rebinBundle(histBundle &hb, int n){	
			 if(hb.kMC){
				 hb.hudsg->Rebin(n);
				 hb.hc   ->Rebin(n);
				 hb.hb   ->Rebin(n);
			 }
			 if(hb.kData) hb.hdata->Rebin(n);
		 }
		 void topHist_style(TH1* h){
			 h->SetMarkerStyle(20);
			 h->SetMarkerSize(0.7);
			 h->SetMarkerColor(kBlack);
		 }

		 void produceTH2QA(){
			 TString folder = folderPath+name+"_QAs/";
			 const int dir_err = system("mkdir -p "+folder);
			 auto cc = eventWeightCheck();	
			 cout<<"event info. generated"<<endl;
			 cc->SaveAs(folder+"eventWeight"+format);
			 for(int i=0; i<11; i++){
				 TString cname = h2List[i];
				 cname.ReplaceAll("_C*", "_stack");
				 std::cout<<"processing "<<cname<<std::endl;
				 auto c = drawQA(cname, h2List[i], h2xtitle[i], xmin[i], xmax[i], rebin[i], logy[i]);
				 cname=cname.ReplaceAll("/","_");
				 c->SaveAs(folder+cname+format);
			 }
		 }



		 multi_pads<base_pad>* eventWeightCheck(){
			 auto c = new multi_pads<base_pad>("eWcheck", "", 1, 3 );
			 srmc  ["hvz"]  ->Rebin(2);
			 srdata["hvz"]  ->Rebin(2);
			 srmc  ["hcent"]->Rebin(2);
			 srdata["hcent"]->Rebin(2);
			 normalize(srmc["hvz"]);
			 normalize(srmc["hcent"]);
			 normalize(srdata["hvz"]);
			 normalize(srdata["hcent"]);
			 normalize(srmc["hpthat"]);
			 c->add(srmc["hvz"], 0, 0);
			 c->add(srdata["hvz"], 0, 0);
			 c->add(srmc["hcent"], 0, 1);
			 c->add(srdata["hcent"], 0, 1);
			 c->add(srmc["hpthat"], 0, 2);
			 //c->at(0,2)->doLogy=1;
			 c->draw();
			 return c;
		 }


		 TString name;
		 int ncent;
		 centralityHelper *cent;
		 simpleReader srmc, srdata;
		 TString h2List[11] = {"jtpt_C*", "jteta_C*", "jtphi_C*", 
			 "wTagger_C*", "pTagger_C*", "nTagger_C*", 
			 "QAs/hnsvtx_C*", "QAs/hsvtxm_C*", "QAs/hsvtxdl_C*", "QAs/hsvtxdls_C*", "QAs/hsvtxntrk_C*"};
		 TString h2xtitle[11] = {"p_{T}^{jet}", "#eta_{T}^{jet}", "#phi_{T}^{jet}", 
			 "csv Value", "positive csv Value", "negative csv Value",
			 "# of SV", "SV mass", "SV distance", "SV distance significance", "Trk # assoicated to SV"};
		 float xmin[11] ={0, -2,  -3.2, -0.05, -0.05, -0.05, 0,   0,   0,   0, 0};
		 float xmax[11] ={1, 1.99,3.19,  1.05,  1.05,  1.05, 5.9, 9.9, 4.9, 80, 9.9};
		 int  rebin[11] ={1, 1, 2, 2, 2,1,1,1,1,1,1 };
		 bool logy [11] ={1, 0, 0, 1, 1, 1, 1,1, 1, 1,1};
		 TString folderPath = "./";
		 std::vector<TH1*> cleanList;
		 std::vector<histBundle> cleanList_bundle;
		 TFile*_mcf,*_dataf;
		 matrixTH1Ptr *m2mis, *m2neg, *m2R, *m2sf, *m2mis_data, *m2neg_data;
		 TString format = ".png";
};

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
			c->legend->AddEntry(hb, "b-jet");
		}
		htg->SetAxisRange(0.8, 1.2, "Y");
		c->add(htg, 0, ncent-i-1);
		c->add(hb, 0, ncent-i-1);
		cleanList.emplace_back((TH1*)hb);
		cleanList.emplace_back((TH1*)htg);
	}
	c->setYrange(0.8, 1.2);
	c->draw();
	TString folder = folderPath+name+"_QAs/"+cname;
	c->SaveAs(folder+"_JEC"+format);
}

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
	m2sf = m2neg_data->divide(*m2neg);
	for(int j=0; j<ncent; j++){
		for(int i=0; i<ncsv; ++i){
			m2sf->at(i,j)->GetYaxis()->SetTitle("SF_{light}");
		}
	}
	auto c = new multi_pads<fast_pad>(name+"_data_neg", "", ncsv, ncent);
	c->setXrange(xmin, xmax);
	c->doHIarrange=true;
	c->setYrange(0., 1.01);
	c->addm2TH1(m2neg_data);
	c->draw();
	TString folder = folderPath+name+"_QAs/";
	c->SaveAs(folder+"negTagRate_Data"+format);
	auto c1 = new multi_pads<fast_pad>(name+"_SF", "", ncsv, ncent);
	c1->setXrange(xmin, xmax);
	c1->doHIarrange=true;
	c1->setYrange(.0, 2.0);
	c1->addm2TH1(m2sf);
	c1->draw();
	c1->SaveAs(folder+"Slight"+format);
}

void bTaggerStep2Analyzer::calculateSF_MC(int ncsv, float xmin, float xmax){
	matrixTH1Ptr *m2mcn = new matrixTH1Ptr("scaleFactor/m2negativeTagger_CSV*_C*", ncsv, ncent);
	matrixTH1Ptr *m2mcw = new matrixTH1Ptr("scaleFactor/m2workingTagger_CSV*_C*", ncsv, ncent);
	matrixTH1Ptr *m2stat = new matrixTH1Ptr("scaleFactor/m2Stat_S*_C*", 1, ncent);
	m2stat->autoLoad(_mcf);
	m2mcn->autoLoad(_mcf);
	m2mcw->autoLoad(_mcf);
	m2mis = new matrixTH1Ptr("misTagRate_CSV*_C*", ncsv, ncent);
	m2neg = new matrixTH1Ptr("negTagRate_CSV*_C*", ncsv, ncent);
	m2R = new matrixTH1Ptr("Rlight_CSV*_C*", ncsv, ncent);
	for(int j=0; j<ncent; j++){
		auto htot = ((TH2*)m2stat->at(0,j))->ProjectionX(Form("allJets_C%d",j));
		auto hlight = ((TH2*)m2stat->at(0,j))->ProjectionX(Form("lightJets_C%d",j),flavorID::udsg+1, flavorID::udsg+1);
		for(int i=0; i<ncsv; ++i){
			auto h1 = ((TH2*)m2mcw->at(i,j))->ProjectionX(Form("misTagged_CSV%d_C%d",i,j), flavorID::udsg+1, flavorID::udsg+1);
			h1->GetXaxis()->SetTitle("p_{T}^{Jet}");
			h1->GetYaxis()->SetTitle("#epsilon^{miss}_{MC}");
			h1->Divide(h1, hlight, 1, 1, "B");
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
			m2R->add(h3,i,j);
		}
	}
	auto c = new multi_pads<fast_pad>(name, "", ncsv, ncent);
	c->setXrange(xmin, xmax);
	c->doHIarrange=true;
	c->setYrange(0., 2);
	c->addm2TH1(m2R);
	c->draw();
	TString folder = folderPath+name+"_QAs/";
	c->SaveAs(folder+"Rlight"+format);
	auto c2 = new multi_pads<fast_pad>(name+"_mis_MC", "", ncsv, ncent);
	c2->setXrange(xmin, xmax);
	c2->doHIarrange=true;
	c2->setYrange(0., 1.01);
	c2->addm2TH1(m2mis);
	c2->draw();
	c2->SaveAs(folder+"misTagRate_MC"+format);
	auto c3 = new multi_pads<fast_pad>(name+"_neg_MC", "", ncsv, ncent);
	c3->setXrange(xmin, xmax);
	c3->doHIarrange=true;
	c3->setYrange(0., 1.01);
	c3->addm2TH1(m2neg);
	c3->draw();
	c3->SaveAs(folder+"negTagRate_MC"+format);
}

void bTaggerStep2Analyzer::scaleFactorPlot(TString name, TString dir,int np, int nc){
	matrixTH1Ptr *m2mcn = new matrixTH1Ptr(dir+"/m2ndisc"+"_P*_C*", np, nc);
	matrixTH1Ptr *m2mcp = new matrixTH1Ptr(dir+"/m2pdisc"+"_P*_C*", np, nc);

	m2mcn->autoLoad(_mcf);
	m2mcp->autoLoad(_mcf);
	TH1 *h;
	for(int i=0; i<np; ++i){
		for(int j=0; j<nc; j++){
			cleanList_bundle.emplace_back(histBundle());
			histBundle &hbp = cleanList_bundle.back();
			projFlavor(hbp, (TH2*)(m2mcp->at(i,j)),0);	

			cleanList_bundle.emplace_back(histBundle());
			histBundle &hbn = cleanList_bundle.back();
			histBundle hbnn;
			projFlavor(hbnn,(TH2*)(m2mcn->at(i,j)),0);
			flipBundle(hbn, hbnn);
			cleanHistBundle(hbnn);
			h=hbn.hudsg;
		}
	}
	h->Draw();
}

