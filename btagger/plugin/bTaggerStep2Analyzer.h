
#include "utility/simpleReader.h"
#include "bTaggerAnalyzer.h"
#include <cstdlib>

class bTaggerStep2Analyzer{
		struct histBundle {TString name; TH1 *hudsg=nullptr, *hc=nullptr, *hb=nullptr, *hdata=nullptr; bool kData=0, kMC=0;};
		public : bTaggerStep2Analyzer(TString n): name(n){}
				 ~bTaggerStep2Analyzer(){}
				 void loadMC(TFile *f){srmc.load(f);}
				 void loadData(TFile *f){srdata.load(f);}

				 void projFlavor(histBundle &hb, TH2* h, bool isData){
						 hb.name = h->GetName();
						 if(isData){
								 hb.hdata = (TH1*)h->ProjectionX(hb.name+"_data", bTaggerAnalyzer::flavorID::unknown+1, bTaggerAnalyzer::flavorID::unknown+1);
						 }else {
								 hb.hudsg = (TH1*) h->ProjectionX(hb.name+"_udsg", bTaggerAnalyzer::flavorID::udsg+1, bTaggerAnalyzer::flavorID::udsg+1);
								 hb.hc = (TH1*) h->ProjectionX(hb.name+"_c", bTaggerAnalyzer::flavorID::c+1, bTaggerAnalyzer::flavorID::c+1);
								 hb.hb = (TH1*) h->ProjectionX(hb.name+"_b", bTaggerAnalyzer::flavorID::b+1, bTaggerAnalyzer::flavorID::b+1);
						 }
						 return hb;
				 }

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
				 multi_pads *drawQA(TString name, TString hname, TString xtitle, float x, float y, int rebin= 1, bool logy=0){
						 ncent = cent->nbins;
						 auto c = new multi_pads(name, "", 1, ncent);
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
						 TString folder = "./"+name+"_QAs/";
						 const int dir_err = system("mkdir -p "+folder);
						 for(int i=0; i<11; i++){
								 TString cname = h2List[i];
								 cname.ReplaceAll("_C*", "_stack");
								 std::cout<<"processing "<<cname<<std::endl;
								 auto c = drawQA(cname, h2List[i], h2xtitle[i], xmin[i], xmax[i], rebin[i], logy[i]);
								 c->SaveAs(folder+cname+".jpg");
						 }
				 }
				 void produceList();


				 TString name;
				 int ncent;
				 centralityHelper *cent;
				 simpleReader srmc, srdata;
				 TString h2List[11] = {"jtpt_C*", "jteta_C*", "jtphi_C*", 
						 "wTagger_C*", "pTagger_C*", "nTagger_C*", 
						 "hnsvtx_C*", "hsvtxm_C*", "hsvtxdl_C*", "hsvtxdls_C*", "hsvtxntrk_C*"};
				 TString h2xtitle[11] = {"p_{T}^{jet}", "#eta_{T}^{jet}", "#phi_{T}^{jet}", 
						 "csv Value", "positive csv Value", "negative csv Value",
						 "# of SV", "SV mass", "SV distance", "SV distance significance", "Trk # assoicated to SV"};
				 float xmin[11] ={0, -2,  -3.2, -0.05, -0.05, -0.05, 0,   0,   0,   0, 0};
				 float xmax[11] ={1, 1.99,3.19,  1.05,  1.05,  1.05, 5.9, 5.9, 4.9, 1, 9.9};
				 int  rebin[11] ={1, 1, 2, 2, 2,1,1,1,1,1,1 };
				 bool logy [11] ={1, 0, 0, 1, 1, 1, 1,1, 1, 1,1};
};

void bTaggerStep2Analyzer::produceList(){
		TString folder = "./"+name+"_QAs/";
		multi_pads *mp;
		mp= drawQA("jteta_stack", "jteta_C*", "#eta^{jet}", -2, 1.99);
		
		mp= drawQA("jtphi_stack", "jtphi_C*", "#phi^{jet}", -3.2, 3.19);
		mp= drawQA("wTagger_stack", "wTagger_C*", "csv Value", -0.05, 1.1, 2);
		mp= drawQA("pTagger_stack", "pTagger_C*", "positive csv Value", -0.05, 1.1, 2);
		mp= drawQA("nTagger_stack", "nTagger_C*", "negative csv Value", -0.05, 1.1, 2);
};
