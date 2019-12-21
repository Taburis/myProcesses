
#ifndef PLOT_CHECK_SIDEBAND_H
#define PLOT_CHECK_SIDEBAND_H
#include "myProcesses/jtc/plugin/PLOTLIB/multi_pads.h"

class jtcSignalProducer;

class plot_check_sideband: public multi_pads<base_pad>{
		public : plot_check_sideband(TString name, int n, int m): multi_pads<base_pad>(name, "", n,m){}
				 virtual ~plot_check_sideband(){}
				 void loadSignal(jtcSignalProducer *js0){
						 js = js0;
						 // ideally should input the sig_p2
				 }
				 void check(){
						 addm2TH1(js->deta_sb_p2);
						 addm2TH1(js->deta_sig_p2);
						 base_pad::doAutoYrange=0;
						 xmin = -3., xmax= 2.99;
						 draw();
						 xtitle = "#Delta#eta";
						 for(int i=0; i<nrow; ++i){
								 for(int j=0; j<ncol; ++j){
											fpads.at(i,j)->line.SetLineStyle(2);				 
											fpads.at(i,j)->drawHLine(0);				 
								 }}
				 }
				 void slice_sideband(float start, float inter, int n=3){
						 for(int i=0 ; i<n; ++i){
								 auto jsb= js->jsig_p2->projX(js->name+Form("_sbslice%d",i),start+i*inter, start+(i+1)*inter,"e",0);
								 jsb->rebinX(10);
								 addm2TH1(jsb);
						 }
						 xtitle = "#Delta#eta";
						 xmin = -3., xmax= 2.99;
						 upMargin = 0.4; downMargin=0.4;
						 draw();
				 }
				 jtcSignalProducer * js;
				 float sidemin = 1.5, sidemax = 2.;
};

#endif
