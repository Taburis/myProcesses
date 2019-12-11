
#include "TCanvas.h"
//#include "jtcAnalysisTools.h"
#include "myProcesses/jtc/plugin/matrixTH1Ptr.h"
#include "myProcesses/jtc/plugin/Utility.h"
#include "myProcesses/jtc/plugin/stackPlot.h"

namespace default_plot_setup{
	Color_t color [] ={kBlue+1, kRed+1, kGreen+2, kAzure+7, kMagenta+2, kBlack};
}

class fast_pad : public TPad{
	public : fast_pad(){}
		 ~fast_pad(){}
		 void add(TH1* h){
			 if(xmin> xmax) {
				 xmin = h->GetXaxis()->GetXmin();
				 xmax = h->GetXaxis()->GetXmax();
			 }
			 hist.push_back(h);}
		 void autoYrange(float x1, float x2);
		 void autoYrange();
		 void draw(TString opt = "");
		 void init_style(){
			 this->SetFillColor(0);
		 }
		 void setXrange(float x1, float x2) {xmin = x1; xmax = x2;}
		 void style0(TH1*h, Color_t color){
			 // used by overlay and overlayR(upper panel);
			 h->SetLineColor(color);
			 h->SetMarkerStyle(20);
			 h->SetMarkerSize(0.8);
			 h->SetMarkerColor(color);
			 //						 h->GetYaxis()->SetTitle(Ytitle);
		 }

		 std::vector<TH1*> hist;
		 float xmin = 0, xmax = -1;
		 Color_t *color = default_plot_setup::color;
		 float upMargin = 0.08, downMargin = 0.08;
		bool doLogy = 0;
};

void fast_pad::autoYrange(float x1, float x2){
	if(hist.size()==0) return;
	float max, min;
	max = get_max_in_range(hist.at(0), x1, x2);
	min = get_min_in_range(hist.at(0), x1, x2);
	for(int i=1; i< hist.size(); ++i){	
		float h = get_max_in_range(hist.at(i), x1, x2);
		float l = get_min_in_range(hist.at(i), x1, x2);
		if(max < h) max =h;
		if(min > l) min =l;
	}
	float marginUp = max+(max-min)*upMargin;
	float marginDown = min-(max-min)*downMargin;
	for(auto &it : hist){
		it->SetAxisRange(marginDown, marginUp, "Y");
		it->SetAxisRange(x1, x2, "X");
	}
}

void fast_pad::autoYrange(){
	auto h = hist.at(0);	
	float xmax = h->GetXaxis()->GetXmax();
	float xmin = h->GetXaxis()->GetXmin();
	autoYrange(xmin, xmax);
}

void fast_pad::draw(TString opt){
	int i=0;
	if(!doLogy) autoYrange(xmin, xmax);
	for(auto &it : hist){
		style0(it, color[i]);
		this->cd();
		it->Draw(opt+"same");
		gPad->SetLogy(doLogy);
		i++;
	}
}

class multi_pads : public TCanvas{
	public : multi_pads(TString name, TString title, int n, int m): TCanvas(name, title,int(m*350),int(n*325)), ncol(m), nrow(n){
			 Divide(ncol, nrow);
			 fpads.setup(nrow, ncol);	 
			 for(int i=0; i< nrow; ++i){
				 for(int j=0; j< ncol; ++j){
					 CD(i,j);
					 auto pad = new fast_pad();
					 pad->init_style();
					 pad->SetMargin(0.15, 0.01, 0.15, 0.03);
					 pad->Draw();
					 fpads.add(pad,i,j);
				 }
			 }
		 }
		 void (*plot_style_init)(TH1* ) = nullptr;
		 virtual ~multi_pads(){}
		 int flatten(int n, int m){
			 if(n > nrow -1 || m > ncol -1 ) {
				 std::cout<<" ==== ERROR: index ("<<n<<", "<<m<<") exceeds the range!!! current shape ["<<nrow<<", "<<ncol<<"] ===="<<std::endl;
				 return 0;
			 }
			 //slightly different from the matrixPtrHolder, the row and column in canvas start from 1 and up, 0 stands for the whole pad.
			 return n*ncol+m+1;
		 }
		 void CD(int n, int m){ this->cd(flatten(n,m));}
		 void add(TH1* h, int n, int m){ fpads.at(n,m)->add(h);}
		 fast_pad* at(int i,int j){return fpads.at(i,j);}
		 void draw(TString opt = "" ){
			 for(int i=0; i< nrow; ++i){
				 for(int j=0; j< ncol; ++j){
					 //cout<<i<<":"<<j<<endl;
					 fpads.at(i,j)->draw(opt);
				 }
			 }
		 }

		 int Ncol(){return ncol;}
		 int Nrow(){return nrow;}
		 int ncol, nrow;
		 matrixPtrHolder<fast_pad> fpads;
};

class plot_jet_spectra : public multi_pads {
	public : plot_jet_spectra(TString name, int ncent): multi_pads(name, "", 3, ncent){} 
		 virtual ~plot_jet_spectra(){}
		 void load(TString name_prefix, TFile *f, int i){
			 auto h = (TH1*) f->Get(name_prefix+Form("_corrpt_%d", i));
			 h->Scale(1.0/h->Integral());
			 add(h, 0, i);
			 at(0,i)->setXrange(0, 400);
			 h = (TH1*) f->Get(name_prefix+Form("_eta_%d", i));
			 h->Scale(1.0/h->Integral());
			 add(h, 1, i);
			 at(1,i)->setXrange(-2, 2);
			 h = (TH1*) f->Get(name_prefix+Form("_phi_%d", i));
			 h->Scale(1.0/h->Integral());
			 add(h, 2, i); at(2,i)->upMargin = 2; at(2,i)->downMargin = 2; 
			 at(2,i)->setXrange(-3.2, 3.2);
		 }
		 void autoLoad(TString name_prefix, TFile *f){
			 for(int i=0; i<ncol; ++i){
				 load(name_prefix, f, i);
			 }
		 }

};

/*
   class plotHelperBase {
   public : plotHelperBase(){}
   ~plotHelperBase(){}
   multi_pads* makeCanvas(TString name, TString title, int n, int m){
   fpads.setup(n,m);
   for(int i=0; i<n; i++){
   for(int j=0; j<n; j++){
   }
   }
   auto mp = new multi_pads(name, title, n,m); return mp;
   };
   void add(TH1* h, int i, int j){fpads.at(i,j)->add(h);}

   };
   */
