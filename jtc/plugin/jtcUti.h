

#ifndef JTCUTI_H
#define JTCUTI_H

#include <fstream>
#include <algorithm>
#include <iostream>
#include <vector>
#include <stdlib.h>
#include "TH1.h"
#include "TH2.h"
#include "myProcesses/jtc/plugin/Utility.h"
#include "myProcesses/jtc/plugin/PLOTLIB/multi_pads.h"

namespace default_setup{
	int nptbin = 20;
	Double_t ptbin[21] = {110, 120, 136, 152, 168, 184, 200, 216, 232, 248, 264, 280, 296, 312, 328, 344, 360, 380, 400, 432, 500};
	Double_t etabin[24] ={-3.5, -3, -2.5,-2.,-1.5, -1., -0.8, -0.6, -0.4, -0.3, -0.2, -0.1, 0.1, 0.2, 0.3, 0.4, 0.6, 0.8, 1., 1.5,2.,2.5, 3, 3.5};
	Double_t phibin[18] ={-1.50796, -1.00531,-0.879646, -.75398, -0.628319,-0.502655, -0.376991, -0.251327, -0.125664, 0.125664, 0.251327, 0.376991, 0.502655, 0.628319,.75398, 0.879646, 1.00531,1.50796};

}

struct histCase{
	//if want to add any hist, need to add it in the quickHistReg as well, and add the filling in the fillCase
	TH2D** sig;
	TH2D** sig_pTweighted;
	TH2D** mixing;
};

double hist_weight(float x, TH1* h = 0){
	if(h==0) return 1;
	int n = h->GetXaxis()->FindBin(x);
	return h->GetBinContent(n);	
}

template <typename T>
multi_pads<T>* prepare_canvas(TString name, int n, int m){
	auto c = new multi_pads<T>(name, "", n,m);
	c->addLegend("upperright");
	c->doHIarrange = 1;
	c->setXrange(0, .99);
	return c;
}

TString reco_tag(bool jet, bool trk){
	TString tag = jet ? "_RecoJet" : "_GenJet";
	TString tag2 = trk ? "_RecoTrk":"_GenTrk";
	return tag+tag2;
}

double findDr(double eta1, double phi1, double eta2, double phi2){
	double dphi = phi1 - phi2;
	while (dphi > M_PI) dphi-= 2*M_PI;
	while (dphi <= -M_PI) dphi += 2*M_PI;
	return sqrt(pow(dphi,2)+pow(eta1-eta2,2));
}

void ReadFileList(std::vector<std::string> &my_file_names, TString file_of_names, bool debug)
{
std:fstream file_stream(file_of_names);
    std::string line;
    my_file_names.clear();
    if( debug ) std::cout << "Open file " << file_of_names << " to extract files to run over" << std::endl;
    if( file_stream.is_open() ) {
	    if( debug ) std::cout << "Opened " << file_of_names << " for reading" << std::endl;
	    int line_num = 0;
	    while( file_stream >> line ) {
		    if( debug ) std::cout << line_num << ": " << line << std::endl;
		    std::string tstring_line(line);
		    tstring_line.erase(std::remove(tstring_line.begin(), tstring_line.end(), '"'), tstring_line.end());
		    tstring_line.erase(std::remove(tstring_line.begin(), tstring_line.end(), ','), tstring_line.end());
		    tstring_line.erase(std::remove(tstring_line.begin(), tstring_line.end(), '['), tstring_line.end());
		    tstring_line.erase(std::remove(tstring_line.begin(), tstring_line.end(), ']'), tstring_line.end());
		    if( tstring_line != "" ) my_file_names.push_back(tstring_line);
		    line_num++;
	    }
    } else {
	    std::cout << "Error, could not open " << file_of_names << " for reading" << std::endl;
	    assert(0);
    }
}

/*
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
	for(int i=1; i<hh->GetNbinsX()+1; ++i){
		Double_t wd = hh->GetBinWidth(i);
		if(!hh->GetBinContent(i)) continue; //skip empty bin
		hh->SetBinContent(i, hh->GetBinContent(i)/wd);
		hh->SetBinError  (i, hh->GetBinError(i)/wd);
	}
	delete h;
	return hh;
}
*/



#endif
