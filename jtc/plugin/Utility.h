
#ifndef UTILITY_H
#define UTILITY_H
#include "xAxis.h"
#include "TLatex.h"

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

#endif
