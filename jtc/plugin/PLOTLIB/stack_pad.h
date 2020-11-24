
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

		void autoStackYrange(){
			float max = sp->hsum->GetXaxis()->GetXmax();
			float min = sp->hsum->GetXaxis()->GetXmin();
			ymax = get_max_in_range(sp->hsum, min, max);
			float ymin0 = get_min_in_range(sp->hsum, min, max);
			if(doLogy){
				ymin = 0.5*ymin0;
				ymax = ymax/0.01;
			} else ymax = ymax/0.8;
		}

		virtual void draw(TString opt="") override {
			gStyle->SetOptStat(0);
			gStyle->SetOptTitle(0);
			uppad->cd();
			sp->Draw("hist");
			((TPad*)gPad)->SetTickx(1);
			((TPad*)gPad)->SetTicky(1);
			if(doNorm){
				sp->normalizeStack();
				for(auto &it : href)
					sp->normalizeReference(it);
			}
			hframe = sp->GetHistogram();
			uppad_style(hframe);
			hframe->Draw();
			autoStackYrange();
			sp->SetMaximum(ymax);
			sp->SetMinimum(ymin);
			sp->Draw("hist");
			gPad->SetLogy(doLogy);
			for(auto &it : href){
				stackRatio(it);
				it->Draw("same pfc");
//	cout<<"integral ref: "<<it->Integral()<<endl;
			}

			downpad->cd();
			((TPad*)gPad)->SetTickx(1);
			((TPad*)gPad)->SetTicky(1);
			line.SetLineStyle(2);
			downpad->cd(); 	
			hframe_down = getTH1Frame(sp->hsum, pname+"_downFrame");
			downpad_style(hframe_down);
			hframe_down->Draw();
			for(auto &it : hratio){
				style0(it, kBlack);
				it->Draw(opt+"same");
				drawHLine(1);
				}
			if(doLegend) drawLegend();
		}
		void drawLegend(){
			uppad->cd();
			sp->legend->Draw();
		}
		stackPlot *sp;
		std::vector<TH1*> href;
		bool doNorm= false, doRatio=0;
};

#endif
