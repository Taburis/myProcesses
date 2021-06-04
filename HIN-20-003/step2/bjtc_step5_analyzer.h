
#include "myProcesses/jtc/plugin/workflow.h"
//#include "myProcesses/jtc/plugin/Utility.h"
#include "myProcesses/jtc/plugin/plotLib.h"
#include "myProcesses/jtc/plugin/jtcTH1Player.h"

// this error table contained the order:
// trigger, JER, JEC, tracking, trackingResidual, decont, tagging bias
int nsources_bjet = 6;
int nsources_incl = 4;
TString systemError_bjet_item[] ={"trigger", "JER", "JEC", "Tracking", "Decont.", "Selec. Bias"};
TString systemError_incl_item[] ={"trigger", "JER", "JEC", "Tracking"};
double systemError_bjet[] = {0.03, .04, .03, .058, .02, 0.05};
double systemError_incl[] = {0.03, .04, .03, .058};

int nsources_bjet_ratio = 5;
int nsources_incl_ratio = 3;
double systemError_bjet_ratio[] = {0.03, .04, .03, .05, 0.05};
double systemError_incl_ratio[] = {0.03, .04, .03};

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
		void caption_pp_pb(TCanvas* c);
		void caption_pb(TCanvas* c);
		void syst_error_split_plot();
		void set_bin_err(TH1* h);
		void syst_error_breakdown_plot();
		virtual void analyze()override;
		void cms_caption(float, float, float);
		void cent_caption(float, float, float,TString);

		void merge_relativeError(TH1* h1, TH1* h2);
		jtcTH1Player * relativeError(TString name, int n, int m, float frac);

		jtcTH1Player * js_bjet, *js_bjet_err, *js_incl, *js_incl_err;
		jtcTH1Player *js_bjet_err2, *js_incl_err2;
		jtcTH1Player * js_bjet_pp, *js_bjet_pp_err, *js_incl_pp, *js_incl_pp_err;

		jtcTH1Player * js_bjet_Pb_data, *js_bjet_Pb_SystError;
		jtcTH1Player * js_incl_Pb_data, *js_incl_Pb_SystError;
		jtcTH1Player * js_bjet_pp_data, *js_bjet_pp_SystError;
		jtcTH1Player * js_incl_pp_data, *js_incl_pp_SystError;

		jtcTH1Player *p_incl_pp_data, *p_incl_pp_data_syst;
		jtcTH1Player *p_bjet_pp_data, *p_bjet_pp_data_syst;
		jtcTH1Player *p_incl_pb_data, *p_incl_pb_data_syst;
		jtcTH1Player *p_bjet_pb_data, *p_bjet_pb_data_syst;

		jtcTH1Player * js_ratio_b2Incl_Pb_data , *js_ratio_b2Incl_Pb_systError;
		jtcTH1Player * js_ratio_b2Incl_pp_data , *js_ratio_b2Incl_pp_systError;
		jtcTH1Player * js_ratio_bjet_data , *js_ratio_bjet_systError;
		jtcTH1Player * js_ratio_incl_data , *js_ratio_incl_systError;

		TCanvas * fig1();
		TCanvas * fig2();
		TCanvas * fig3();
		TCanvas * fig4();
		TCanvas * fig5(); // incl ratio
		TCanvas * fig6();
		TCanvas * fig7();
		TCanvas * fig_JECUncert();
		TCanvas * P_overlay();
		TCanvas * P_diff_bjet();
		void post_check();
		TFile * pbfile, *ppfile, *systf;
		TString pprefer_path,  systFilePath;
		TLatex cms, cent, txt;
		TLine line;
};

void bjtc_step5_analyzer::merge_relativeError(TH1* h1, TH1* err){
	for(int j=1; j<h1->GetNbinsX()+1; j++){
		double cont = h1->GetBinContent(j);
		double rate = fabs(1-err->GetBinContent(j));
		double error = h1->GetBinError(j);
		h1->SetBinError(j, pow(pow(error,2)+pow(rate*cont, 2),0.5));
	}
}


void bjtc_step5_analyzer::caption_pp_pb(TCanvas *c){
	txt.SetTextFont(42);
	txt.SetTextSize(0.04);
	c->cd(0);
	txt.DrawLatexNDC(0.1, 0.94, "#sqrt{s_{NN}} = 5.02 TeV,  PbPb 1.7 nb^{-1}, pp 27.4 pb^{-1}      anti-k_{T} jet (R = 0.4): #font[12]{p}_{T}^{jet} > 120 GeV, |#font[15]{#eta}_{jet}| < 1.6");
}

void bjtc_step5_analyzer::caption_pb(TCanvas *c){
	txt.SetTextFont(42);
	txt.SetTextSize(0.04);
	c->cd(0);
	txt.DrawLatexNDC(0.1, 0.94, "#sqrt{s_{NN}} = 5.02 TeV,  PbPb 1.7 nb^{-1}      anti-k_{T} jet (R = 0.4): #font[12]{p}_{T}^{jet} > 120 GeV, |#font[15]{#eta}_{jet}| < 1.6");
}

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
	h->GetYaxis()->SetTitleOffset(1.4);
	h->GetYaxis()->SetTitleSize(0.07);
	h->GetYaxis()->SetLabelSize(0.07);
	h->GetYaxis()->SetLabelOffset(0.017);
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
	bool use2err = 1;
	pbfile = TFile::Open(output+"/bjtc_step4_output.root");	
	systf = TFile::Open(systFilePath);	
	js_bjet = new jtcTH1Player("js_bjet/js_bjet_data_*_*",base->npt, base->ncent);
	js_bjet_err = new jtcTH1Player("js_bjet/js_bjet_data_bkgError_*_*",base->npt, base->ncent);
	js_bjet_err2 = new jtcTH1Player("js_bjet/js_bjet_data_bkgError2_*_*",1, base->ncent);
	js_incl = new jtcTH1Player("js_inclusive/js_inclusive_data_*_*",base->npt, base->ncent);
	js_incl_err = new jtcTH1Player("js_inclusive/js_inclusive_data_bkgError_*_*",base->npt, base->ncent);
	js_incl_err2= new jtcTH1Player("js_inclusive/js_inclusive_data_bkgError2_*_*",1, base->ncent);
	js_bjet->autoLoad(pbfile);
	js_bjet_err->autoLoad(pbfile);
	js_bjet_err2->autoLoad(pbfile);
	js_incl->autoLoad(pbfile);
	js_incl_err->autoLoad(pbfile);
	js_incl_err2->autoLoad(pbfile);


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
	if(use2err){
		js_incl_Pb_SystError = (jtcTH1Player*) js_incl_err2->clone("js_incl_Pb_systError");
		js_bjet_Pb_SystError = (jtcTH1Player*) js_bjet_err2->clone("js_bjet_Pb_systError");
	}else{
		js_incl_Pb_SystError = js_incl_err->contractX("js_incl_Pb_systError");
		js_bjet_Pb_SystError = js_bjet_err->contractX("js_bjet_Pb_systError");
	}
	js_bjet_Pb_data = js_bjet->contractX("js_bjet_Pb_data");
	auto js_bjet_Pb_SystError_forRatio = js_bjet_err->contractX("js_bjet_Pb_systError_forRatio");
	auto js_incl_Pb_SystError_forRatio = js_incl_err->contractX("js_bjet_Pb_systError_forRatio");
	//merge the systematic error from decontamination:
	//auto syst_err_decont = new jtcTH1Player("syst/syst_decont_*_*", 1, base->ncent);
	//syst_err_decont->autoLoad(pbfile);
/*
	for(int j= 0; j<base->ncent; j++){
		auto h = js_bjet_Pb_SystError->at(0,j);
		merge_relativeError(h, syst_err_decont->at(0,j));
		h = js_bjet_Pb_SystError_forRatio->at(0,j);
		merge_relativeError(h, syst_err_decont->at(0,j));
	}
*/

	for(int i=0; i<	nsources_bjet; ++i){
		js_bjet_Pb_SystError->mergeError(systemError_bjet[i]);
	}
	for(int i=0; i<	nsources_incl; ++i){
		js_incl_Pb_SystError->mergeError(systemError_incl[i]);
	}
	for(int i=0; i<	nsources_bjet_ratio; ++i){
		js_incl_Pb_SystError_forRatio->mergeError(systemError_bjet_ratio[i]);
	}
	for(int i=0; i<	nsources_incl_ratio; ++i){
		js_incl_Pb_SystError_forRatio->mergeError(systemError_incl_ratio[i]);
	}
	//auto js_bias_systError = new jtcTH1Player("js_bjet_taggingBias_systUncert_*_*",1, base->ncent);
	//js_bias_systError->autoLoad(systf);
	//js_bjet_Pb_SystError->mergeError(js_bias_systError);


	js_incl_pp_data = js_incl_pp->contractX("js0_incl_pp_data");
	js_incl_pp_SystError = js_incl_pp_err->contractX("js0_incl_pp_systError");
	js_bjet_pp_data = js_bjet_pp->contractX("js0_bjet_pp_data");
	js_bjet_pp_SystError = js_bjet_pp_err->contractX("js0_bjet_pp_systError");

	p_incl_pp_data=(jtcTH1Player*) js_incl_pp_data->clone("P_incl_pp_data");
	p_incl_pp_data_syst=(jtcTH1Player*) js_incl_pp_SystError->clone("P_incl_pp_data_syst");
	p_incl_pb_data=(jtcTH1Player*) js_incl_Pb_data->clone("P_incl_pb_data");
	p_incl_pb_data_syst=(jtcTH1Player*) js_incl_Pb_SystError->clone("P_incl_pb_data_syst");
	p_bjet_pb_data=(jtcTH1Player*) js_bjet_Pb_data->clone("P_bjet_pb_data");
	p_bjet_pb_data_syst=(jtcTH1Player*) js_bjet_Pb_SystError->clone("P_bjet_pb_data_syst");
	p_bjet_pp_data=(jtcTH1Player*) js_bjet_pp_data->clone("P_bjet_pp_data");
	p_bjet_pp_data_syst=(jtcTH1Player*) js_bjet_pp_SystError->clone("P_bjet_pp_data_syst");

	js_incl_pp_data->area_normalize(0,1.0,      "width");
	js_incl_pp_SystError->area_normalize(0,1.0, "width");
	js_bjet_pp_data->area_normalize(0,1.0,      "width");
	js_bjet_pp_SystError->area_normalize(0,1.0, "width");
	js_incl_Pb_data->area_normalize(0,1.0,     "width");
	js_incl_Pb_SystError->area_normalize(0,1.0,"width");
	js_bjet_Pb_data->area_normalize(0,1.0,     "width");
	js_bjet_Pb_SystError->area_normalize(0,1.0,"width");
	js_bjet_Pb_SystError_forRatio->area_normalize(0,1.0,"width");
	js_incl_Pb_SystError_forRatio->area_normalize(0,1.0,"width");
	/*
	*/

	js_incl_pp_data->duplicateY("js_incl_pp_data", 3);
	js_incl_pp_SystError->duplicateY("js_incl_pp_systError",3);
	js_bjet_pp_data->duplicateY("js_bjet_pp_data",3);
	js_bjet_pp_SystError->duplicateY("js_bjet_pp_systError",3);

	js_ratio_b2Incl_Pb_data      =(jtcTH1Player*) js_bjet_Pb_data->divide(*js_incl_Pb_data);
	js_ratio_b2Incl_Pb_systError =(jtcTH1Player*) js_bjet_Pb_SystError_forRatio->divide(*js_incl_Pb_SystError_forRatio);
	js_ratio_b2Incl_pp_data      =(jtcTH1Player*) js_bjet_pp_data->divide(*js_incl_pp_data);
	js_ratio_b2Incl_pp_systError =(jtcTH1Player*) js_bjet_pp_SystError->divide(*js_incl_pp_SystError);

	js_ratio_bjet_data      =(jtcTH1Player*) js_bjet_Pb_data->divide(*js_bjet_pp_data);
	js_ratio_bjet_systError =(jtcTH1Player*) js_bjet_Pb_SystError->divide(*js_bjet_pp_SystError);

	js_ratio_incl_data      =(jtcTH1Player*) js_incl_Pb_data->divide(*js_incl_pp_data);
	js_ratio_incl_systError =(jtcTH1Player*) js_incl_Pb_SystError->divide(*js_incl_pp_SystError);
}

TCanvas*  bjtc_step5_analyzer::fig1(){
	TCanvas * c = new TCanvas("cfig1","", 1600, 700);
	c->SetMargin(0.18,0.05,0.15,0.1);
	c->Divide(3,1, 0,0);
	gStyle->SetOptStat(0);
	TLegend* tl=new TLegend(0.5,0.75,0.95,0.93); tl->SetLineColor(0);
	for(int i=0; i<3; i++){
		c->cd(3-i);
		TH1* h = js_bjet_Pb_SystError->at(0,i);
		h->SetAxisRange(0.01, 100, "Y");
		h->SetAxisRange(0., .99, "X");
		hist_style_error(h,kBlue+2, i==1);
		h->GetYaxis()->SetTitle("#rho(#Deltar)");
		h->Draw("e2");
		if(i==0) tl->AddEntry(h, "b jets", "pf");
		h = js_bjet_Pb_data->at(0,i);
		hist_style_data(h,kBlue+2, i==1);
		gPad->SetLogy();
		h->Draw("same");

		h = js_incl_Pb_SystError->at(0,i);
		hist_style_error(h,kRed+2);
		h->Draw("e2same");
		if(i==0) tl->AddEntry(h, "Inclusive", "pf");
		h = js_incl_Pb_data->at(0,i);
		hist_style_data(h,kRed+2);
		gPad->SetLogy();
		h->Draw("same");
	}
	c->cd(1);
	cms_caption(0.24,0.93,0.07);
	cent_caption(0.26,0.86,0.05, "Cent:30-90%");
	c->cd(2);
	cent_caption(0.08,0.86,0.062, "Cent:10-30%");
	c->cd(3);
	tl->Draw();
	cent_caption(0.08,0.86,0.062, "Cent:0-10%");
	caption_pb(c);
	return c;
}


TCanvas*  bjtc_step5_analyzer::fig2(){
	TCanvas * c = new TCanvas("cfig2","", 1600, 700);
	c->SetMargin(0.18,0.05,0.15,0.1);
	c->Divide(3,1, 0,0);
	gStyle->SetOptStat(0);
	line.SetLineStyle(2);
	TLegend* tl=new TLegend(0.5,0.75,0.95,0.93); tl->SetLineColor(0);
	for(int i=0; i<3; i++){
		c->cd(3-i);
		TH1* h = js_ratio_b2Incl_Pb_systError->at(0,i);
		h->SetAxisRange(0.5, 2.7, "Y");
		h->SetAxisRange(0., .99, "X");
		hist_style_error(h,kBlue+2, i==1);
		h->GetYaxis()->SetTitle("#rho(#Deltar)_{b}/#rho(#Deltar)_{incl.}");
		h->Draw("e2");
		if(i==0) tl->AddEntry(h, "PbPb", "pf");
		h = js_ratio_b2Incl_Pb_data->at(0,i);
		hist_style_data(h,kBlue+2, i==1);
		h->Draw("same");
		line.DrawLine(0, 1, 1,1);

		h = js_ratio_b2Incl_pp_systError->at(0,i);
		hist_style_error(h,kRed+2);
		h->Draw("e2same");
		if(i==0) tl->AddEntry(h, "pp", "pf");
		h = js_ratio_b2Incl_pp_data->at(0,i);
		hist_style_data(h,kRed+2);
		h->Draw("same");
		line.DrawLine(0, 1, 1,1);
	}
	c->cd(1);
	cms_caption(0.24,0.93,0.07);
	cent_caption(0.26,0.86,0.05, "Cent:30-90%");
	c->cd(2);
	cent_caption(0.08,0.86,0.062, "Cent:10-30%");
	c->cd(3);
	tl->Draw();
	cent_caption(0.08,0.86,0.062, "Cent:0-10%");
	caption_pp_pb(c);
	return c;
}

TCanvas*  bjtc_step5_analyzer::fig3(){
	TCanvas * c = new TCanvas("cfig3","", 1600, 700);
	c->SetMargin(0.18,0.05,0.15,0.1);
	c->Divide(3,1, 0,0);
	gStyle->SetOptStat(0);
	TLegend* tl=new TLegend(0.5,0.75,0.95,0.93); tl->SetLineColor(0);
	for(int i=0; i<3; i++){
		c->cd(3-i);
		TH1* h = js_incl_Pb_SystError->at(0,i);
		h->SetAxisRange(0.01, 100, "Y");
		h->SetAxisRange(0., .99, "X");
		hist_style_error(h,kBlue+2, i==1);
		h->GetYaxis()->SetTitle("#rho(#Deltar)");
		h->Draw("e2");
		if(i==0) tl->AddEntry(h, "PbPb", "pf");
		h = js_incl_Pb_data->at(0,i);
		hist_style_data(h,kBlue+2, i==1);
		gPad->SetLogy();
		h->Draw("same");

		h = js_incl_pp_SystError->at(0,i);
		hist_style_error(h,kRed+2);
		h->Draw("e2same");
		if(i==0) tl->AddEntry(h, "pp", "pf");
		h = js_incl_pp_data->at(0,i);
		hist_style_data(h,kRed+2);
		gPad->SetLogy();
		h->Draw("same");
	}
	c->cd(1);
	cms_caption(0.24,0.93,0.07);
	cent_caption(0.26,0.86,0.05, "Cent:30-90%");
	c->cd(2);
	cent_caption(0.08,0.86,0.062, "Cent:10-30%");
	c->cd(3);
	tl->Draw();
	cent_caption(0.08,0.86,0.062, "Cent:0-10%");
	caption_pp_pb(c);
	return c;
}

TCanvas*  bjtc_step5_analyzer::fig4(){
	TCanvas * c = new TCanvas("cfig4","", 1600, 700);
	c->SetMargin(0.18,0.05,0.15,0.1);
	c->Divide(3,1, 0,0);
	gStyle->SetOptStat(0);
	TLegend* tl=new TLegend(0.5,0.75,0.95,0.93); tl->SetLineColor(0);
	for(int i=0; i<3; i++){
		c->cd(3-i);
		TH1* h = js_bjet_Pb_SystError->at(0,i);
		h->SetAxisRange(0.01, 100, "Y");
		h->SetAxisRange(0., .99, "X");
		hist_style_error(h,kBlue+2, i==1);
		h->GetYaxis()->SetTitle("#rho(#Deltar)");
		h->Draw("e2");
		if(i==0) tl->AddEntry(h, "PbPb", "pf");
		h = js_bjet_Pb_data->at(0,i);
		hist_style_data(h,kBlue+2, i==1);
		gPad->SetLogy();
		h->Draw("same");

		h = js_bjet_pp_SystError->at(0,i);
		hist_style_error(h,kRed+2);
		h->Draw("e2same");
		if(i==0) tl->AddEntry(h, "pp", "pf");
		h = js_bjet_pp_data->at(0,i);
		hist_style_data(h,kRed+2);
		gPad->SetLogy();
		h->Draw("same");
	}
	c->cd(1);
	cms_caption(0.24,0.93,0.07);
	cent_caption(0.26,0.86,0.05, "Cent:30-90%");
	c->cd(2);
	cent_caption(0.08,0.86,0.062, "Cent:10-30%");
	c->cd(3);
	tl->Draw();
	cent_caption(0.08,0.86,0.062, "Cent:0-10%");
	caption_pp_pb(c);
	return c;
}

TCanvas*  bjtc_step5_analyzer::fig5(){
	TCanvas * c = new TCanvas("cfig5","", 1600, 700);
	c->SetMargin(0.18,0.05,0.15,0.1);
	c->Divide(3,1, 0,0);
	gStyle->SetOptStat(0);
	line.SetLineStyle(2);
	TLegend* tl=new TLegend(0.5,0.75,0.95,0.93); tl->SetLineColor(0);
	for(int i=0; i<3; i++){
		c->cd(3-i);
		TH1* h = js_ratio_bjet_systError->at(0,i);
		h->SetAxisRange(0., 3, "Y");
		h->SetAxisRange(0., .99, "X");
		hist_style_error(h,kBlue+2, i==1);
		h->GetYaxis()->SetTitle("#rho(#Deltar)_{PbPb}/#rho(#Deltar)_{pp}");
		h->Draw("e2");
		if(i==0) tl->AddEntry(h, "b jets", "pf");
		h = js_ratio_bjet_data->at(0,i);
		hist_style_data(h,kBlue+2, i==1);
		h->Draw("same");
		line.DrawLine(0, 1, 1,1);

		h = js_ratio_incl_systError->at(0,i);
		hist_style_error(h,kRed+2);
		h->Draw("e2same");
		if(i==0) tl->AddEntry(h, "Inclusive", "pf");
		h = js_ratio_incl_data->at(0,i);
		hist_style_data(h,kRed+2);
		h->Draw("same");
		line.DrawLine(0, 1, 1,1);
	}
	c->cd(1);
	cms_caption(0.24,0.93,0.07);
	cent_caption(0.26,0.86,0.05, "Cent:30-90%");
	c->cd(2);
	cent_caption(0.08,0.86,0.062, "Cent:10-30%");
	c->cd(3);
	tl->Draw();
	cent_caption(0.08,0.86,0.062, "Cent:0-10%");
	caption_pp_pb(c);
	return c;
}
TCanvas*  bjtc_step5_analyzer::fig6(){
	TCanvas * c = new TCanvas("cfig6","", 1800, 1300);
	c->SetMargin(0.18,0.05,0.15,0.15);
	c->Divide(3,2, 0,0);
	gStyle->SetOptStat(0);
	line.SetLineStyle(2);
	TLegend* tl=new TLegend(0.5,0.75,0.95,0.93); tl->SetLineColor(0);
	for(int i=0; i<3; i++){
		c->cd(3-i);
		TH1* h = js_bjet_Pb_SystError->at(0,i);
		h->SetAxisRange(0.01, 100, "Y");
		h->SetAxisRange(0., .99, "X");
		hist_style_error(h,kBlue+2, i==1);
		h->GetYaxis()->SetTitle("#rho(#Deltar)");
		h->Draw("e2");
		if(i==0) tl->AddEntry(h, "b jets", "pf");
		h = js_bjet_Pb_data->at(0,i);
		hist_style_data(h,kBlue+2, i==1);
		gPad->SetLogy();
		h->Draw("same");

		h = js_incl_Pb_SystError->at(0,i);
		hist_style_error(h,kRed+2);
		h->Draw("e2same");
		if(i==0) tl->AddEntry(h, "Inclusive", "pf");
		h = js_incl_Pb_data->at(0,i);
		hist_style_data(h,kRed+2);
		gPad->SetLogy();
		h->Draw("same");

		c->cd(6-i);
		h = js_ratio_bjet_systError->at(0,i);
		h->SetAxisRange(0., 2.5, "Y");
		h->SetAxisRange(0., .99, "X");
		hist_style_error(h,kBlue+2, i==1);
		h->GetYaxis()->SetTitle("#rho(#Deltar)_{PbPb}/#rho(#Deltar)_{pp}");
		h->Draw("e2");
		h = js_ratio_bjet_data->at(0,i);
		hist_style_data(h,kBlue+2, i==1);
		h->Draw("same");
		line.DrawLine(0, 1, 1,1);

		h = js_ratio_incl_systError->at(0,i);
		hist_style_error(h,kRed+2);
		h->Draw("e2same");
		h = js_ratio_incl_data->at(0,i);
		hist_style_data(h,kRed+2);
		h->Draw("same");
		line.DrawLine(0, 1, 1,1);
	}
	c->cd(1);
	cms_caption(0.24,0.93,0.07);
	cent_caption(0.26,0.86,0.05, "Cent:30-90%");
	c->cd(2);
	cent_caption(0.08,0.86,0.062, "Cent:10-30%");
	tl->Draw();
	cent_caption(0.08,0.86,0.062, "Cent:0-10%");
	txt.SetTextFont(42);
	txt.SetTextSize(0.025);
	c->cd(0);
	txt.DrawLatexNDC(0.055, 0.95, "#sqrt{s_{NN}} = 5.02 TeV, PbPb 1.7 nb^{-1}, pp 27.4 pb^{-1}, anti-k_{T} jet (R = 0.4): #font[12]{p}_{T}^{jet} > 120 GeV, |#font[15]{#eta}_{jet}| < 1.6");
	return c;
}

TCanvas*  bjtc_step5_analyzer::fig7(){
	TCanvas * c = new TCanvas("cfig7","", 1200, 1300);
	c->SetMargin(0.18,0.05,0.15,0.15);
	c->Divide(2,2, 0,0);
	gStyle->SetOptStat(0);
	line.SetLineStyle(2);
	TLegend* tl=new TLegend(0.5,0.75,0.95,0.93); tl->SetLineColor(0);
	for(int i=0; i<2; i++){
		c->cd(2-i);
		TH1* h = js_bjet_Pb_SystError->at(0,i);
		h->SetAxisRange(0.01, 100, "Y");
		h->SetAxisRange(0., .99, "X");
		hist_style_error(h,kBlue+2, i==1);
		h->GetYaxis()->SetTitle("#rho(#Deltar)");
		h->Draw("e2");
		if(i==0) tl->AddEntry(h, "b jets", "pf");
		h = js_bjet_Pb_data->at(0,i);
		hist_style_data(h,kBlue+2, i==1);
		gPad->SetLogy();
		h->Draw("same");

		h = js_incl_Pb_SystError->at(0,i);
		hist_style_error(h,kRed+2);
		h->Draw("e2same");
		if(i==0) tl->AddEntry(h, "Inclusive", "pf");
		h = js_incl_Pb_data->at(0,i);
		hist_style_data(h,kRed+2);
		gPad->SetLogy();
		h->Draw("same");

		c->cd(4-i);
		h = js_ratio_bjet_systError->at(0,i);
		h->SetAxisRange(0., 3, "Y");
		h->SetAxisRange(0., .99, "X");
		hist_style_error(h,kBlue+2, i==1);
		h->GetYaxis()->SetTitle("#rho(#Deltar)_{PbPb}/#rho(#Deltar)_{pp}");
		h->Draw("e2");
		if(i==0) tl->AddEntry(h, "b jets", "pf");
		h = js_ratio_bjet_data->at(0,i);
		hist_style_data(h,kBlue+2, i==1);
		h->Draw("same");
		line.DrawLine(0, 1, 1,1);

		h = js_ratio_incl_systError->at(0,i);
		hist_style_error(h,kRed+2);
		h->Draw("e2same");
		if(i==0) tl->AddEntry(h, "Inclusive", "pf");
		h = js_ratio_incl_data->at(0,i);
		hist_style_data(h,kRed+2);
		h->Draw("same");
		line.DrawLine(0, 1, 1,1);
	}
	c->cd(1);
	cms_caption(0.24,0.93,0.07);
	cent_caption(0.26,0.86,0.05, "Cent:30-90%");
	c->cd(2);
	tl->Draw();
	cent_caption(0.08,0.86,0.062, "Cent:0-30%");
	txt.SetTextFont(42);
	txt.SetTextSize(0.025);
	c->cd(0);
	txt.DrawLatexNDC(0.055, 0.95, "#sqrt{s_{NN}} = 5.02 TeV, PbPb 1.7 nb^{-1}, pp 27.4 pb^{-1}, anti-k_{T} jet (R = 0.4): #font[12]{p}_{T}^{jet} > 120 GeV, |#font[15]{#eta}_{jet}| < 1.6");
	return c;
}
TCanvas*  bjtc_step5_analyzer::P_overlay(){
	TCanvas * c = new TCanvas("cp1","", 1200, 700);
	c->SetMargin(0.18,0.05,0.15,0.1);
	c->Divide(2,1, 0,0);
	gStyle->SetOptStat(0);
	line.SetLineStyle(2);
	TLegend* tl=new TLegend(0.5,0.75,0.95,0.93); tl->SetLineColor(0);
	for(int i=0; i<2; i++){
		c->cd(2-i);
		TH1* h = p_bjet_pb_data_syst->at(0,i);
		h->SetAxisRange(1, 4000, "Y");
		h->SetAxisRange(0., .99, "X");
		hist_style_error(h,kBlue+2, i==1);
		h->GetYaxis()->SetTitle("P(#Deltar)_{b}");
		h->Draw("e2");
		if(i==0) tl->AddEntry(h, "PbPb", "pf");
		h = p_bjet_pb_data->at(0,i);
		hist_style_data(h,kBlue+2, i==1);
		h->Draw("same");
		//line.DrawLine(0, 1, 1,1);
		gPad->SetLogy();

		h = p_bjet_pp_data_syst->at(0,i);
		hist_style_error(h,kRed+2);
		h->Draw("e2same");
		if(i==0) tl->AddEntry(h, "pp", "pf");
		h = p_bjet_pp_data->at(0,i);
		hist_style_data(h,kRed+2);
		h->Draw("same");
		//line.DrawLine(0, 1, 1,1);
		gPad->SetLogy();
	}
	c->cd(1);
	cms_caption(0.24,0.93,0.07);
	cent_caption(0.26,0.86,0.05, "Cent:30-90%");
	c->cd(2);
	tl->Draw();
	cent_caption(0.08,0.86,0.062, "Cent:0-30%");
	caption_pp_pb(c);
	return c;
}

TCanvas*  bjtc_step5_analyzer::P_diff_bjet(){
	TCanvas * c = new TCanvas("cp1","", 1200, 700);
	c->SetMargin(0.18,0.05,0.15,0.1);
	c->Divide(2,1, 0,0);
	gStyle->SetOptStat(0);
	line.SetLineStyle(2);
	TLegend* tl=new TLegend(0.5,0.75,0.95,0.93); tl->SetLineColor(0);
	auto diff_syst = (jtcTH1Player*) p_bjet_pb_data_syst->add2("P_bjet_diff_syst", *p_bjet_pp_data_syst, 1,-1);
	auto diff = (jtcTH1Player*) p_bjet_pb_data->add2("P_bjet_diff", *p_bjet_pp_data, 1,-1);
	for(int i=0; i<2; i++){
		c->cd(2-i);
		TH1* h =(TH1*) p_bjet_pb_data_syst->at(0,i)->Clone(Form("diff_syst_%d",i));
		h->Add(h, p_bjet_pp_data_syst->at(0,i), 1, -1);
		cout<<h->GetName()<<endl;
		h->SetAxisRange(-200, 200, "Y");
		h->SetAxisRange(0., .99, "X");
		hist_style_error(h,kBlue+2, i==1);
		h->GetYaxis()->SetTitle("P(#Deltar)_{Pb}-P(#Deltar)_{pp}");
		h->Draw("e2");
		if(i==0) tl->AddEntry(h, "PbPb", "pf");
		h =(TH1*) p_bjet_pb_data->at(0,i)->Clone(Form("diff_%d",i));
		h->Add(h, p_bjet_pp_data->at(0,i), 1, -1);
		hist_style_data(h,kBlue+2, i==1);
		h->Draw("same");
		//line.DrawLine(0, 1, 1,1);
		/*
		   h = p_bjet_pp_data_syst->at(0,i);
		   hist_style_error(h,kRed+2);
		   h->Draw("e2same");
		   if(i==0) tl->AddEntry(h, "pp", "pf");
		   h = p_bjet_pp_data->at(0,i);
		   hist_style_data(h,kRed+2);
		   h->Draw("same");
		   */
		//line.DrawLine(0, 1, 1,1);
	}
	c->cd(1);
	cms_caption(0.24,0.93,0.07);
	cent_caption(0.26,0.86,0.05, "Cent:30-90%");
	c->cd(2);
	tl->Draw();
	cent_caption(0.08,0.86,0.062, "Cent:0-30%");
	caption_pp_pb(c);
	return c;
}

void bjtc_step5_analyzer::syst_error_breakdown_plot(){
	js_bjet_err = new jtcTH1Player("js_bjet/js_bjet_data_bkgError_*_*",base->npt, base->ncent);
	js_bjet_err->autoLoad(pbfile);
	jtcTH1Player* jsbjet[nsources_bjet+2];
	jsbjet[0] =(jtcTH1Player*) js_bjet_err->contractX("js_bjet_Pb_syst_bkg");
	for(int i=1; i<	nsources_bjet+1; ++i){
		jsbjet[i] =(jtcTH1Player*) jsbjet[0]->clone("js_bjet_Pb_syst_"+systemError_bjet_item[i]);
		jsbjet[i]->absContent(systemError_bjet[i-1]);
		jsbjet[i]->absError(0);
	}

	auto syst_err_decont = new jtcTH1Player("syst/syst_decont_*_*", 1, base->ncent);
	syst_err_decont->autoLoad(pbfile);
	jsbjet[5] = (jtcTH1Player*) syst_err_decont->clone("js_bjet_syst_decont");
	jsbjet[nsources_bjet+1] = new jtcTH1Player("js_bjet_taggingBias_systUncert_*_*",1, base->ncent);
	jsbjet[nsources_bjet+1]->autoLoad(systf);
	for(int j= 0 ;j<base->ncent; j++){
		auto h = jsbjet[5]->at(0,j);//decontamination
		auto h2 = jsbjet[nsources_bjet+1]->at(0,j); //tagging bias
		auto h3 = jsbjet[0]->at(0,j); 
		for(int k= 1; k<jsbjet[4]->at(0,0)->GetNbinsX()+1; k++){
			h->SetBinContent(k, fabs(h->GetBinContent(k)-1)+systemError_bjet[4]);
			h->SetBinError(k,0);
			h2->SetBinContent(k,fabs(h2->GetBinContent(k)-1));
			h2->SetBinError(k,0);
			float c3= h3->GetBinContent(k);
			float e3= h3->GetBinError(k);
			h3->SetBinContent(k, e3/c3);
			h3->SetBinError(k,0);
		}
	}
	auto c = new plotManager();
	c->initSquarePad("canvas_breakdown", "", 1,2);
	for(int i=0; i<	nsources_bjet+2; ++i){
		for(int j=0; j< 2; j++){
			auto h = jsbjet[i]->at(0,j);
			h->SetLineWidth(2);
			//for(int k=1; k<h->GetNbinsX()+1; k++){
			//	//h->SetBinContent(k,h->GetBinError(k));
			//	//h->SetBinError(k, 0);
			//}
		}
	}

	for(int i=1; i<	nsources_bjet+1; ++i){
		jsbjet[i]->at(0,0)->SetTitle("Systematic uncertainty: Cent:0-30%");
		jsbjet[i]->at(0,1)->SetTitle("Systematic uncertainty: Cent:30-90%");
		jsbjet[i]->at(0,0)->GetYaxis()->SetTitle("Relative uncertainty");
		jsbjet[i]->at(0,1)->GetYaxis()->SetTitle("Relative uncertainty");
		c->addm2TH1(jsbjet[i], systemError_bjet_item[i-1],"l");
	}
	c->addm2TH1(jsbjet[0], "Bkg", "l");
	c->addm2TH1(jsbjet[nsources_bjet+1], "taging Bias","l");

	c->setXrange(0,0.99);
	c->setYrange(-0.01,0.3);
	c->draw();
	c->drawLegend(0.3, 0.5, 0.7, 0.85);
	c->save(fig_output+"/system_breakdown_plot.png");

}

void bjtc_step5_analyzer::syst_error_split_plot(){
	//overlay the error by sources
	js_bjet_err = new jtcTH1Player("js_bjet/js_bjet_data_bkgError_*_*",base->npt, base->ncent);
	js_bjet_err->autoLoad(pbfile);
	jtcTH1Player* jsbjet[nsources_bjet+2];
	jsbjet[0] =(jtcTH1Player*) js_bjet_err->contractX("js_bjet_Pb_syst_bkg");
	for(int i=1; i<	nsources_bjet+1; ++i){
		jsbjet[i] =(jtcTH1Player*) jsbjet[0]->clone("js_bjet_Pb_syst_"+systemError_bjet_item[i]);
		jsbjet[i]->absError(0);
		jsbjet[i]->mergeError(systemError_bjet[i-1]);
	}
	auto js_bias_systError = new jtcTH1Player("js_bjet_taggingBias_systUncert_*_*",1, base->ncent);
	js_bias_systError->autoLoad(systf);
	jsbjet[nsources_bjet+1] = (jtcTH1Player*) jsbjet[0]->clone("js_bjet_Pb_syst_taggingBias");
	jsbjet[nsources_bjet+1]->absError(0);
	jsbjet[nsources_bjet+1]->mergeError(js_bias_systError);

	auto c = new plotManager();
	c->initSquarePad("canvas_splitting", "", 1,2);
	for(int i=0; i<	nsources_bjet+2; ++i){
		for(int j=0; j< 2; j++){
			auto h = jsbjet[i]->at(0,j);
			h->SetLineWidth(2);
			for(int k=1; k<h->GetNbinsX()+1; k++){
				h->SetBinContent(k,h->GetBinError(k));
				h->SetBinError(k, 0);
			}
		}
	}

	for(int i=1; i<	nsources_bjet+1; ++i){
		jsbjet[i]->at(0,0)->SetTitle("Systematic uncertainty: Cent:0-30%");
		jsbjet[i]->at(0,1)->SetTitle("Systematic uncertainty: Cent:30-90%");
		jsbjet[i]->at(0,0)->GetYaxis()->SetTitle("Absolute uncertainty");
		jsbjet[i]->at(0,1)->GetYaxis()->SetTitle("Absolute uncertainty");
		c->addm2TH1(jsbjet[i], systemError_bjet_item[i-1],"l");
	}
	c->addm2TH1(jsbjet[0], "Bkg", "l");
	c->addm2TH1(jsbjet[nsources_bjet+1], "taging Bias","l");

	c->setXrange(0,0.99);
	c->setYrange(0.01,1000);
	c->doLogy = 1;
	c->draw();

	c->drawLegend(0.6, 0.5, 0.9, 0.85);
	c->save(fig_output+"/system_split_plot.png");
}

void bjtc_step5_analyzer::post_check(){
	auto incl_err = (jtcTH1Player*) js_incl_err->contractX("incl_err1");
	auto bjet_err = (jtcTH1Player*) js_bjet_err->contractX("incl_err1");
	auto c = new plotManager();
	c->initOverlayPad("canvas_bkgerror", "", 1, 2);
	for(int i=0; i<1; i++){
		for(int j=0; j<bjet_err->Ncol(); j++){
			auto h = incl_err->at(i,j);
			h->SetTitle("signal pTweighted: Cent: 30-90%");
			set_bin_err(h);
			c->addHist(h, i, 1-j, "Int. after", "pl");
			h = js_incl_err2->at(i,j);
			h->SetTitle("signal pTweighted: Cent: 0-30%");
			set_bin_err(h);
			c->addHist(h, i, 1-j, "Int. first", "pl");
			((overlayPad*)c->at(i,1-j))->rymin = 0.0;
			((overlayPad*)c->at(i,1-j))->rymax = 2.;
		}
	}
	c->setXrange(0, 0.99);
	c->draw();	
	c->drawLegend();
	c->c->SaveAs(fig_output+"/post_check/bkg_error.pdf");
}

void bjtc_step5_analyzer::set_bin_err(TH1* h){
	for(int k=0; k<h->GetNbinsX()+1; k++){
		auto e  = h->GetBinError(k);
		h->SetBinContent(k, e);
		h->SetBinError(k, 0);
	}
}

void bjtc_step5_analyzer::analyze(){
	preprocess();
	   auto cfig1 = fig1();
	   cfig1->SaveAs(fig_output+"/figure_nominal_js_overlay.pdf");
	   auto cfig2 = fig2();
	   cfig2->SaveAs(fig_output+"/figure_nominal_js_ratio_b2Inclusive.pdf");
	   auto cfig3 = fig3();
	   cfig3->SaveAs(fig_output+"/figure_nominal_js_inclusive.pdf");
	   auto cfig4 = fig4();
	   cfig4->SaveAs(fig_output+"/figure_nominal_js_bjets.pdf");
	   auto cfig5 = fig5();
	   cfig5->SaveAs(fig_output+"/figure_nominal_js_ratio_overlay.pdf");
	   auto cfig6 = fig6();
	   cfig6->SaveAs(fig_output+"/figure_nominal_js_bjet_vs_incl.pdf");
	/*
	   auto pfig1 = P_overlay();
	   pfig1->SaveAs(fig_output+"/figure_P_bjet_overlay.pdf");
	   auto pfig2 = P_diff_bjet();
	   pfig2->SaveAs(fig_output+"/figure_P_bjet_diff.pdf");
	   auto cfig6 = fig6();
	   cfig6->SaveAs(fig_output+"/figure_nominal_js_bjet_vs_incl.pdf");
	   */
	//post_check();	
	//syst_error_split_plot();
	//syst_error_breakdown_plot();

}	
