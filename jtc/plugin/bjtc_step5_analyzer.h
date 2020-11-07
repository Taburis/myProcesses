
#include "myProcesses/jtc/plugin/workflow.h"
#include "myProcesses/jtc/plugin/Utility.h"
#include "myProcesses/jtc/plugin/plotLib.h"
#include "myProcesses/jtc/plugin/jtcTH1Player.h"

// this error table contained the order:
// trigger, JER, JEC, tracking, trackingResidual
int nsources = 5;
double systemError[] = {0.03, .04, .03, .03};

class bjtc_step5_analyzer: public analyzer{
	public:
		bjtc_step5_analyzer(TString name, workflow &b0, ParaSet &ps0 ):analyzer(name,b0, ps0){
			//npt = ps0.getPara<int>("npt");
			//ncent = ps0.getPara<int>("ncent");
		}
		~bjtc_step5_analyzer(){}
		void preprocess();
		void hist_style_data(TH1*,Color_t, bool side=0);
		void hist_style_error(TH1*,Color_t, bool side=0);
		virtual void analyze()override;
		void cms_caption(float, float, float);
		void cent_caption(float, float, float,TString);

		jtcTH1Player * js_bjet, *js_bjet_err, *js_incl, *js_incl_err;
		jtcTH1Player * js_bjet_pp, *js_bjet_pp_err, *js_incl_pp, *js_incl_pp_err;

		jtcTH1Player * js_bjet_Pb_data, *js_bjet_Pb_SystError;
		jtcTH1Player * js_incl_Pb_data, *js_incl_Pb_SystError;
		jtcTH1Player * js_bjet_pp_data, *js_bjet_pp_SystError;
		jtcTH1Player * js_incl_pp_data, *js_incl_pp_SystError;

		jtcTH1Player * js_ratio_b2Incl_Pb_data , *js_ratio_b2Incl_Pb_systError;
		jtcTH1Player * js_ratio_b2Incl_pp_data , *js_ratio_b2Incl_pp_systError;

		TCanvas * fig2();
		TCanvas * fig_JECUncert();
		TFile * pbfile, *ppfile, *systf;
		TString pprefer_path;
		TLatex cms, cent;
};

void bjtc_step5_analyzer::cent_caption(float x, float y, float s, TString t){
	cent.SetTextFont(42);
	cent.SetTextSize(s);
	cent.DrawLatexNDC(x,y,t);
}

void bjtc_step5_analyzer::cms_caption(float x, float y, float s){
	cms.SetTextFont(62);
	cms.SetTextSize(s);
	cms.DrawLatexNDC(x,y,"CMS");
	cms.SetTextFont(52);
	cms.SetTextSize(s*0.95);
	cms.DrawLatexNDC(x+2.2*s,y,"Preliminary");
	//cms.DrawLatexNDC(x+2.2*s,y,"Preliminary");
}

void bjtc_step5_analyzer::hist_style_error(TH1* h,Color_t c1, bool side){
	h->SetTitle("");
	h->GetYaxis()->SetTitle("#rho(#Deltar)_{b}/#rho(#Deltar)_{incl.}");
	h->GetYaxis()->SetTitleSize(0.07);
	h->GetYaxis()->SetLabelSize(0.07);
	h->GetYaxis()->SetLabelOffset(0.02);
	h->GetYaxis()->CenterTitle();
	h->GetXaxis()->SetTitle("#Deltar");
	h->GetXaxis()->CenterTitle();
	h->SetMarkerStyle(20);
	h->SetFillColorAlpha(c1,0.4);
	h->SetFillStyle(1001);
	h->SetMarkerSize(1.4);
	h->SetLineColor(c1);
	h->SetMarkerColor(c1);
	h->GetXaxis()->SetNdivisions(505);
	h->GetYaxis()->SetNdivisions(505);
	if(side){
		h->GetXaxis()->SetLabelSize(0.07);
		h->GetXaxis()->SetLabelOffset(0.01);
		h->GetXaxis()->SetTitleSize(0.09);
		h->GetXaxis()->SetTitleOffset(0.8);
	}else {
		h->GetXaxis()->SetLabelSize(0.08);
		h->GetXaxis()->SetLabelOffset(0.005);
		h->GetXaxis()->SetTitleSize(0.098);
		h->GetXaxis()->SetTitleOffset(0.76);
	}

}

void bjtc_step5_analyzer::hist_style_data(TH1* h,Color_t c1, bool side){
	h->SetTitle("");
	h->GetXaxis()->SetTitle("#Deltar");
	h->GetXaxis()->CenterTitle();
	h->SetMarkerStyle(20);
	h->SetMarkerSize(1.4);
	h->SetLineColor(c1);
	h->SetMarkerColor(c1);
	
}

void bjtc_step5_analyzer::preprocess(){
	pbfile = TFile::Open(output+"/bjtc_step4_output.root");	
	systf = TFile::Open(output+"/bjtc_systematic.root");	
	js_bjet = new jtcTH1Player("js_bjet/js_bjet_data_*_*",base->npt, base->ncent);
	js_bjet_err = new jtcTH1Player("js_bjet/js_bjet_data_bkgError_*_*",base->npt, base->ncent);
	js_incl = new jtcTH1Player("js_inclusive/js_inclusive_data_*_*",base->npt, base->ncent);
	js_incl_err = new jtcTH1Player("js_inclusive/js_inclusive_data_bkgError_*_*",base->npt, base->ncent);
	js_bjet->autoLoad(pbfile);
	js_bjet_err->autoLoad(pbfile);
	js_incl->autoLoad(pbfile);
	js_incl_err->autoLoad(pbfile);

	ppfile = TFile::Open(pprefer_path);
	js_bjet_pp = new jtcTH1Player("dr_signal_bjtc_jetShape_step3_*_*",base->npt, 1);
	js_bjet_pp_err = new jtcTH1Player("dr_bjtc_jetShape_systError_*_*",base->npt, 1);
	js_incl_pp = new jtcTH1Player("dr_signal_dijtc_jetShape_step3_*_*",base->npt, 1);
	js_incl_pp_err = new jtcTH1Player("dr_dijtc_jetShape_systError_*_*",base->npt, 1);
	js_bjet_pp->autoLoad(ppfile);
	js_bjet_pp_err->autoLoad(ppfile);
	js_incl_pp->autoLoad(ppfile);
	js_incl_pp_err->autoLoad(ppfile);

	js_incl_Pb_data = js_incl->contractX("js_incl_Pb_data");
	js_incl_Pb_SystError = js_incl_err->contractX("js_incl_Pb_systError");
	js_bjet_Pb_data = js_bjet->contractX("js_bjet_Pb_data");
	js_bjet_Pb_SystError = js_bjet_err->contractX("js_bjet_Pb_systError");
	for(int i=0; i<	nsources; ++i){
		js_bjet_Pb_SystError->mergeError(systemError[i]);
		js_incl_Pb_SystError->mergeError(systemError[i]);
	}
	auto js_bias_systError = new jtcTH1Player("js_bjet_taggingBias_systUncert_*_*",1, base->ncent);
	js_bias_systError->autoLoad(systf);
	js_bjet_Pb_SystError->mergeError(js_bias_systError);


	js_incl_pp_data = js_incl_pp->contractX("js0_incl_pp_data");
	js_incl_pp_SystError = js_incl_pp_err->contractX("js0_incl_pp_systError");
	js_bjet_pp_data = js_bjet_pp->contractX("js0_bjet_pp_data");
	js_bjet_pp_SystError = js_bjet_pp_err->contractX("js0_bjet_pp_systError");

	js_incl_pp_data->duplicateY("js_incl_pp_data", 2);
	js_incl_pp_SystError->duplicateY("js_incl_pp_systError",2);
	js_bjet_pp_data->duplicateY("js_bjet_pp_data",2);
	js_bjet_pp_SystError->duplicateY("js_bjet_pp_systError",2);

	js_ratio_b2Incl_Pb_data      =(jtcTH1Player*) js_bjet_Pb_data->divide(*js_incl_Pb_data);
	js_ratio_b2Incl_Pb_systError =(jtcTH1Player*) js_bjet_Pb_SystError->divide(*js_incl_Pb_SystError);
	js_ratio_b2Incl_pp_data      =(jtcTH1Player*) js_bjet_pp_data->divide(*js_incl_pp_data);
	js_ratio_b2Incl_pp_systError =(jtcTH1Player*) js_bjet_pp_SystError->divide(*js_incl_pp_SystError);
}

TCanvas*  bjtc_step5_analyzer::fig2(){
	TCanvas * c = new TCanvas("c","", 1200, 700);
	c->SetMargin(0.18,0.05,0.15,0.1);
	c->Divide(2,1, 0,0);
	gStyle->SetOptStat(0);
	TLegend* tl=new TLegend(0.5,0.75,0.95,0.93); tl->SetLineColor(0);
	for(int i=0; i<2; i++){
		c->cd(2-i);
		TH1* h = js_ratio_b2Incl_Pb_systError->at(0,i);
		h->SetAxisRange(0.5, 2.7, "Y");
		h->SetAxisRange(0., .99, "X");
		hist_style_error(h,kBlue+2, i==1);
		h->Draw("e2");
		if(i==0) tl->AddEntry(h, "PbPb", "pf");
		h = js_ratio_b2Incl_Pb_data->at(0,i);
		hist_style_data(h,kBlue+2, i==1);
		h->Draw("same");

		h = js_ratio_b2Incl_pp_systError->at(0,i);
		hist_style_error(h,kRed+2);
		h->Draw("e2same");
		if(i==0) tl->AddEntry(h, "pp", "pf");
		h = js_ratio_b2Incl_pp_data->at(0,i);
		hist_style_data(h,kRed+2);
		h->Draw("same");
	}
	c->cd(1);
	cms_caption(0.24,0.93,0.07);
	cent_caption(0.26,0.86,0.05, "Cent:30-90%");
	c->cd(2);
	tl->Draw();
	cent_caption(0.08,0.86,0.064, "Cent:0-30%");
	return c;
}

void bjtc_step5_analyzer::analyze(){
	preprocess();
	auto cfig2 = fig2();
	cfig2->SaveAs(fig_output+"/figure_js_ratio_b2Inclusive.pdf");

}	
