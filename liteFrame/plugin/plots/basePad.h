
#ifndef BASEPAD_H
#define BASEPAD_H

#include "TPad.h"
#include "TColor.h"
#include "TLine.h"
#include "TLatex.h"
#include "TStyle.h"
#include "myProcesses/liteFrame/plugin/Utility.h"

namespace plot_default_setup{
	Color_t color [] ={kBlue+1, kRed+1, kGreen+2, kAzure+7, kMagenta+2, kBlack};
}

class basePad{
	struct histPack{
		TH1* h; TString label; TString labelOpt;
	};
	struct hframe{
		TH1* h; float xmin, xmax, ymin, ymax; int kValid = 0; 
	};
	public :
	basePad(){}
	basePad(TString name){pname = name;
		style=new TStyle(name+"_style", "");
		style->SetLegendBorderSize(0);
		style->SetTitleBorderSize(0);
	}
	~basePad(){}
	void default_style(TH1* h, Color_t color){
		h->SetMarkerStyle(marker);
		h->SetMarkerSize(markerSize);
		h->SetMarkerColor(color);
		h->SetLineColor(color);
	}
	virtual void draw() = 0;
	void basicStyle (TH1* h){
		h->GetYaxis()->SetLabelSize(0.06);
		h->GetYaxis()->SetTitleSize(0.06);
		h->GetXaxis()->SetLabelSize(0.06);
		h->GetXaxis()->SetTitleSize(0.07);
		h->GetXaxis()->SetTitleOffset(0.8);
		h->GetXaxis()->SetNdivisions(505);
		h->GetXaxis()->CenterTitle();
	}
	TH1F* getTH1Frame(TH1* h, TString name){
		Double_t max = h->GetXaxis()->GetXmax();
		Double_t min = h->GetXaxis()->GetXmin();
		//      cout<<"range: "<<min<<" : "<<max<<endl;
		auto h0 = new TH1F(name, "", 1, min, max);
		h0->GetXaxis()->SetTitle(h->GetXaxis()->GetTitle());
		h0->GetYaxis()->SetTitle(h->GetYaxis()->GetTitle());
		return h0;
	}
	void addHist(TH1* h0, TString label = "", TString opt = ""){
		histPack pk;
		pk.h=h0; pk.label=label; pk.labelOpt = opt;
		hists.emplace_back(pk);
	}
	void drawText(float x, float y, TString txt){
		pad->cd();
		latex.SetTextSize(0.07);
		latex.DrawLatexNDC(x,y,txt);
	}
	void drawLegend(TLegend &lg){
		for(auto &it : hists){
			lg.AddEntry(it.h, it.label, it.labelOpt);
		}
		pad->cd();
		lg.Draw();
	}
	TLegend* bookLegend(TString pos = "upperright"){
		TLegend *lg;
		if(pos=="upperright"){
			lg = new TLegend(0.6, 0.7, 0.93, 0.88);
		}
		lg->SetLineColor(0);
		return lg;
	}
	void autoYrange(float x1, float x2, TH1* h, std::vector<histPack> &hist, bool doLogy = 0){
		float upMargin = 0.15, downMargin = 0.1;
		if(hist.size()==0) return;
		float max, min;
		max = get_max_in_range(hist.at(0).h, x1, x2);
		min = get_min_in_range(hist.at(0).h, x1, x2);
		for(int i=1; i< int(hist.size()); ++i){	
			float h = get_max_in_range(hist.at(i).h, x1, x2);
			float l = get_min_in_range(hist.at(i).h, x1, x2);
			if(max < h) max =h;
			if(min > l) min =l;
		}
		float marginUp = doLogy ? max*10: max+(max-min)*upMargin;
		float marginDown = doLogy ? min*0.01 : min-(max-min)*downMargin;
		h->SetAxisRange(marginDown, marginUp, "Y");
		h->SetAxisRange(x1, x2, "X");
	}


	std::vector<histPack> hists;
	float xmin=1, xmax=0;
	TPad* pad;
	TLatex latex;
	TString pname;
	bool doLogy=0;
	TLine line;
	TString xtitle="", ytitle="";

	int marker = 20;
	float  markerSize = 0.8;
	TStyle *style;
};

class squarePad : public basePad {
	public:
		squarePad (){};
		squarePad(TString name): basePad(name){}
		~squarePad (){};

		void frame_style(TH1* h){
			gStyle->SetTitleX(0.4);
			gStyle->SetTitleY(0.95);
			h->GetYaxis()->SetLabelSize(0.06);
			h->GetYaxis()->SetTitleSize(0.06);
			h->GetXaxis()->SetLabelSize(0.06);
			h->GetXaxis()->SetTitleSize(0.07);
			h->GetXaxis()->SetTitleOffset(0.8);
			h->GetXaxis()->SetNdivisions(505);
			h->GetXaxis()->CenterTitle();
		}

		void draw() override{
			gStyle->SetOptStat(0);
			((TPad*)gPad)->SetTickx(1);
			((TPad*)gPad)->SetTicky(1);
			//margin: left, right, bottom, top
			gPad->SetMargin(0.19, 0.05, 0.17, 0.03);
			pad = (TPad*)gPad;
			int i=0; bool kframe = 1;
			for(auto &it : hists){
				default_style(it.h, plot_default_setup::color[i]);
				if(kframe){
					hframe = it.h;
					frame_style(hframe);
					kframe = 0;
				}
				it.h->Draw("same"); i++;
			}
		}

		TH1* hframe;
		float width=350, height=350;
};

class overlayPad : public basePad{
	public :
		overlayPad(){};
		overlayPad(TString name): basePad(name){
			float r = 0.32;
			pname = name;
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
		}

		~overlayPad(){};

		virtual void uppad_style(TH1* h){
			style->SetTitleX(0.45);
			style->SetTitleY(0.95);
			//h->GetYaxis()->SetTitle(ytitle);
			h->GetYaxis()->SetTitleSize(0.07);
			h->GetYaxis()->SetTitleOffset(1.2);
			h->GetYaxis()->SetLabelSize(0.07);
			h->GetXaxis()->SetNdivisions(505);
			//cout<<"-------------- "<<this->xmin<<" : "<<this->xmax<<endl;
			if(this->xmin > this->xmax ) {
				xmin = h->GetXaxis()->GetXmin();
				xmax = h->GetXaxis()->GetXmax();
			} else h->SetAxisRange(xmin, xmax, "X");
			style->cd();
			autoYrange(xmin, xmax, h, hists);
		}

		virtual void downpad_style(TH1* h){
			h->SetTitle("");
			h->GetYaxis()->SetLabelSize(0.14);
			h->GetYaxis()->SetNdivisions(505);
			h->GetXaxis()->SetLabelSize(.15);
			h->GetXaxis()->SetTitleSize(.18);
			h->GetXaxis()->SetNdivisions(505);
			h->GetXaxis()->SetTitleOffset(.92);
			h->GetXaxis()->CenterTitle();
			//			h->GetXaxis()->SetTitle(xtitle);
			//h->GetYaxis()->SetTitle(ratio_title);
			h->GetYaxis()->CenterTitle();
			h->GetYaxis()->SetTitleSize(0.15);
			h->GetYaxis()->SetTitleOffset(0.5);

			h->SetAxisRange(rymin, rymax, "Y");
			if(xmin > xmax ) {
				xmin = h->GetXaxis()->GetXmin();
				xmax = h->GetXaxis()->GetXmax();
			} else{
				h->SetAxisRange(xmin, xmax, "X");
			}
		}
		void getRatio(){
			if(hists.size() <2) return 0;
			hden = hists[0].h;
			for(int i=1; i<int(hists.size()); ++i){
				TH1* h = (TH1*)hists.at(i).h->Clone(Form("%s_ratio",hists.at(i).h->GetName()));
				h->Divide(hden);
				hratio.emplace_back(h);
			}
			return;
		}
		void draw() override{
			getRatio();
			uppad->cd();
			pad = uppad;
			gStyle->SetOptStat(0);
			((TPad*)gPad)->SetTickx(1);
			((TPad*)gPad)->SetTicky(1);
			int i=0; bool kframe = 1;
			for(auto &it : hists){
				default_style(it.h, plot_default_setup::color[i]);
				if(kframe){
					hframe_up = it.h;
					uppad_style(hframe_up);
					kframe = 0;
				}
				it.h->Draw("same");
				gPad->SetLogy(doLogy);
				i++;}
			downpad->cd(); i=0;
			((TPad*)gPad)->SetTickx(1);
			((TPad*)gPad)->SetTicky(1);
			gStyle->SetOptStat(0);
			line.SetLineStyle(2);
			kframe = 1;
			for(auto &it : hratio){
				default_style(it, plot_default_setup::color[i+1]);
				if(kframe){
					kframe =0;
					hframe_down = it;
					downpad_style(it);
				}
				it->Draw("same");
				line.DrawLine(xmin, 1, xmax, 1);
				i++;}
		}


		TPad *uppad, *downpad;
		std::vector<TH1*> hratio;
		TH1* hden, *hframe_down, *hframe_up;
		float width=350, height=350;
		float rymin = 0.5, rymax = 1.5;
};

#endif
