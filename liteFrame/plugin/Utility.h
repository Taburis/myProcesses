
#ifndef LITEFRAME_UTILITY_H
#define LITEFRAME_UTILITY_H
#include "TLatex.h"
#include "myProcesses/jtc/plugin/xAxis.h"

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

void reset_constant_bin(TH1* h, float c){
	// func: set all bin contents of h to a constant c, including overflow or underflow bin
	for(int i=0;i< h->GetNbinsX()+2; i++){
		h->SetBinContent(i,c);
	}
}

float get_max_in_range(TH1*h, float xmin, float xmax){
	if(xmax <xmin ){
		std::cout<<"Error: get_max_in_range:"<<"boundary problem ["<<xmin<<","<<xmax<<"]"<<std::endl;
		return 0;
	}
	int n0 = h->GetXaxis()->FindBin(xmin);
	int n1 = h->GetXaxis()->FindBin(xmax);
	float x=0;
	bool init= 1;
	for(int i=n0 ;i<n1+1; ++i){
		float y =h->GetBinContent(i);
		float e =h->GetBinError(i);
		//if(e==0) continue;// skip empty bin
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
	float x=0;
	bool init = 1;
	for(int i=n0 ;i<n1+1; ++i){
		float y =h->GetBinContent(i);
		float e =h->GetBinError(i);
		//if( e==0) continue;// skip empty bin
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
			float wy = h->GetYaxis()->GetBinWidth(j);
			float cont= h->GetBinContent(i,j);
			float err = h->GetBinError(i,j);
			h->SetBinContent(i,j , cont/wx/wy);
			h->SetBinError(i, j, err/wx/wy);
		}
	}
}

//void regularization(TH1* h){
//	float xmin = h->GetXaxis()->GetXmin();
//	float xmax = h->GetXaxis()->GetXmax();
//	divide_bin_size(h);
//	h->SetAxisRange(xmin, xmax-10, "X");
//}

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

float th1_ave_content(TH1* h, float x1, float x2){
	int n1 = h->FindBin(x1);
	int n2 = h->FindBin(x2);
	float ave = 0;
	for(auto k=n1; k<n2+1; k++){
		ave+=h->GetBinContent(k);	
	}
	return ave/(n2-n1+1);
}
float th1_ave_error(TH1* h, float x1, float x2){
	int n1 = h->FindBin(x1);
	int n2 = h->FindBin(x2);
	float ave = 0;
	for(auto k=n1; k<n2+1; k++){
		ave+=h->GetBinError(k);	
	}
	return ave/(n2-n1+1);

}

void set_errorbased_plot_range(TH1*h, float xmin, float xmax, float scale = 3.0){
	int n0 = h->GetXaxis()->FindBin(xmin);
	int n1 = h->GetXaxis()->FindBin(xmax);
	float vmax=h->GetBinContent(n0);
	float vmin=h->GetBinContent(n0);
	float error = h->GetBinError(n0);
	for(int i=n0+1 ;i<n1+1; ++i){
		float y =h->GetBinContent(i);
		float e =h->GetBinError(i);
		if( e==0) continue;// skip empty bin
		error+=e;
		if(vmax< y) vmax = y;
		if(vmin> y) vmin = y;
	}
	error = error/(n1-n0+1);
	h->SetAxisRange(vmin-scale*error, vmax+scale*error, "Y");
}

void autoYrange(float x1, float x2, TH1* h, bool doLogy = 0){
	float upMargin = 0.15, downMargin = 0.1;
	float max, min;
	max = get_max_in_range(h, x1, x2);
	min = get_min_in_range(h, x1, x2);
	float marginUp = doLogy ? max*10: max+(max-min)*upMargin;
	float marginDown = doLogy ? min*0.01 : min-(max-min)*downMargin;
	h->SetAxisRange(marginDown, marginUp, "Y");
	h->SetAxisRange(x1, x2, "X");
}

#endif
