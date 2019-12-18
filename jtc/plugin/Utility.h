
#ifndef UTILITY_H
#define UTILITY_H
#include "TLatex.h"
#include "myProcesses/jtc/plugin/xAxis.h"

class centralityHelper{
		public : 
				centralityHelper(int nhibin, float *hibins){
						nbins = nhibin;
						hibin = hibins;
						centax = new xAxis(nbins, hibin);
						makeLabels();
				}
				~centralityHelper(){}
				int jcent(int hibin){
						return centax->findBin(hibin);
				}
				void makeLabels(){
						centLabel = new TString[nbins];
						for(int i=0; i<nbins; ++i){
								centLabel[i]=Form("Cent:%d-%d%%",(int)floor(hibin[i]/2),(int)floor(hibin[i+1]/2));
						}
				}
				void addCentLabel(int i, float x=0.25, float y= 0.85){
						if( centLabel==nullptr) makeLabels();
						tx.DrawLatexNDC(x,y,centLabel[i]);
				}

				float *hibin;
				int nbins;
				xAxis *centax; 
				TString *centLabel = nullptr;
				TLatex tx;
};

void normalize(TH1*h){
		float s = h->Integral();
		h->Scale(1.0/s);
}

void normalize(TH1 *h, float x, float y){
		int n0=h->FindBin(x);
		int n1=h->FindBin(y);
		float s = h->Integral(n0, n1);
		h->Scale(1.0/s);
}

float get_max_in_range(TH1*h, float xmin, float xmax){
		if(xmax <xmin ){
				std::cout<<"Error: get_max_in_range:"<<"boundary problem!"<<std::endl;
				return 0;
		}
		int n0 = h->GetXaxis()->FindBin(xmin);
		int n1 = h->GetXaxis()->FindBin(xmax);
		float x;
		bool init= 1;
		for(int i=n0 ;i<n1+1; ++i){
				float y =h->GetBinContent(i);
				float e =h->GetBinError(i);
				if( e==0) continue;// skip empty bin
				if( init) {init =0; x = y; continue;}
				if(x< y) x = y;
		}
		return x;
}

float get_min_in_range(TH1*h, float xmin, float xmax){
		if(xmax <xmin ){
				std::cout<<"Error: get_min_in_range:"<<"boundary problem!"<<std::endl;
				return 0;
		}
		int n0 = h->GetXaxis()->FindBin(xmin);
		int n1 = h->GetXaxis()->FindBin(xmax);
		float x;
		bool init = 1;
		for(int i=n0 ;i<n1+1; ++i){
				float y =h->GetBinContent(i);
				float e =h->GetBinError(i);
				if( e==0) continue;// skip empty bin
				if( init) {init =0; x = y; continue;}
				if(x> y) x = y;
		}
		return x;
}

void divide_bin_size(TH1* h){
		// at most 2D histogram, this function works fine for TH2 histogram
		int nybins = h->GetNbinsY();
		int nxbins = h->GetNbinsX();
		for(int i=1; i<nxbins+1; ++i){
				for(int j=1; j<nybins+1; ++j){
						float wx = h->GetXaxis()->GetBinWidth(i);
						float wy = h->GetXaxis()->GetBinWidth(j);
						float cont= h->GetBinContent(i,j);
						float err = h->GetBinError(i,j);
						h->SetBinContent(i,j , cont/wx/wy);
						h->SetBinError(i, j, err/wx/wy);
				}
		}
}
TH1* invariantRebin(TH1* h1, TString name , int n, Double_t * bins){
		// rebin the histogram based on the bins given in the parameter
		if(n == h1->GetNbinsX() ) return h1;
		TH1* h=(TH1*) h1->Clone("tmp");
		//input h needs to be invariant
		for(int i=1; i<h->GetNbinsX()+1; ++i){
				Double_t wd = h->GetBinWidth(i);
				h->SetBinContent(i, h->GetBinContent(i)*wd);
				h->SetBinError  (i, h->GetBinError(i)*wd);
		}
		TH1* hh = h->Rebin(n, name, bins);
		divide_bin_size(hh);
		delete h;
		return hh;
}

#endif
