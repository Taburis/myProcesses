
#ifndef BASE_PAD_H
#define BASE_PAD_H

#include "TLine.h"
#include "TStyle.h"

namespace default_plot_setup{
	Color_t color [] ={kBlue+1, kRed+1, kGreen+2, kAzure+7, kMagenta+2, kBlack};
}

class base_pad {
	public : base_pad(){ }
		 base_pad(TString name): pname(name){ 
			 //cd();
			 pad= (TPad*)gPad;
		 }
		 virtual void init(){
			 hframe = gPad->DrawFrame(0,0, 1,1);
			 hframe->SetName(pname+"_frame");
		 }
		 ~base_pad(){}
		 void autoYrange(float x1, float x2);
		 void autoYrange();
		TString getName(){return pname;}
		 virtual void addTH1(TH1* h){
			 if(hframe == nullptr) hframe = h;
			 hist.push_back(h);
			 if(xmin< xmax) return;
			 xmin = h->GetXaxis()->GetXmin();
			 xmax = h->GetXaxis()->GetXmax();
		 }
		 virtual void setup_frame(TH1* h){
			 h->SetAxisRange(xmin, xmax, "X");
			 if(doAutoYrange) autoYrange(xmin, xmax);
//			 else if(ymax > ymin) h->SetAxisRange(ymin, ymax, "Y");
			 h->GetYaxis()->SetLabelSize(0.06);
			 h->GetXaxis()->SetLabelSize(0.06);
			 h->GetXaxis()->SetTitle(xtitle);
			 h->GetXaxis()->SetTitleSize(0.07);
			 h->GetXaxis()->SetTitleOffset(0.8);
			 h->GetXaxis()->CenterTitle();
			 //						 hframe->SetAxisRange(ymin, ymax, "Y");
		 }
		 virtual void draw(TString opt){
			 //						 cout<<"base_pad::draw()"<<endl;
			((TPad*)gPad)->SetTickx(1);
			((TPad*)gPad)->SetTicky(1);
			 int i=0;
			 gPad->SetBottomMargin(0.12);
			 gPad->SetTopMargin(0.06);
			 gPad->SetLeftMargin(0.18);
			 gPad->SetRightMargin(0.04);
			 if(hframe !=nullptr) setup_frame(hframe);
			 for(auto &it : hist){
				 pad = (TPad*) gPad;
				 style0(it, default_plot_setup::color[i]);
				 it->Draw(opt+"same");
				 gPad->SetLogy(doLogy);
				 i++;}
			 if(doLegend) legend->Draw();
		 }
		 virtual void style0(TH1* h, Color_t color){
			 h->SetMarkerStyle(marker);
			 h->SetMarkerSize(markerSize);
			 h->SetMarkerColor(color);
			 h->SetLineColor(color);
		 }
		 virtual void copy_config(base_pad &p){
			 if(p.xmin < p.xmax){xmin = p.xmin; xmax = p.xmax;}
			 doLogy = p.doLogy;
			 marker = p.marker; markerSize = p.markerSize;
			 doAutoYrange = p.doAutoYrange;
			 upMargin=p.upMargin; downMargin = p.downMargin;
			 xtitle = p.xtitle;
			 ytitle = p.ytitle;
			 doline = p.doline; yline = p.yline;

			 ymin = p.ymin; ymax=p.ymax;
		 }
		 void drawText(float x, float y, TString txt){
			 if(pad==nullptr) std::cout<<"please draw canvas before adding labels"<<std::endl;
			 pad->cd();
			 text.SetTextSize(0.07);
			 text.DrawLatexNDC(x, y, txt);
		 }
		 virtual void setXrange(float x1, float x2) {xmin = x1; xmax = x2;}
		 virtual void setYrange(float x1, float x2) {ymin = x1; ymax = x2;doAutoYrange = 0;}
		 virtual void drawHLine(float y, int opt = 0){
			 // draw a horizontal line
			 int xxmin = hframe->GetXaxis()->GetXmin();
			 int xxmax = hframe->GetXaxis()->GetXmax();
			 line.DrawLine(xxmin, y, xxmax, y);
		 }

		 float xmin = 0, xmax = -1;
		 float ymin = 0, ymax = -1;
		 bool doLogy = 0, doLegend = 0, doAutoYrange =0;
		 float upMargin = 0.1, downMargin = 0.1;
		 int marker = 20; float markerSize = 0.8;
		 TH1* hframe = nullptr;
		 TString pname;
		 std::vector<TH1*>hist;
		 TString xtitle="", ytitle="";
		 TPad* pad=nullptr; TLegend* legend=nullptr;
		 TLatex text; TLine line;
		 bool doline = 0; float yline;
};

void base_pad::autoYrange(float x1, float x2){
	if(hist.size()==0) return;
	float max, min;
	max = get_max_in_range(hist.at(0), x1, x2);
	min = get_min_in_range(hist.at(0), x1, x2);
	for(int i=1; i< int(hist.size()); ++i){	
		float h = get_max_in_range(hist.at(i), x1, x2);
		float l = get_min_in_range(hist.at(i), x1, x2);
		if(max < h) max =h;
		if(min > l) min =l;
	}
	float marginUp = max+(max-min)*upMargin;
	float marginDown = min-(max-min)*downMargin;
	hframe->SetAxisRange(marginDown, marginUp, "Y");
	hframe->SetAxisRange(x1, x2, "X");
}

void base_pad::autoYrange(){
	auto h = hist.at(0);	
	float xmax = h->GetXaxis()->GetXmax();
	float xmin = h->GetXaxis()->GetXmin();
	autoYrange(xmin, xmax);
}

#endif

