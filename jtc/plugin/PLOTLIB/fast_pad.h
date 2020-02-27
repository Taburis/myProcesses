
#ifndef FAST_PAD_H
#define FAST_PAD_H

#include "myProcesses/jtc/plugin/PLOTLIB/base_pad.h"

class fast_pad : public base_pad{
	public : fast_pad(TString name):base_pad(name){
			 //SetFillColor(1);
			 //SetMargin(0.15, 0.01, 0.15, 0.03);
			 //gPad->DrawFrame(0,0,1,1);
		 }
		 ~fast_pad(){}
		// virtual void add(TH1* h) override {
		//	 if(xmin> xmax) {
		//		 xmin = h->GetXaxis()->GetXmin();
		//		 xmax = h->GetXaxis()->GetXmax();
		//	 }
		//	 hist.push_back(h);
		// }
		 virtual void draw(TString opt = "") override;
		 void init_style(){
		 }
//		 void setXrange(float x1, float x2) {xmin = x1; xmax = x2;}
		 void setup_frame(TH1*h) override {
			 gStyle->SetOptStat(0);
			 h->SetAxisRange(xmin, xmax, "X");
			 if(doAutoYrange) autoYrange(xmin, xmax);
			 else h->SetAxisRange(ymin, ymax, "Y");
			 h->GetYaxis()->SetLabelSize(0.05);
			 h->GetXaxis()->SetLabelSize(0.05);
//			 h->GetXaxis()->SetTitle(xtitle);
			 h->GetXaxis()->SetTitleSize(0.05);
			 h->GetYaxis()->SetTitleSize(0.05);
			 h->GetXaxis()->SetTitleOffset(1.12);
			 h->GetXaxis()->CenterTitle();
			 setTitlePosition();
		 };
		 void setTitlePosition(){
			 gStyle->SetTitleX(0.6);
			 gStyle->SetTitleY(0.97);
		 }


		 Color_t *color = default_plot_setup::color;
		 float upMargin = 0.08, downMargin = 0.08;
		 bool doLogy = 0;
		 TLegend l;
		 //style variables
		 int marker = 24; float marksize = 0.7;
};

void fast_pad::draw(TString opt){
	int i=0;
	if(hframe !=nullptr) setup_frame(hframe);
	gPad->SetMargin(0.17,0.005, 0.13, 0.1);
	for(auto &it : hist){
		pad = (TPad*) gPad;
		style0(it, default_plot_setup::color[i]);
		it->Draw(opt+"same");
		gPad->SetLogy(doLogy);
		i++;}
	if(doline){
		cout<<"draw line:"<<xmin<<", "<<xmax<<endl;
		line.SetLineStyle(2); line.DrawLine(xmin, yline, xmax, yline);
	}
	if(doLegend) legend->Draw();
}

#endif
