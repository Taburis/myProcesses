
#include "jtcTH1Player.h"
#include "TGraphErrors.h"

void combineError(TH1D* h , TH1D* h2){
		for(int i=1; i<h->GetNbinsX()+1; ++i){
				h->SetBinError(i, pow(pow(h->GetBinError(i),2)+pow(h2->GetBinError(i),2), 0.5));
		}
}

TGraphErrors* makeGraph(TH1D* h, TH1D* h2, Color_t linec, Color_t color, bool isline = 0){
		combineError(h,h2);
		int n = h->GetXaxis()->FindBin(1);
		h->SetLineWidth(2);
		//		auto n = h->GetNbinsX();
		float * x = new float[n];
		float * y = new float[n];
		float *ey = new float[n];
		float *ex = new float[n];
		for(int i=0; i<n-1; ++i){
				float error = h->GetBinError(i+1);
				x[i]=h->GetXaxis()->GetBinCenter(i+1);
				y[i]=h->GetBinContent(i+1);
				ex [i]=0.;
				ey [i]=error;
		}
		x[n-1] = 1;
		y[n-1]=(h->GetBinContent(n-1)+h->GetBinContent(n))/2;
		if( isline) y[n-1]=h->GetBinContent(n-1);
		ex[n-1]=0;
		ey[n-1] = h->GetBinError(n);
		auto g = new TGraphErrors(n, x, y, ex, ey);
		g->SetFillStyle(1001);
		g->SetLineColor(linec);
		g->SetFillColorAlpha(color, 0.5);
		return g;
}

void config_frame(TH1D* h, TString Ytitle){
		h->GetYaxis()->SetTitle(Ytitle);
		h->GetYaxis()->SetTitleOffset(1.4);
		h->GetYaxis()->CenterTitle();
		h->GetXaxis()->CenterTitle();
		h->GetXaxis()->SetLabelSize(0.07);
		h->GetXaxis()->SetTitleSize(0.07);
		h->GetYaxis()->SetLabelSize(0.06);
		h->GetYaxis()->SetTitleSize(0.07);
		h->GetYaxis()->SetTitleOffset(1.3);
}

void config_err(TH1D* h, Color_t color){
		h->SetLineColor(kWhite);
		h->SetMarkerColor(color);
		h->SetFillColorAlpha(color, .5);
		h->SetFillStyle(1001);
}
void config2(TH1D* h, TH1D* err, Color_t color, bool dash = 0){
		combineError(h, err);
		h->SetLineColor(color);
		h->SetMarkerSize(0);
		err->SetMarkerSize(0);
		h->SetFillColor(0);
		err->SetFillColorAlpha(color, .5);
		err->SetLineColor(color);
		err->SetFillStyle(1001);
		h  ->SetLineWidth(1);
		err->SetLineWidth(1);
		if(dash){
				h->SetLineStyle(7);
				err->SetLineStyle(7);
		}
}

void newDrawJS_forSup(){
		bool drawP = 0;
		TString figDumpPath = "./";
		const int n_new_drbin = 7;
		TString track_label[] = {"1 < p_{T}^{trk} < 2 GeV",
				"2 < p_{T}^{trk} < 3 GeV", "3 < p_{T}^{trk} < 4 GeV",
				"4 < p_{T}^{trk} < 8 GeV", "8 < p_{T}^{trk} < 12 GeV",
				"p_{T}^{trk} > 12 GeV"};
		Double_t newdrbin[n_new_drbin+1] = {0.,0.05,0.1,0.15,0.25,0.35,0.6,1.};
		TString finalPath = "/Users/tabris/frameLite/hin18020/plots/";
		auto dataf = TFile::Open(finalPath+ "AN17337Result_new.root");
		auto f8 = TFile::Open("/Users/tabris/frameLite/test/Pythia8_pp_bjet2017_reference.root");
		//auto f8 = TFile::Open("PYTHIA8_reference2.root");
		auto f8Jussi = TFile::Open("/Users/tabris/frameLite/test/Pythia8_pp_Jussi_reference.root");
		TH1D* js_b_p8_all = (TH1D*) f8->Get("dr_bjtc_jetShape_all_0_0");
		TH1D* js_b_p8_err_all = (TH1D*) f8->Get("dr_bjtc_jetShape_all_systError_0_0");
		TH1D* js_incl_p8_err_all = (TH1D*)f8Jussi->Get("dr_incl_jetShape_all_systError_0_0");
		TH1D* js_incl_p8_all = (TH1D*) f8Jussi->Get("dr_incl_jetShape_all_0_0");
		int n0 = js_b_p8_all->FindBin(1);
		js_b_p8_all->Scale(1.0/js_b_p8_all->Integral(1, n0, "w"));
		js_b_p8_err_all->Scale(1.0/js_b_p8_err_all->Integral(1, n0, "w"));
		js_incl_p8_all->Scale(1.0/js_incl_p8_all->Integral(1, n0, "w"));
		js_incl_p8_err_all->Scale(1.0/js_incl_p8_err_all->Integral(1, n0, "w"));
		auto js_ratio_p8_all = (TH1D*) js_b_p8_all->Clone("ratio_Pythia8_JS");
		auto js_ratio_p8_err_all = (TH1D*) js_b_p8_err_all->Clone("ratio_Pythia8_JS_systError");
		js_ratio_p8_all->Divide(js_incl_p8_all);
		js_ratio_p8_err_all->Divide(js_incl_p8_err_all);


		TString savename = "bJTC_JS_WTA_P8_";//fig1.pdf";
		//	auto inclf = TFile::Open(finalPath+ "incl_pp_referenceForbJTC.root");
		const int nPt = 6;
		TH1D** js_b = new TH1D*[nPt];
		TH1D** js_b_err = new TH1D*[nPt];
		TH1D** js_incl_err = new TH1D*[nPt];
		TH1D** js_incl = new TH1D*[nPt];
		TH1D** js_leg = new TH1D*[nPt];// for legend only

		auto j2_incl = new jtcTH1Player("dr_signal_dijtc_jetShape_step3_*_*");
		auto j2_incl_err = new jtcTH1Player("dr_dijtc_jetShape_systError_*_*");
		auto j2_b = new jtcTH1Player("dr_signal_bjtc_jetShape_step3_*_*");
		auto j2_b_err = new jtcTH1Player("dr_bjtc_jetShape_systError_*_*");
		j2_incl->autoLoad(dataf);
		j2_incl_err->autoLoad(dataf);
		j2_b->autoLoad(dataf);
		j2_b_err->autoLoad(dataf);

		TH1D* ratio_data = (TH1D*) dataf->Get("division_dr_data_bjtc_js_allpt_0_0");
		TH1D* ratio_data_err = (TH1D*) dataf->Get("division_dr_data_bjtc_allpt_syt_err_0_0");
		TH1D* ratio_mc = (TH1D*) dataf->Get("division_dr_mc_gsp_unweighted_bjtc_syst_allpt_0_0");
		TH1D* ratio_mc_err = (TH1D*) dataf->Get("division_dr_mc_gsp_unweighted_bjtc_allpt_syst_err_0_0");

		auto j2_p6_incl = (TH1D*) dataf->Get("dr_mc_incl_js_allpt_0_0");
		auto j2_p6_incl_err = (TH1D*) dataf->Get("dr_mc_incl_allpt_syst_err_0_0");
		auto j2_p6_b = (TH1D*) dataf->Get("dr_mc_gsp_unweighted_bjtc_syst_allpt_0_0");
		auto j2_p6_b_err = (TH1D*) dataf->Get("dr_mc_gsp_unweighted_bjtc_allpt_syst_err_0_0");

		auto j2_incl_all = (TH1D*) dataf->Get("dr_data_incl_js_allpt_0_0");
		auto j2_b_all = (TH1D*) dataf->Get("dr_data_bjtc_js_allpt_0_0");
		auto j2_incl_all_err = (TH1D*) dataf->Get("dr_data_incl_allpt_syst_err_0_0");
		auto j2_b_all_err = (TH1D*) dataf->Get("dr_data_bjtc_allpt_syt_err_0_0");


		int nn0 = j2_b_all->FindBin(1);
		float s_b_all = 1.0/j2_b_all->Integral(1, nn0, "w");
		float s_incl_all = 1.0/j2_incl_all->Integral(1, nn0, "w");
		float s_b_mc = 1.0/j2_p6_b->Integral(1, nn0 ,"w" );
		float s_incl_mc = 1.0/j2_p6_incl->Integral(1, nn0, "w");
		if(!drawP){
				j2_b_all->Scale(s_b_all);
				j2_incl_all->Scale(s_b_all);
				j2_p6_b->Scale(s_b_all);
				j2_p6_incl->Scale(s_incl_all);
				ratio_data->Scale(s_b_all/s_incl_all);

				ratio_mc->Scale(s_b_mc/s_incl_mc);
				ratio_data_err->Scale(s_b_all/s_incl_all);
				ratio_mc_err->Scale(s_b_mc/s_incl_mc);
				j2_b_all_err->Scale(s_b_all);
				j2_incl_all_err->Scale(s_b_all);
				j2_p6_b_err->Scale(s_b_all);
				j2_p6_incl_err->Scale(s_incl_all);
		}

		for(auto i=0; i<nPt; ++i){
				js_b[i] = (TH1D*) j2_b->at(i, 0);
				js_b_err[i]= (TH1D*) j2_b_err->at(i,0);
				js_incl[i] = (TH1D*) j2_incl->at(i, 0);
				js_incl_err[i]= (TH1D*) j2_incl_err->at(i,0);
				js_b[i]->SetLineColor(kWhite);
				js_incl[i]->SetLineColor(kWhite);
				js_incl[i]->Scale(s_incl_all);
				js_b[i]->Scale(s_b_all);
				js_incl_err[i]->Scale(s_incl_all);
				js_b_err[i]->Scale(s_b_all);
		}
		//quick config
		float y1min = 0.001, y1max=2;
		float y2min = 0.5, y2max=1.9;
		//sum them to get the total 
		TH1D* js_tot[2], *js_err_tot[2];
		js_tot[0]=(TH1D*) js_incl[0]->Clone("js_incl_tot");
		js_tot[1]=(TH1D*) js_b[0]->Clone("js_b_tot");
		js_err_tot[0]=(TH1D*) js_incl_err[0]->Clone("js_incl_err_tot");
		js_err_tot[1]=(TH1D*) js_b_err[0]->Clone("js_b_err_tot");

		TH1D* js_p6, *js_p6_err;
		js_p6 = (TH1D*) dataf->Get("division_dr_mc_gsp_unweighted_bjtc_syst_allpt_0_0");
		js_p6_err = (TH1D*) dataf->Get("division_dr_mc_gsp_unweighted_bjtc_allpt_syst_err_0_0");

		js_p6->SetMarkerStyle(20);
		js_p6->SetMarkerSize(1);
		js_p6->SetMarkerColor(kAzure+2);
		js_p6->SetLineColor(kAzure+3);
		js_p6_err->SetMarkerStyle(20);
		js_p6_err->SetMarkerSize(1);
		js_p6_err->SetMarkerColor(kAzure+2);
		js_p6_err->SetLineColor(kWhite);
		js_p6_err->SetFillStyle(1001);
		js_p6_err->SetFillColorAlpha(kAzure+2, 0.5);

		for(int i=1; i<6; ++i){
				js_tot[0]->Add(js_incl[i]);
				js_tot[1]->Add(js_b[i]);
				js_err_tot[0]->Add(js_incl_err[i]);
				js_err_tot[1]->Add(js_b_err[i]);
		}
		for(int j=0; j<2; ++j){
				js_tot[j]->SetMarkerStyle(20);
				js_tot[j]->SetMarkerSize(1);
				js_tot[j]->SetMarkerColor(kBlack);
				js_err_tot[j]->SetMarkerStyle(20);
				js_err_tot[j]->SetMarkerSize(.8);
				js_err_tot[j]->SetMarkerColor(kBlack);
				js_err_tot[j]->SetFillStyle(1001);
				js_err_tot[j]->SetFillColorAlpha(kGray+3, 0.4);
		}
		TH1D* ratio[3][2];
		TH1D* ratio_err[3][2];


		int bins[3] = {0, 1, 3};
		Color_t color[3] = {kBlack, kOrange+1, kGreen+3};
		for(int i=0; i<3; ++i){
				ratio[i][0]=(TH1D*) js_b[bins[i]]->Clone(Form("ratio_%d",i));
				ratio[i][1]=(TH1D*) js_incl[bins[i]]->Clone(Form("ratio2_%d",i));
				ratio_err[i][0]=(TH1D*) js_b[bins[i]]->Clone(Form("ratio_err_%d",i));
				ratio_err[i][1]=(TH1D*) js_incl[bins[i]]->Clone(Form("ratio2_err_%d",i));
				for(int j=bins[i]+1; j<6; ++j){
						ratio[i][0]->Add(js_b[j]);
						ratio[i][1]->Add(js_incl[j]);
						ratio_err[i][0]->Add(js_b_err[j]);
						ratio_err[i][1]->Add(js_incl_err[j]);
				}
				ratio_err[i][0]->Divide(ratio_err[i][1]);
				ratio[i][0]->Divide(ratio[i][1]);
				ratio_err[i][0]->SetMarkerColor(color[i]);
				ratio_err[i][0]->SetLineColor(color[i]);
				ratio_err[i][0]->SetMarkerStyle(20);
				ratio_err[i][0]->SetMarkerSize(1);
				ratio_err[i][0]->SetFillStyle(1001);
				ratio_err[i][0]->SetFillColorAlpha(kGray+3, .4);
				ratio_err[i][0]->SetLineColor(kWhite);
				ratio[i][0]->SetMarkerColor(color[i]);
				ratio[i][0]->SetLineColor(color[i]);
				ratio[i][0]->SetMarkerStyle(20);
				ratio[i][0]->SetMarkerSize(1.);
				ratio[i][0]->SetAxisRange(y2min, y2max, "y");
				ratio[i][0]->SetTitle("");
				ratio[i][0]->SetStats(0);
				ratio[i][0]->GetXaxis()->CenterTitle();
				ratio[i][0]->GetXaxis()->SetTitleSize(0.08);
				ratio[i][0]->GetXaxis()->SetLabelSize(0.07);
				ratio[i][0]->GetXaxis()->SetNdivisions(505);
				ratio[i][0]->SetAxisRange(0, .99, "X");
				ratio[i][1]->SetAxisRange(0, .99, "X");
		}


		gStyle->SetPadLeftMargin(0.18);
        gStyle->SetPadRightMargin(0.01);
        gStyle->SetPadTopMargin(1);
        gStyle->SetPadBottomMargin(0.15);
		
		auto c2 = new TCanvas("c2", "", 510, 510);
		c2->cd();
		config2(js_incl_p8_all, js_incl_p8_err_all, kGreen+3, 1);
		config2(j2_p6_incl, j2_p6_incl_err, kAzure+2);
		j2_incl_all->SetLineColor  (kRed+2);
		j2_incl_all->SetMarkerColor(kRed+2);
		config_err(j2_incl_all_err, kRed+2);
//		gPad->SetLogy();
		if(!drawP) j2_incl_all->SetAxisRange(0.0005, .99, "Y");
		else j2_incl_all->SetAxisRange(0.5, 3000, "Y");
		j2_incl_all->SetAxisRange(0, .99, "X");
		config_frame(j2_incl_all, "#rho(#Deltar)");

		TLatex *tx = new TLatex();
		tx->SetTextSize(0.05);
		tx->SetTextFont(62);
		TLegend *tl = new TLegend(0.23, 0.64, 0.55, 0.78); tl->SetLineColor(kWhite);
		tl->AddEntry(j2_incl_all_err, "Data", "pf");
		tl->AddEntry(j2_p6_incl_err, "PYTHIA 6", "fl");
//		tl->AddEntry(js_incl_p8_err_all , "PYTHIA 8", "fl");

		j2_b_all->SetLineColor  (kRed+2);
		j2_b_all->SetMarkerColor(kRed+2);
		config_err(j2_b_all_err, kRed+2);
		//auto ge_p6_b = makeGraph(j2_p6_b, j2_p6_b_err, kAzure+2, kAzure+2);
		config2(js_b_p8_all, js_b_p8_err_all, kGreen+3, 1);
		config2(j2_p6_b, j2_p6_b_err, kAzure+2);
		//else	j2_b_all->SetAxisRange(-100, 1400, "Y");
		if(!drawP) j2_b_all->SetAxisRange(.0005, 1, "Y");
		else j2_b_all->SetAxisRange(.5, 3000, "Y");
		j2_b_all->SetAxisRange(0, .99, "X");
		config_frame(j2_b_all, "#rho(#Deltar)");
		ratio_data->SetLineColor  (kRed+2);
		ratio_data->SetMarkerColor(kRed+2);
		config_err(ratio_data_err, kRed+2);
		config2(ratio_mc, ratio_mc_err, kAzure+2);
		config2(js_ratio_p8_all, js_ratio_p8_err_all, kGreen+3, 1);

		ratio_data->SetAxisRange(.5, 2.49, "Y");
		ratio_data->SetAxisRange(0, .99, "X");
		if(drawP){
				ratio_data->GetYaxis()->SetTitle("#rho(#Deltar)_{b}/#rho(#Delta)_{incl}");
				config_frame(ratio_data, "#rho(#Deltar)_{b}/#rho(#Delta)_{incl}");
		}
		else{
				ratio_data->GetYaxis()->SetTitle("#rho(#Deltar)_{b}/#rho(#Delta)_{incl}");
				config_frame(ratio_data, "#rho(#Deltar)_{b}/#rho(#Delta)_{incl}");
		}
		ratio_data->Draw("same");
		ratio_data_err->Draw("e2 same");
		ratio_mc->Draw("hist same");
		ratio_mc_err->Draw("e2 same");

		tl->Draw();

		TLine l;
		l.DrawLine(0, 1, 1, 1);
		tx->DrawLatexNDC(0.26, 0.84, "b jets/inclusive jets");

		tx->SetTextFont(42);

		c2->cd(1);
		tx->SetTextFont(62);
		tx->SetTextSize(0.07);
		tx->DrawLatexNDC(0.0001, 0.93, "CMS");
		tx->SetTextSize(0.06);
		tx->SetTextFont(52);
		tx->DrawLatexNDC(0.16, 0.93, "Preliminary");
		tx->SetTextSize(0.045);
		tx->SetTextFont(42);
		tx->DrawLatexNDC(0.5, 0.93, "pp 27.4 pb^{-1} (5.02 TeV)");

		tx->SetTextSize(0.045);
		float xp = 0.52, yp = 0.62, yd = 0.07;
		tx->DrawLatexNDC(xp, yp+2*yd, "anti-k_{T} PF jet (#DeltaR=0.4)");
		tx->DrawLatexNDC(xp, yp+1*yd, "p_{T}^{jet} > 120 GeV, |#eta_{jet}| < 1.6");
		tx->DrawLatexNDC(xp, yp, "p_{T}^{trk} > 1 GeV");
		c2->SaveAs(figDumpPath+savename+"sup.pdf");
}
