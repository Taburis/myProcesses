
#ifndef STACK_PAD_H
#define STACK_PAD_H
#include "myProcesses/jtc/plugin/PLOTLIB/overlay_pad.h"
#include "myProcesses/jtc/plugin/PLOTLIB/stackPlot.h"

class stack_pad : public overlay_pad{
	public: stack_pad(){}
		stack_pad(TString name, float r = .32): overlay_pad(name, r) {
		}
		//virtual void add(TH1* h) override {
		//}
		void projection(TH2* h, Option_t* axis="x", Int_t fbin1=1, Int_t lbin1= -1) {
			sp = new stackPlot(TString(Form("%s_stack",h->GetName())),(TH1*)h,axis, fbin1,lbin1);
			sp->addLegend();
			sp->defaultColor();
		}
		void addReference(TH1* h, TString label){
			auto h1=  (TH1*) h->Clone(this->getName()+"_ref");
			sp->legend->AddEntry(h1, label, "lp");
			href.emplace_back(h1);
			h1->SetMarkerStyle(20);
			h1->SetMarkerSize(0.8);
		}
		void stackRatio(TH1* h){
			auto h0 = (TH1*) sp->hsum->Clone(Form("%s_ratio",h->GetName()));
			h0->Divide(h);
			hratio.emplace_back(h0);
		}
		virtual void draw(TString opt) override {
			gStyle->SetOptStat(0);
			gStyle->SetOptTitle(0);
			uppad->cd();
			((TPad*)gPad)->SetTickx(1);
			((TPad*)gPad)->SetTicky(1);
			sp->Draw("hist");
			if(doNorm){
				sp->normalizeStack();
				for(auto &it : href)
					sp->normalizeReference(it);
			}
			hframe = sp->GetHistogram();
			uppad_style(hframe);
			hframe->Draw();
			sp->SetMaximum(ymax);
			sp->SetMinimum(ymin);
			sp->Draw("hist");

			for(auto &it : href){
				stackRatio(it);
				 gPad->SetLogy(doLogy);
				it->Draw("same pfc");
			}

			downpad->cd();
			((TPad*)gPad)->SetTickx(1);
			((TPad*)gPad)->SetTicky(1);
			line.SetLineStyle(2);
			downpad->cd(); int i=0;
			downpad_style(hratio.at(0));
			for(auto &it : hratio){
				style0(it, kBlack);
				//style0(it, default_plot_setup::color[i]);
				it->Draw(opt+"same");
				drawHLine(1);
				i++;}
			if(doLegend) drawLegend();
		}
		void drawLegend(){
			uppad->cd();
			sp->legend->Draw();
		}
		stackPlot *sp;
		std::vector<TH1*> href;
		bool doNorm= false;
};

#endif
