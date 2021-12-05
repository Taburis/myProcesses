
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

int nsources_bjet_ratio = 5;
int nsources_incl_ratio = 3;
double systemError_bjet_ratio[] = {0.03, .03, .03, .05, 0.05};
double systemError_incl_ratio[] = {0.03, .03, .03};

class bjtc_step5_analyzer: public analyzer{
	public:
		bjtc_step5_analyzer(TString name, workflow &b0, ParaSet &ps0 ):analyzer(name,b0, ps0){
			syst_b_dict["Trigger"]  = 0.03;
			syst_b_dict["Tracking"] = 0.058;
			syst_b_dict["JEC+JER"]  = 0.043;
			syst_b_dict["Tagging"]  = 0.05;
			syst_b_dict["Decont"]   = 0.08;
			//npt = ps0.getPara<int>("npt");
			//ncent = ps0.getPara<int>("ncent");
			syst_b.push_back(pair("Trigger", 0.03)); // trigger
			syst_b.push_back(pair("Tracking", 0.058)); // tracking
			syst_b.push_back(pair("JEC", 0.03)); // JEC
			syst_b.push_back(pair("JER", 0.03)); // JER
			syst_b.push_back(pair("Bias", 0.05)); // tagging bias
			syst_b.push_back(pair("Decont.", 0.08)); // decontamination bias

			syst_incl.push_back(pair<TString, float>("Trigger", 0.03)); // trigger
			syst_incl.push_back(pair<TString, float>("Tracking", 0.058)); // tracking
			syst_incl.push_back(pair<TString, float>("JEC", 0.03)); // JEC
			syst_incl.push_back(pair<TString, float>("JER", 0.03)); // JER

			syst_ratio_b2incl_b.push_back(pair<TString, float>("Trigger", 0.03)); // trigger
			syst_ratio_b2incl_b.push_back(pair<TString, float>("Tagging Bias",0.05)); // tagging bias
			syst_ratio_b2incl_b.push_back(pair<TString, float>("Decont.", 0.08)); // decontamination bias
			syst_ratio_b2incl_incl.push_back(pair<TString, float>("Trigger", 0.03)); // trigger
		}
		~bjtc_step5_analyzer(){}
		void preprocess(TString, TString);
		void merge_relative_sources(jtcTH1Player *js, vector<pair<TString,float>> &source);
		void plot_dr_ratio(TString, TString, TString, jtcTH1Player*, jtcTH1Player *);
		void hist_style_data(TH1*,Color_t, bool side=0);
		void hist_style_error(TH1*,Color_t, bool side=0);
		void caption_pp_pb(TCanvas* c);
		void caption_pb(TCanvas* c);
		void syst_error_breakdown();
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
		TCanvas * P_fig1();
		TCanvas * P_fig2();
		TCanvas * P_fig3();
		TCanvas * P_diff_bjet();
		void  probe_plots();
		void post_check();
		TFile * pbfile, *ppfile, *systf;
		TString pprefer_path,  systFilePath;
		TLatex cms, cent, txt;
		TLine line;
		map<TString, float> syst_b_dict;
		vector<std::pair<TString, float>> syst_ratio_b2incl_b, syst_ratio_b2incl_incl;
		vector<std::pair<TString, float>> syst_b, syst_incl;
};

void bjtc_step5_analyzer::merge_relative_sources(jtcTH1Player *js, vector<pair<TString,float>> &source){
	for(auto & p : source){
		js->mergeError(p.second);
	}
}

void bjtc_step5_analyzer::merge_relativeError(TH1* h1, TH1* err){
	// merge the relative error into the signal
	for(int j=1; j<h1->GetNbinsX()+1; j++){
		double cont = h1->GetBinContent(j);
		double rate = fabs(1-err->GetBinContent(j));
		double error = h1->GetBinError(j);
		h1->SetBinError(j, pow(pow(error,2)+pow(rate*cont, 2),0.5));
	}
}


void bjtc_step5_analyzer::caption_pp_pb(TCanvas *c){
	txt.SetTextFont(42);
	txt.SetTextSize(0.056);
	c->cd(0);
	txt.DrawLatexNDC(0.07, 0.94, "#sqrt{s_{NN}} = 5.02 TeV,  PbPb 1.7 nb^{-1}, pp 27.4 pb^{-1},     anti-k_{T} jet (R = 0.4): #font[12]{p}_{T}^{jet} > 120 GeV, |#font[15]{#eta}_{jet}| < 1.6");
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
		h->GetXaxis()->SetLabelOffset(0.012);
		h->GetXaxis()->SetTitleSize(0.085);
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

void bjtc_step5_analyzer::preprocess(TString name, TString name2){
	bool use2err = 1;
	pbfile = TFile::Open(name);	
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
	auto js_bjet_Pb_SystError_forRatio = (jtcTH1Player*) js_bjet_Pb_SystError->clone("js_bjet_Pb_systError_forRatio");
	auto js_incl_Pb_SystError_forRatio = (jtcTH1Player*) js_incl_Pb_SystError->clone("js_bjet_Pb_systError_forRatio");
	merge_relative_sources(js_bjet_Pb_SystError_forRatio, syst_ratio_b2incl_b);
	merge_relative_sources(js_incl_Pb_SystError_forRatio, syst_ratio_b2incl_incl);

	//merge_relative_sources(js_bjet_err, syst_b);
	//merge_relative_sources(js_incl_err, syst_incl);

	//merge_relative_sources(js_bjet_Pb_SystError, syst_b);
	//merge_relative_sources(js_incl_Pb_SystError, syst_incl);

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

	js_incl_pp_data->area_normalize(0, 1.0,    "width");
	js_incl_pp_SystError->area_normalize(0,1.0,"width");
	js_bjet_pp_data->area_normalize(0,1.0,     "width");
	js_bjet_pp_SystError->area_normalize(0,1.0,"width");
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

	auto fout = TFile::Open(name2, "update");
	js_incl_Pb_data->write();
	js_bjet_Pb_data->write();
	js_incl_pp_data->write();
	js_bjet_pp_data->write();
	js_ratio_b2Incl_Pb_data->write();
	js_ratio_b2Incl_pp_data->write();
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
	TLegend* tl=new TLegend(0.55,0.8,0.95,0.99); tl->SetLineColor(0);
	for(int i=0; i<3; i++){
		c->cd(3-i);
		TH1* h = js_ratio_b2Incl_Pb_systError->at(0,i);
		h->SetAxisRange(0.5, 2.7, "Y");
		h->SetAxisRange(0., .99, "X");
		hist_style_error(h,kBlue+2, i==2);
		h->GetYaxis()->SetTitle("#rho(#Deltar)_{b}/#rho(#Deltar)_{incl.}");
		h->Draw("e2");
		if(i==0) tl->AddEntry(h, "PbPb", "pf");
		h = js_ratio_b2Incl_Pb_data->at(0,i);
		hist_style_data(h,kBlue+2, i==2);
		h->Draw("same");
		line.DrawLine(0, 1, 1,1);

		h = js_ratio_b2Incl_pp_systError->at(0,i);
		hist_style_error(h,kRed+2);
		h->SetAxisRange(0.5, 2.7, "Y");
		h->SetAxisRange(0., .99, "X");
		h->GetYaxis()->SetTitle("#rho(#Deltar)_{b}/#rho(#Deltar)_{incl.}");
		//h->Draw("e2");
		h->Draw("e2same");
		if(i==0) tl->AddEntry(h, "pp", "pf");
		h = js_ratio_b2Incl_pp_data->at(0,i);
		hist_style_data(h,kRed+2, i==2);
		h->Draw("same");
		line.DrawLine(0, 1, 1,1);
	}
	c->cd(1);
	cms_caption(0.24,0.93,0.07);
	cent_caption(0.26,0.85,0.052, "Cent:30-90%");
	c->cd(2);
	cent_caption(0.08,0.85,0.062, "Cent:10-30%");
	c->cd(3);
	tl->Draw();
	cent_caption(0.08,0.85,0.062, "Cent:0-10%");
	caption_pp_pb(c);

	c->cd(0);
	TBox bx; bx.SetFillColor(kWhite); bx.SetFillStyle(1001);
//	bx.SetFillColor(kBlack);
	bx.DrawBox(0.36, 0.08, 0.39, 0.13);
	bx.DrawBox(0.66, 0.08, 0.69, 0.13);
	bx.DrawBox(0.97, 0.08, 0.99, 0.13);
	txt.SetTextSize(0.06);
	txt.DrawLatex(0.365, 0.08, "1");
	txt.DrawLatex(0.67,  0.08, "1");
	txt.DrawLatex(0.975, 0.08, "1");
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
		h->SetAxisRange(-1., 3.9, "Y");
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
	c->cd(3);
	tl->Draw();
	cent_caption(0.08,0.86,0.062, "Cent:0-10%");
	txt.SetTextFont(42);
	txt.SetTextSize(0.025);
	c->cd(0);
	txt.DrawLatexNDC(0.055, 0.95, "#sqrt{s_{NN}} = 5.02 TeV, PbPb 1.7 nb^{-1}, pp 27.4 pb^{-1}, anti-k_{T} jet (R = 0.4): #font[12]{p}_{T}^{jet} > 120 GeV, |#font[15]{#eta}_{jet}| < 1.6");
	return c;
}
TCanvas*  bjtc_step5_analyzer::fig7(){
	TCanvas * c = new TCanvas("cfig7","", 1800, 1300);
	c->SetMargin(0.18,0.05,0.15,0.15);
	c->Divide(3,2, 0,0);
	gStyle->SetOptStat(0);
	line.SetLineStyle(2);
	TLegend* tl=new TLegend(0.4,0.75,0.95,0.93); tl->SetLineColor(0);
	TLegend* tl2=new TLegend(0.25,0.65,0.85,0.92); tl2->SetLineColor(0);
	for(int i=0; i<3; i++){
		c->cd(3-i);
		TH1* h = js_bjet_Pb_SystError->at(0,i);
		h->SetAxisRange(0.01, 100, "Y");
		h->SetAxisRange(0., .99, "X");
		hist_style_error(h,kBlue+2, i==2);
		h->GetYaxis()->SetTitle("#rho(#Deltar)");
		h->Draw("e2");
		if(i==0) tl->AddEntry(h, "b jets", "pf");
		h = js_bjet_Pb_data->at(0,i);
		hist_style_data(h,kBlue+2, i==2);
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
		h->SetAxisRange(0., 3.5, "Y");
		h->SetAxisRange(0., .99, "X");
		hist_style_error(h,kBlue+2, i==2);
		h->GetYaxis()->SetTitle("#rho(#Deltar)_{PbPb}/#rho(#Deltar)_{pp}");
		h->Draw("e2");
		if(i==2) tl2->AddEntry(h, "b (PbPb)/b (pp)", "pf");
		h = js_ratio_bjet_data->at(0,i);
		hist_style_data(h,kBlue+2, i==2);
		h->Draw("same");
		line.DrawLine(0, 1, 1,1);

		h = js_ratio_incl_systError->at(0,i);
		hist_style_error(h,kRed+2);
		h->Draw("e2same");
		if(i==2) tl2->AddEntry(h, "incl.(PbPb)/incl.(pp)", "pf");
		h = js_ratio_bjet_data->at(0,i);
		h = js_ratio_incl_data->at(0,i);
		hist_style_data(h,kRed+2);
		h->Draw("same");
		line.DrawLine(0, 1, 1,1);

		h = (TH1*) js_bjet_Pb_SystError->at(0,i)->Clone(Form("fig7_b2inclpp_syst_%d",i));
		h->Divide(js_incl_pp_SystError->at(0,i));
		h->SetAxisRange(0., .99, "X");
		hist_style_error(h,kGreen+2, i==2);
		h->GetYaxis()->SetTitle("#rho(#Deltar)_{PbPb}/#rho(#Deltar)_{incl. pp}");
		h->Draw("samee2");
		if(i==2) tl2->AddEntry(h, "b.(PbPb)/incl.(pp)", "pf");
		h = (TH1*) js_bjet_Pb_data->at(0,i)->Clone(Form("fig7_b2inclpp_data_%d",i));
		h->Divide(js_incl_pp_data->at(0,i));
		hist_style_data(h,kGreen+2, i==2);
		h->Draw("same");
		line.DrawLine(0, 1, 1,1);

	}
	c->cd(1);
	cms_caption(0.24,0.93,0.07);
	cent_caption(0.26,0.86,0.057, "Cent:30-90%");
	c->cd(2);
	cent_caption(0.08,0.86,0.06, "Cent:10-30%");
	c->cd(3);
	tl->Draw();
	cent_caption(0.08,0.86,0.06, "Cent:0-10%");
	c->cd(0);
	txt.SetTextFont(42);
	txt.SetTextSize(0.035);
	txt.DrawLatexNDC(0.055, 0.96, "#sqrt{s_{NN}} = 5.02 TeV, PbPb 1.7 nb^{-1}, pp 27.4 pb^{-1}, anti-k_{T} jet (R = 0.4): #font[12]{p}_{T}^{jet} > 120 GeV, |#font[15]{#eta}_{jet}| < 1.6");
	c->cd(4);
	tl2->Draw();

	//bx.SetFillColor(kBlack);
	c->cd(0);
	TBox bx; bx.SetFillColor(kWhite); bx.SetFillStyle(1001);
	bx.DrawBox(0.36, 0.035, 0.39, 0.07);
	bx.DrawBox(0.66, 0.035, 0.69, 0.07);
	bx.DrawBox(0.97, 0.035, 0.99, 0.07);
	txt.SetTextSize(0.036);
	txt.DrawLatex(0.365, 0.041, "1");
	txt.DrawLatex(0.67, 0.041, "1");
	txt.DrawLatex(0.975, 0.041, "1");
	return c;
}
void  bjtc_step5_analyzer::post_check(){
	auto stack_incl = js_incl->stack("stack_incl");
	auto stack_incl_pp = js_incl_pp_data->stack("stack_incl");
	auto stack_b = js_bjet->stack("stack_b");
	TString ptl[] = {"pT > 1", "pT > 2", "pT > 3", "pT > 4", "pT > 8", "pT > 12"};
	TString ctl[] = {"Cent: 0-10%", "Cent: 10-30%", "Cent: 30-90%"};
	for(auto i=0 ;i<js_incl->Nrow(); i++){
		for(auto j=0 ;j<js_incl->Ncol(); j++){
			TString title =	ctl[j]+" "+ptl[i];
			stack_incl->at(i,j)->SetTitle(title);
			stack_incl_pp->at(i,j)->SetTitle(title);
		}
	}
	plot_dr_ratio("xcheck_stack_ratio_incl_vs_bjet", "incl.(pp)", "b jet", stack_incl_pp, stack_b);
	//plot_dr_ratio("xcheck_stack_ratio_incl_vs_bjet", "incl.", "b jet", stack_incl, stack_b);
}

void  bjtc_step5_analyzer::probe_plots(){
	TH1D *hbpp[3][3], *hbppe[3][3];
	TH1D *hipp[3][3], *hippe[3][3];
	TH1D *hbpb[3][3], *hbpbe[3][3];
	TH1D *hipb[3][3], *hipbe[3][3];

	TH1D *hbrPb2pp[3][3], *hbrPb2ppErr[3][3];
	TH1D *hirPb2pp[3][3], *hirPb2ppErr[3][3];
	TH1D *hrb2i[3][3], *hrb2iErr[3][3];
	int ptj[3] = {0, 1, 4};
	for(auto i=0; i<3; i++){
		for(auto j=0; j<3; j++){
			int ptbin = ptj[j];
			// cent, pt
			hbpb[i][j] = (TH1D*) js_bjet->at(ptbin, i)->Clone(Form("js_bjet_ptIntegrated_%d", j));
			hipb [i][j]= (TH1D*) js_incl->at(ptbin,i)->Clone(Form("js_incl_ptIntegral_%d",j));
			hbpbe[i][j] = (TH1D*) js_bjet_err->at(ptbin, i)->Clone(Form("js_ratio_ptIntegrated_err_%d", j));
			hipbe[i][j]= (TH1D*) js_incl_err->at(ptbin,i)->Clone(Form("js_incl_ptIntegral_err_%d",j));
			hipp[i][j]= (TH1D*) js_incl_pp->at(ptbin,0)->Clone(Form("js_pp_incl_ptIntegral_%d",j));
			hippe[i][j]= (TH1D*) js_incl_pp_err->at(ptbin,0)->Clone(Form("js_pp_incl_ptIntegral_err_%d",j));
			hbpp[i][j]= (TH1D*) js_bjet_pp->at(ptbin,0)->Clone(Form("js_pp_bjet_ptIntegral_%d",j));
			hbppe[i][j]= (TH1D*) js_bjet_pp_err->at(ptbin,0)->Clone(Form("js_pp_bjet_ptIntegral_err_%d",j));
			for(auto k=ptbin+1; k<6; k++){
				hbpbe[i][j]->Add(js_bjet_err->at(k, i));	
				hbpb[i][j]->Add(js_bjet->at(k, i));	
				hipb[i][j]->Add(js_incl->at(k, i));	
				hipbe[i][j]->Add(js_incl_err->at(k, i));	
				hippe[i][j]->Add(js_incl_pp_err->at(k, 0));	
				hipp[i][j]->Add(js_incl_pp->at(k, 0));	
				hbppe[i][j]->Add(js_bjet_pp_err->at(k, 0));	
				hbpp[i][j]->Add(js_bjet_pp->at(k, 0));	
			}
			hbrPb2pp[i][j] = (TH1D*) hbpb[i][j]->Clone(Form("hbrP22pp_%d_%d", i,j));
			hbrPb2pp[i][j]->Divide(hbpp[i][j]);
			hbrPb2ppErr[i][j] = (TH1D*) hbpbe[i][j]->Clone(Form("hbrP22pp_%d_%d", i,j));
			hbrPb2ppErr[i][j]->Divide(hbppe[i][j]);
			hirPb2pp[i][j] = (TH1D*) hipb[i][j]->Clone(Form("hirP22pp_%d_%d", i,j));
			hirPb2pp[i][j]->Divide(hipp[i][j]);
			hirPb2ppErr[i][j] = (TH1D*) hipbe[i][j]->Clone(Form("hirP22pp_%d_%d", i,j));
			hirPb2ppErr[i][j]->Divide(hippe[i][j]);
			hrb2i[i][j] = (TH1D*) hbpb[i][j]->Clone(Form("hrb2i_%d_%d", i,j));
			hrb2i[i][j]->Divide(hipb[i][j]);
			hrb2iErr[i][j] = (TH1D*) hbpbe[i][j]->Clone(Form("hrb2iErr_%d_%d", i,j));
			hrb2iErr[i][j]->Divide(hipbe[i][j]);
		}
	}
	TCanvas * c = new TCanvas("cprobe1","", 1800, 1300);
	c->SetMargin(0.18,0.05,0.15,0.15);
	c->Divide(3,2, 0,0);
	gStyle->SetOptStat(0);
	line.SetLineStyle(2);
	TLegend* tl=new TLegend(0.5,0.75,0.95,0.93); tl->SetLineColor(0);
	TLegend* tl2=new TLegend(0.5,0.75,0.95,0.93); tl2->SetLineColor(0);
	for(int i=0; i<3; i++){
		c->cd(3-i);
		TH1* h = hbrPb2pp[i][0];
		h->SetAxisRange(0, 4, "Y");
		h->SetAxisRange(0., .99, "X");
		hist_style_error(h,kBlue+2, i==1);
		h->GetYaxis()->SetTitle("#rho(#Deltar)_{PbPb}/#rho(#Deltar)_{pp}");
		hist_style_data(h,kBlue+2);
		h->Draw("");
		h = hbrPb2pp[i][1];
		hist_style_data(h,kGreen+2);
		h->Draw("same");
		h = hbrPb2pp[i][2];
		hist_style_data(h,kRed+2);
		h->Draw("same");

		h = hbrPb2ppErr[i][0];
		hist_style_error(h,kBlue+2);
		h->Draw("e2same");
		if(i==0) tl->AddEntry(h, "b jets pT>1 GeV", "pf");
		h = hbrPb2ppErr[i][1];
		hist_style_error(h,kGreen+2);
		h->Draw("e2same");
		if(i==0) tl->AddEntry(h, "b jets pT>2 GeV", "pf");
		h = hbrPb2ppErr[i][2];
		hist_style_error(h,kRed+2);
		h->Draw("e2same");
		if(i==0) tl->AddEntry(h, "b jets pT>8 GeV", "pf");
		line.DrawLine(0, 1, 1,1);

		c->cd(6-i);
		h = hirPb2pp[i][0];
		h->SetAxisRange(0, 4, "Y");
		h->SetAxisRange(0., .99, "X");
		hist_style_error(h,kBlue+2, i==1);
		h->GetYaxis()->SetTitle("#rho(#Deltar)_{PbPb}/#rho(#Deltar)_{pp}");
		hist_style_data(h,kBlue+2);
		h->Draw("");
		h = hirPb2pp[i][1];
		hist_style_data(h,kGreen+2);
		h->Draw("same");
		h = hirPb2pp[i][2];
		hist_style_data(h,kRed+2);
		h->Draw("same");

		h = hirPb2ppErr[i][0];
		hist_style_error(h,kBlue+2);
		h->Draw("e2same");
		if(i==0) tl2->AddEntry(h, "incl: pT>1 GeV", "pf");
		h = hirPb2ppErr[i][1];
		hist_style_error(h,kGreen+2);
		h->Draw("e2same");
		if(i==0) tl2->AddEntry(h, "incl: pT>2 GeV", "pf");
		h = hirPb2ppErr[i][2];
		hist_style_error(h,kRed+2);
		h->Draw("e2same");
		if(i==0) tl2->AddEntry(h, "incl: pT>8 GeV", "pf");
		line.DrawLine(0, 1, 1,1);

	}
	c->cd(6);
	tl2->Draw();
	c->cd(1);
	cms_caption(0.24,0.93,0.07);
	cent_caption(0.26,0.86,0.05, "Cent:30-90%");
	c->cd(2);
	cent_caption(0.08,0.86,0.062, "Cent:10-30%");
	c->cd(3);
	tl->Draw();
	cent_caption(0.08,0.86,0.062, "Cent:0-10%");
	txt.SetTextFont(42);
	txt.SetTextSize(0.025);
	c->cd(0);
	txt.DrawLatexNDC(0.055, 0.95, "#sqrt{s_{NN}} = 5.02 TeV, PbPb 1.7 nb^{-1}, pp 27.4 pb^{-1}, anti-k_{T} jet (R = 0.4): #font[12]{p}_{T}^{jet} > 120 GeV, |#font[15]{#eta}_{jet}| < 1.6");
	c->SaveAs(fig_output+"/probe_fig1.pdf");

	TCanvas * c2 = new TCanvas("probe_fig2","", 1600, 700);
	c2->SetMargin(0.18,0.05,0.15,0.1);
	c2->Divide(3,1, 0,0);
	gStyle->SetOptStat(0);
	line.SetLineStyle(2);
	TLegend* tl3=new TLegend(0.5,0.75,0.95,0.93); tl3->SetLineColor(0);
	for(int i=0; i<3; i++){
		c2->cd(3-i);
		TH1* h = hrb2i[i][0];
		h->SetAxisRange(0, 3.9, "Y");
		h->SetAxisRange(0., .99, "X");
		hist_style_error(h,kBlue+2, i==2);
		h->GetYaxis()->SetTitle("#rho(#Deltar)_{b}/#rho(#Deltar)_{incl}");
		h->Draw("");
		h = hrb2i[i][1];
		hist_style_data(h,kGreen+2, i==2);
		h->Draw("same");
		h = hrb2i[i][2];
		hist_style_data(h,kRed+2, i==2);
		h->Draw("same");

		h = hrb2iErr[i][0];
		hist_style_error(h,kBlue+2);
		h->Draw("e2same");
		if(i==0) tl3->AddEntry(h, "PbPb: pT > 1 GeV", "pf");
		h = hrb2iErr[i][1];
		hist_style_error(h,kGreen+2);
		h->Draw("e2same");
		if(i==0) tl3->AddEntry(h, "PbPb: pT > 2 GeV", "pf");
		h = hrb2iErr[i][2];
		hist_style_error(h,kRed+2);
		h->Draw("e2same");
		if(i==0) tl3->AddEntry(h, "PbPb: pT > 8 GeV", "pf");

		line.DrawLine(0, 1, 1,1);
		//gPad->SetLogy();
	}
	c2->cd(1);
	cms_caption(0.24,0.93,0.07);
	cent_caption(0.26,0.86,0.05, "Cent:30-90%");
	c2->cd(2);
	cent_caption(0.08,0.86,0.062, "Cent:10-30%");
	c2->cd(3);
	tl3->Draw();
	cent_caption(0.08,0.86,0.062, "Cent:0-10%");
	caption_pp_pb(c2);
	c2->SaveAs(fig_output+"/probe_fig2.pdf");

}

TCanvas*  bjtc_step5_analyzer::P_overlay(){
	TCanvas * c = new TCanvas("cp1","", 1600, 700);
	c->SetMargin(0.18,0.05,0.15,0.1);
	c->Divide(3,1, 0,0);
	gStyle->SetOptStat(0);
	line.SetLineStyle(2);
	TLegend* tl=new TLegend(0.5,0.75,0.95,0.93); tl->SetLineColor(0);
	for(int i=0; i<3; i++){
		c->cd(3-i);
		TH1* h = (TH1*) p_bjet_pb_data_syst->at(0,i)->Clone(Form("P_ratio_b_pb_syst_%d",i));
		h->Divide(h, p_incl_pb_data_syst->at(0,i), 1, 1);
		h->SetAxisRange(0, 3.9, "Y");
		h->SetAxisRange(0., .99, "X");
		hist_style_error(h,kBlue+2, i==2);
		h->GetYaxis()->SetTitle("P(#Deltar)_{b}/P(#Deltar)_{incl}");
		h->Draw("e2");
		if(i==0) tl->AddEntry(h, "PbPb", "pf");
		h = (TH1*) p_bjet_pb_data->at(0,i)->Clone(Form("P_ratio_b_pb_%d",i));
		h->Divide(h, p_incl_pb_data->at(0,i), 1, 1);
		hist_style_data(h,kBlue+2, i==2);
		h->Draw("same");
		//line.DrawLine(0, 1, 1,1);
		//gPad->SetLogy();

		h = (TH1*) p_bjet_pp_data_syst->at(0,i)->Clone(Form("P_ratio_b_pp_syst_%d",i));
		h->Divide(h, p_incl_pp_data_syst->at(0,i), 1, 1);
		hist_style_error(h,kRed+2);
		h->Draw("e2same");
		if(i==0) tl->AddEntry(h, "pp", "pf");
		h = (TH1*) p_bjet_pp_data->at(0,i)->Clone(Form("P_ratio_b_pp_%d",i));
		h->Divide(h, p_incl_pp_data->at(0,i), 1, 1);
		hist_style_data(h,kRed+2);
		h->Draw("same");
		line.DrawLine(0, 1, 1,1);
		//gPad->SetLogy();
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

TCanvas*  bjtc_step5_analyzer::P_fig1(){
	TCanvas * c = new TCanvas("cp1","", 1600, 700);
	c->SetMargin(0.18,0.05,0.15,0.1);
	c->Divide(3,1, 0,0);
	gStyle->SetOptStat(0);
	line.SetLineStyle(2);
	TLegend* tl=new TLegend(0.5,0.75,0.95,0.93); tl->SetLineColor(0);
	for(int i=0; i<3; i++){
		c->cd(3-i);
		TH1* h = (TH1*) p_bjet_pb_data_syst->at(0,i)->Clone(Form("P_aux_fig1_ratio_b_pb_syst_%d",i));
		h->Divide(h, p_bjet_pp_data_syst->at(0,i), 1, 1);
		h->SetAxisRange(0, 3.9, "Y");
		h->SetAxisRange(0., .99, "X");
		hist_style_error(h,kBlue+2, i==2);
		h->GetYaxis()->SetTitle("P(#Deltar)_{PbPb}/P(#Deltar)_{pp}");
		h->Draw("e2");
		if(i==0) tl->AddEntry(h, "b jet", "pf");
		h = (TH1*) p_bjet_pb_data->at(0,i)->Clone(Form("P_aux_fig1_ratio_b_%d",i));
		h->Divide(h, p_bjet_pp_data->at(0,i), 1, 1);
		hist_style_data(h,kBlue+2, i==2);
		h->Draw("same");
		//line.DrawLine(0, 1, 1,1);
		//gPad->SetLogy();

		h = (TH1*) p_incl_pb_data_syst->at(0,i)->Clone(Form("P_aux_fig1_ratio_incl_pb_syst_%d",i));
		h->Divide(h, p_incl_pp_data_syst->at(0,i), 1, 1);
		hist_style_error(h,kRed+2);
		h->Draw("e2same");
		if(i==0) tl->AddEntry(h, "pp", "pf");
		h = (TH1*) p_incl_pb_data->at(0,i)->Clone(Form("P_aux_fig1_ratio_ratio_incl_%d",i));
		h->Divide(h, p_incl_pp_data->at(0,i), 1, 1);
		hist_style_data(h,kRed+2);
		h->Draw("same");
		line.DrawLine(0, 1, 1,1);
		//gPad->SetLogy();
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

TCanvas*  bjtc_step5_analyzer::P_fig2(){
	TCanvas * c = new TCanvas("cp_fig1","", 1600, 700);
	c->SetMargin(0.18,0.05,0.15,0.1);
	c->Divide(3,1, 0,0);
	gStyle->SetOptStat(0);
	line.SetLineStyle(2);
	TLegend* tl=new TLegend(0.5,0.8,0.95,0.99); tl->SetLineColor(0);
	for(int i=0; i<3; i++){
		c->cd(3-i);
		TH1* h = (TH1*) p_bjet_pb_data_syst->at(0,i)->Clone(Form("P_aux_fig2_ratio_b_pb_syst_%d",i));
		h->Divide(h, p_incl_pp_data_syst->at(0,i), 1, 1);
		h->SetAxisRange(0, 4.9, "Y");
		h->SetAxisRange(0., .99, "X");
		hist_style_error(h,kBlue+2, i==2);
		//h->GetYaxis()->SetTitle("P(#Deltar)_{b}/P(#Deltar)_{incl.}");
		h->GetYaxis()->SetTitle("");
		h->Draw("e2");
		if(i==1) tl->AddEntry(h, "P_{Pb,b}/P_{pp,incl}", "pf");
		h = (TH1*) p_bjet_pb_data->at(0,i)->Clone(Form("P_aux_fig2_ratio_b_pb_%d",i));
		h->Divide(h, p_incl_pp_data->at(0,i), 1, 1);
		hist_style_data(h,kBlue+2, i==2);
		h->Draw("same");
		//line.DrawLine(0, 1, 1,1);
		//gPad->SetLogy();

		h = (TH1*) p_incl_pb_data_syst->at(0,i)->Clone(Form("P_aux_fig2_ratio_incl_pb_syst_%d",i));
		h->Divide(h, p_incl_pp_data_syst->at(0,i), 1, 1);
		hist_style_error(h,kRed+2);
		h->Draw("e2same");
		//if(i==1) tl->AddEntry(h, "pp", "pf");
		if(i==1) tl->AddEntry(h, "P_{Pb,incl}/P_{pp,incl}", "pf");
		h = (TH1*) p_incl_pb_data->at(0,i)->Clone(Form("P_aux_fig2_ratio_b_pp_%d",i));
		h->Divide(h, p_incl_pp_data->at(0,i), 1, 1);
		hist_style_data(h,kRed+2);
		h->Draw("same");
		line.DrawLine(0, 1, 1,1);
		//gPad->SetLogy();
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

TCanvas*  bjtc_step5_analyzer::P_fig3(){
	TCanvas * c = new TCanvas("cp_fig3","", 1600, 700);
	c->SetMargin(0.18,0.05,0.15,0.1);
	c->Divide(3,1, 0,0);
	gStyle->SetOptStat(0);
	line.SetLineStyle(2);
	TLegend* tl=new TLegend(0.4,0.8,0.95,0.99); tl->SetLineColor(0);
	for(int i=0; i<3; i++){
		c->cd(3-i);
		TH1* h = (TH1*) p_bjet_pb_data_syst->at(0,i)->Clone(Form("P_aux_fig3_ratio_pb_syst_%d",i));
		h->Add(h, p_bjet_pp_data_syst->at(0,i), 1, -1);
		h->Divide(h, p_incl_pp_data_syst->at(0,i), 1, 1);
		h->SetAxisRange(-1, 3.9, "Y");
		h->SetAxisRange(0., .99, "X");
		hist_style_error(h,kBlue+2, i==2);
		//h->GetYaxis()->SetTitle("P(#Deltar)_{b}/P(#Deltar)_{incl.}");
		h->GetYaxis()->SetTitle("");
		h->Draw("e2");
		if(i==1) tl->AddEntry(h, "(P_{Pb,b}-P_{pp,b})/P_{pp,incl}", "pf");
		h = (TH1*) p_bjet_pb_data->at(0,i)->Clone(Form("P_aux_fig3_ratio_pb_pb_%d",i));
		h->Add(h, p_bjet_pp_data->at(0,i), 1, -1);
		h->Divide(h, p_incl_pp_data->at(0,i), 1, 1);
		hist_style_data(h,kBlue+2, i==2);
		h->Draw("same");
		//line.DrawLine(0, 1, 1,1);
		//gPad->SetLogy();

		h = (TH1*) p_incl_pb_data_syst->at(0,i)->Clone(Form("P_aux_fig2_ratio_incl_pp_syst_%d",i));
		h->Add(h, p_incl_pp_data_syst->at(0,i), 1, -1);
		h->Divide(h, p_incl_pp_data_syst->at(0,i), 1, 1);
		hist_style_error(h,kRed+2);
		h->Draw("e2same");
		//if(i==1) tl->AddEntry(h, "pp", "pf");
		if(i==1) tl->AddEntry(h, "(P_{Pb,incl}-P_{pp,incl})/P_{pp,incl}", "pf");
		h = (TH1*) p_incl_pb_data->at(0,i)->Clone(Form("P_aux_fig2_ratio_b_pp_%d",i));
		h->Add(h, p_incl_pp_data->at(0,i), 1, -1);
		h->Divide(h, p_incl_pp_data->at(0,i), 1, 1);
		hist_style_data(h,kRed+2);
		h->Draw("same");
		line.DrawLine(0, 0, 1,0);
		//gPad->SetLogy();
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

TCanvas*  bjtc_step5_analyzer::P_diff_bjet(){
	TCanvas * c = new TCanvas("cp1","", 1600, 700);
	c->SetMargin(0.18,0.05,0.15,0.1);
	c->Divide(3,1, 0,0);
	gStyle->SetOptStat(0);
	line.SetLineStyle(2);
	TLegend* tl=new TLegend(0.5,0.75,0.95,0.93); tl->SetLineColor(0);
	for(int i=0; i<3; i++){
		c->cd(3-i);
		TH1* h =(TH1*) p_bjet_pb_data_syst->at(0,i)->Clone(Form("diff_syst_%d",i));
		//h->Add(h, p_bjet_pp_data_syst->at(0,i), 1, -1);
		h->Divide(h, p_incl_pp_data_syst->at(0,i), 1, 1);
		//h->Divide(h, p_bjet_pp_data_syst->at(0,i), 1, 1);
		cout<<h->GetName()<<endl;
		h->SetAxisRange(0, 4.9, "Y");
		h->SetAxisRange(0., .99, "X");
		hist_style_error(h,kBlue+2, i==2);
		h->GetYaxis()->SetTitle("P(#Deltar)_{Pb}/P(#Deltar)_{pp}-1");
		//h->GetYaxis()->SetTitle("P(#Deltar)_{Pb}-P(#Deltar)_{pp}");
		h->Draw("e2");
		if(i==0) tl->AddEntry(h, "b jet", "pf");

		h =(TH1*) p_incl_pb_data_syst->at(0,i)->Clone(Form("diff_incl_syst_%d",i));
		//h->Add(h, p_incl_pp_data_syst->at(0,i), 1, -1);
		h->Divide(h, p_incl_pp_data_syst->at(0,i), 1, 1);
		hist_style_error(h,kRed+2, i==2);
		h->Draw("samee2");
		if(i==0) tl->AddEntry(h, "incl. jet", "pf");

		h =(TH1*) p_bjet_pb_data->at(0,i)->Clone(Form("diff_%d",i));
		//h->Add(h, p_bjet_pp_data->at(0,i), 1, -1);
		h->Divide(h, p_incl_pp_data->at(0,i), 1, 1);
		//h->Divide(h, p_bjet_pp_data->at(0,i), 1, 1);
		hist_style_data(h,kBlue+2, i==2);
		h->Draw("same");

		h =(TH1*) p_incl_pb_data->at(0,i)->Clone(Form("diff_incl_%d",i));
		//h->Add(h, p_incl_pp_data->at(0,i), 1, -1);
		h->Divide(h, p_incl_pp_data->at(0,i), 1, 1);
		hist_style_data(h,kRed+2, i==2);
		h->Draw("same");
		line.DrawLine(0, 0, 1,0);
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
	cent_caption(0.26,0.87,0.053, "Cent:30-90%");
	c->cd(2);
	cent_caption(0.08,0.86,0.062, "Cent:0-30%");
	c->cd(3);
	tl->Draw();
	cent_caption(0.08,0.86,0.062, "Cent:0-30%");
	caption_pp_pb(c);
	return c;
}

void bjtc_step5_analyzer::syst_error_breakdown_plot(){
	//js_bjet_err = new jtcTH1Player("js_bjet/js_bjet_data_bkgError_*_*",base->npt, base->ncent);
	//js_bjet_err->autoLoad(pbfile);
	auto js_incl_error = js_incl_err2;
	auto js_bjet_error = js_bjet_err2;
	int size_syst_b = syst_b.size();
	jtcTH1Player* jsbjet[size_syst_b+1];
	jsbjet[0] =(jtcTH1Player*) js_bjet_error->contractX("js_bjet_Pb_syst_bkg");

	auto c = new plotManager();
	c->initSquarePad("canvas_breakdown", "", 1,3);
	auto it = syst_b_dict.begin();
	for(int i=1; i<	size_syst_b+1; ++i){
		//jsbjet[i] =(jtcTH1Player*) jsbjet[0]->clone("js_bjet_Pb_syst_"+systemError_bjet_item[i]);
		jsbjet[i] =(jtcTH1Player*) jsbjet[0]->clone("js_bjet_Pb_syst_"+syst_b[i-1].first);
		float scale = syst_b[i-1].second;
		//		jsbjet[i]->absError(0);
		for(int j=0; j<base->ncent; j++){
			auto h = jsbjet[i]->at(0,j);
			for(int k=1; k<h->GetNbinsX()+1; k++){
				float c = h->GetBinContent(k);
				h->SetBinContent(k, c*scale);
				h->SetBinError(k,0);
			}
		}
	}
	for(int j=0; j<base->ncent; j++){
			auto h = jsbjet[0]->at(0,j);
			for(int k=1; k<h->GetNbinsX()+1; k++){
				float e = h->GetBinError(k);
				h->SetBinContent(k, e);
				h->SetBinError(k,0);
			}
		}

	for(int i=0; i<	size_syst_b+1; ++i){
		for(int j=0; j< 3; j++){
			auto h = jsbjet[i]->at(i,j);
			h->SetLineWidth(2);
		}
	}

	for(int i=1; i<	size_syst_b+1; ++i){
		jsbjet[i]->at(0,0)->SetTitle("Systematic uncertainty: Cent:0-30%");
		jsbjet[i]->at(0,1)->SetTitle("Systematic uncertainty: Cent:30-90%");
		jsbjet[i]->at(0,0)->GetYaxis()->SetTitle("Relative uncertainty");
		jsbjet[i]->at(0,1)->GetYaxis()->SetTitle("Relative uncertainty");
		c->addm2TH1(jsbjet[i], systemError_bjet_item[i-1],"l");
	}
	c->addm2TH1(jsbjet[0], "Bkg", "l");
//	c->addm2TH1(jsbjet[size_syst_b+1], "taging Bias","l");

	c->setXrange(0,0.99);
	c->doLogy=1;
//	c->setYrange(-0.01,0.3);
	c->draw();
	c->drawLegend(0.3, 0.5, 0.7, 0.85);
	c->save(fig_output+"/system_breakdown_plot.pdf");

}

void bjtc_step5_analyzer::syst_error_breakdown(){
	//overlay the error by sources
	int size_syst_b = syst_b_dict.size();
	jtcTH1Player* jsbjet[size_syst_b+1];
	jsbjet[0] =(jtcTH1Player*) js_bjet_err2->contractX("js_bjet_Pb_syst_bkg");

	auto c = new plotManager();
	c->initSquarePad("canvas_splitting", "", 1,3);
	auto it = syst_b_dict.begin();
	for(int i=0; i<	syst_b_dict.size()+1; ++i){
		if(i>0){
			jsbjet[i] =(jtcTH1Player*) js_bjet_err2->clone("js_bjet_Pb_syst_"+it->first);
			jsbjet[i]->mergeError(it->second);
		}
		jsbjet[i]->at(0,0)->SetTitle("Systematic uncertainty: Cent:0-10%");
		jsbjet[i]->at(0,1)->SetTitle("Systematic uncertainty: Cent:10-30%");
		jsbjet[i]->at(0,2)->SetTitle("Systematic uncertainty: Cent:30-90%");
		jsbjet[i]->at(0,0)->GetYaxis()->SetTitle("Absolute uncertainty");
		jsbjet[i]->at(0,1)->GetYaxis()->SetTitle("Absolute uncertainty");
		jsbjet[i]->at(0,2)->GetYaxis()->SetTitle("Absolute uncertainty");
		for(int j=0; j< 3; j++){
			auto h = jsbjet[i]->at(0,j);
			h->SetLineWidth(2);
			for(int k=1; k<h->GetNbinsX()+1; k++){
				h->SetBinContent(k,h->GetBinError(k));
				h->SetBinError(k, 0);
			}
		}
		//cout<<it->first<<endl;
		if(i>0) {
			c->addm2TH1(jsbjet[i], it->first,"l");
			it++;
		}else {
			c->addm2TH1(jsbjet[i], "BKG+ME","l");
		}
	}


	c->setXrange(0,0.99);
	c->setYrange(0.005,1500);
	c->doLogy = 1;
	c->draw();

	c->drawLegend(0.6, 0.5, 0.9, 0.85);
	c->save(fig_output+"/system_breakdown.pdf");
}

void bjtc_step5_analyzer::plot_dr_ratio(TString cname,  TString l1, TString l2, jtcTH1Player* j1, jtcTH1Player* j2){
	auto c = new plotManager();
	c->initOverlayPad("c_"+cname, "", j1->Nrow(), j1->Ncol());
	c->setXrange(0,0.99);
	c->addm2TH1(j1, l1);
	c->addm2TH1(j2, l2);
	c->setRatioYrange(0,2);
	c->draw();
	c->drawLegend();
	c->save(fig_output+"/"+cname+".pdf");
}
/*
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
   */
void bjtc_step5_analyzer::set_bin_err(TH1* h){
	for(int k=0; k<h->GetNbinsX()+1; k++){
		auto e  = h->GetBinError(k);
		h->SetBinContent(k, e);
		h->SetBinError(k, 0);
	}
}

void bjtc_step5_analyzer::analyze(){
	preprocess("step4_output_sm.root", "step5_output_sm.root");
	//preprocess("step4_output_nominal.root", "step5_output_nominal.root");
	//preprocess("step4_output_nominal.root", "step5_output_nominal.root");
	auto cfig1 = fig1();
	cfig1->SaveAs(fig_output+"/figure_nominal_js_overlay.pdf");
	//auto cfig2 = fig2();
	//cfig2->SaveAs(fig_output+"/figure_nominal_js_ratio_b2Inclusive.pdf");
	//auto cfig3 = fig3();
	//cfig3->SaveAs(fig_output+"/figure_nominal_js_inclusive.pdf");
	//auto cfig4 = fig4();
	//cfig4->SaveAs(fig_output+"/figure_nominal_js_bjets.pdf");
	//auto cfig5 = fig5();
	//cfig5->SaveAs(fig_output+"/figure_nominal_js_ratio_overlay.pdf");
	//auto cfig6 = fig6();
	//cfig6->SaveAs(fig_output+"/figure_nominal_js_bjet_vs_incl.pdf");
	auto cfig7 = fig7();
	cfig7->SaveAs(fig_output+"/figure_nominal_fig1.pdf");
	auto cfig2 = fig2();
	cfig2->SaveAs(fig_output+"/figure_nominal_fig2.pdf");

	////auto pfig2 = P_diff_bjet();
	////pfig2->SaveAs(fig_output+"/figure_P_bjet_diff.pdf");
	////auto pfig1 = P_overlay();
	////pfig1->SaveAs(fig_output+"/figure_P_bjet_overlay.pdf");
	//auto pafig1 = P_fig1();
	//pafig1->SaveAs(fig_output+"/figure_P_aux_fig1.pdf");
	//auto pafig2 = P_fig2();
	//pafig2->SaveAs(fig_output+"/figure_P_aux_fig2.pdf");
	//auto pafig3 = P_fig3();
	//pafig3->SaveAs(fig_output+"/figure_P_aux_fig3.pdf");
	//probe_plots();
	/*
	   auto cfig6 = fig6();
	   cfig6->SaveAs(fig_output+"/figure_nominal_js_bjet_vs_incl.pdf");
	   */
	//post_check();	
	//syst_error_breakdown();
	syst_error_breakdown_plot();
}	
