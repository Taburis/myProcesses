
#ifndef JTC_PLOTLIB_H
#define JTC_PLOTLIB_H

#include "myProcesses/jtc/plugin/PLOTLIB/multi_pads.h"
#include "myProcesses/jtc/plugin/PLOTLIB/base_pad.h"
#include "myProcesses/jtc/plugin/PLOTLIB/overlay_pad.h"
#include "myProcesses/jtc/plugin/PLOTLIB/stack_pad.h"

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

void style_101(TH1* h, Color_t c, float size){
	h->SetMarkerColor(c);
	h->SetLineColor(c);
	h->SetMarkerStyle(20);
	h->SetMarkerSize(size);
}

/*
   class plotManager {
   public : plotManager(){}
   ~plotManager(){}
   multi_pads* bookCanvas(Tstring name, int n, int m, float w, float h){
   gc = multi_pads(name,"", n,m,w,h);
   return gc;
   }

   multi_pads *gc=nullptr;
   };
   */
#endif 
