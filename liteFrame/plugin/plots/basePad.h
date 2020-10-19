
#ifndef BASEPAD_H
#define BASEPAD_H

#include "TPad.h"
#include "TColor.h"
#include "TLine.h"
#include "TLatex.h"
#include "TStyle.h"

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
	basePad(TString name){pname = name;}
	~basePad(){}
	void default_style(TH1* h, Color_t color){
		h->SetMarkerStyle(marker);
		h->SetMarkerSize(markerSize);
		h->SetMarkerColor(color);
		h->SetLineColor(color);
	}
	virtual void draw() = 0;
	//	virtual void createPad() = 0;
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

	std::vector<histPack> hists;
	float xmin, xmax;
	TPad* pad;
	TLatex latex;
	TString pname;

	int marker = 20;
	float  markerSize = 0.8;
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
			gStyle->SetTitleX(0.56);
			gStyle->SetTitleY(0.95);
			//h->GetYaxis()->SetTitle(ytitle);
			h->GetYaxis()->SetTitleSize(0.07);
			h->GetYaxis()->SetTitleOffset(1.2);
			h->GetYaxis()->SetLabelSize(0.07);
			h->GetXaxis()->SetNdivisions(505);
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
			//h->GetXaxis()->SetTitle(xtitle);
			//h->GetYaxis()->SetTitle(ratio_title);
			h->GetYaxis()->CenterTitle();
			h->GetYaxis()->SetTitleSize(0.15);
			h->GetYaxis()->SetTitleOffset(0.5);

			h->SetAxisRange(rymin, rymax, "Y");
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
				//				gPad->SetLogy(doLogy);
				i++;}
			//if(doLegend) {
			//	legend->SetLineColor(kWhite); legend->Draw();
			//}
			hframe_down = this->getTH1Frame(hists.at(0).h, pname+"_downFrame");
			downpad->cd(); i=0;
			((TPad*)gPad)->SetTickx(1);
			((TPad*)gPad)->SetTicky(1);
			gStyle->SetOptStat(0);
			downpad_style(hframe_down);
			hframe_down->Draw(); 
			for(auto &it : hratio){
				default_style(it, plot_default_setup::color[i+1]);
				it->Draw("same");
//				l.DrawLine(xmin, 1, xmax, 1);
				i++;}
		}


		TPad *uppad, *downpad;
		std::vector<TH1*> hratio;
		TH1* hden, *hframe_down, *hframe_up;
		float width=350, height=350;
		float rymin = 0.5, rymax = 1.5;
};

#endif
