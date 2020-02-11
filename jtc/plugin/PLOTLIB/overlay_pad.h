
#ifndef OVERLAY_PAD_H
#define OVERLAY_PAD_H
#include "base_pad.h"

class overlay_pad : public base_pad{
	public : overlay_pad(){}
		 overlay_pad(TString name,float r = .32){
			 uppad  = new TPad(name+"_up", "", 0.0, r, 1, 0.98);
			 downpad= new TPad(name+"_down", "", 0.0, 0.0, 1, r);
			 uppad  ->SetTopMargin(0.08);
			 uppad  ->SetLeftMargin(0.18);
			 uppad  ->SetRightMargin(0.03);
			 uppad  ->SetTopMargin(0.03);
			 uppad  ->SetBottomMargin(0);
			 downpad->SetLeftMargin(0.18);
			 downpad->SetTopMargin(0);
			 downpad->SetRightMargin(0.03);
			 downpad->SetBottomMargin(0.4);
			 uppad  ->Draw();
			 downpad->Draw();
			 //uppad->cd();
			 //gPad->DrawFrame(0,0,1,1);
		 }

		 TPad *uppad, *downpad;
		 std::vector<TH1*> hratio;
		 bool getRatio(){
			 //hframe which is the first histogram will be the denominator for all rest histograms.
			 if(hist.size() <2) return 0;
			 for(int i=1; i<int(hist.size()); ++i){
				 TH1* h = (TH1*)hist.at(i)->Clone(Form("%s_ratio",hist.at(i)->GetName()));
				 h->Divide(hframe);
				 hratio.emplace_back(h);
			 }
			 return 1;
		 }
		 void uppad_style(TH1* h){
			h->GetYaxis()->SetTitle(ytitle);
			h->GetYaxis()->SetTitleSize(0.05);
			h->GetYaxis()->SetTitleOffset(1.4);
			h->GetYaxis()->SetLabelSize(0.07);
		 }
		 void downpad_style(TH1* h){
			 h->GetYaxis()->SetLabelSize(0.14);
			 h->GetYaxis()->SetNdivisions(505);
			 h->GetXaxis()->SetLabelSize(.18);
			 h->GetXaxis()->SetTitleSize(.2);
			 h->GetXaxis()->SetTitleOffset(.82);
			 h->GetXaxis()->CenterTitle();
			 h->GetXaxis()->SetTitle(xtitle);
			 h->SetAxisRange(xmin, xmax, "X");
			 h->SetAxisRange(rymin, rymax, "Y");
		 }
		 virtual void draw(TString opt){
			 if(hframe !=nullptr) setup_frame(hframe);
			 int i=0;
			 uppad->cd();
			 pad = uppad;
			 for(auto &it : hist){
				 //cout<<"drawing"<<endl;
				 style0(it, default_plot_setup::color[i]);
				 it->Draw(opt+"same");
				 gPad->SetLogy(doLogy);
				 i++;}
			 if(doLegend) legend->Draw();
			 if(!getRatio()) return; 
			 l.SetLineStyle(2);
			 downpad->cd(); i=0;
			 downpad_style(hratio.at(0));
			 for(auto &it : hratio){
				 style0(it, kBlack);
				 //style0(it, default_plot_setup::color[i]);
				 it->Draw(opt+"same");
				 l.DrawLine(xmin, 1, xmax, 1);
				 i++;}
		 }
		 TLine l; 
		 // the ymin and ymax for the ratio pad
		 float rymin = 0.8, rymax = 1.2;
};

#endif
