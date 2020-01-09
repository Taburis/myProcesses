
#ifndef FAST_PAD_H
#define FAST_PAD_H

#include "myProcesses/jtc/plugin/PLOTLIB/base_pad.h"

class fast_pad : public base_pad{
		public : fast_pad(TString name):base_pad(name){
//						 SetFillColor(1);
//						 SetMargin(0.15, 0.01, 0.15, 0.03);
						 gPad->DrawFrame(0,0,1,1);
				 }
				 ~fast_pad(){}
				 void add(TH1* h){
						 if(xmin> xmax) {
								 xmin = h->GetXaxis()->GetXmin();
								 xmax = h->GetXaxis()->GetXmax();
						 }
						 hist.push_back(h);}
				 virtual void draw(TString opt = "");
				 void init_style(){
				 }
				 void setXrange(float x1, float x2) {xmin = x1; xmax = x2;}

				 std::vector<TH1*> hist;
				 float xmin = 0, xmax = -1;
				 Color_t *color = default_plot_setup::color;
				 float upMargin = 0.08, downMargin = 0.08;
				 bool doLogy = 0;
				 TLegend l;
				 //style variables
				 int marker = 24; float marksize = 0.7;
};

void fast_pad::draw(TString opt){
		int i=0;
		if(!doLogy) autoYrange(xmin, xmax);
		for(auto &it : hist){
				//style0(it, color[i]);
				pad->cd();
				it->Draw(opt+"same");
				gPad->SetLogy(doLogy);
				i++;
		}
}

#endif
