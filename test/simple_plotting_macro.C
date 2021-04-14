
void config_err(TH1D* h, Color_t color){
        h->SetLineColor(kWhite);
        h->SetMarkerColor(color);
        h->SetFillColorAlpha(color, .5);
        h->SetFillStyle(1001);
}
void config2(TH1D* h, TH1D* err, Color_t color,Color_t color2, bool dash = 0){
        combineError(h, err);
        h->SetLineColor(color);
        h->SetMarkerSize(0);
        err->SetMarkerSize(0);
        h->SetFillColor(0);
        err->SetFillColorAlpha(color2, .5);
        err->SetLineColor(color);
        err->SetFillStyle(1001);
        h  ->SetLineWidth(1);
        err->SetLineWidth(1);
        if(dash){
//              err->SetFillColorAlpha(color2, .5);
                h  ->SetLineStyle(2);
                err->SetLineStyle(2);
        h  ->SetLineWidth(1);
        err->SetLineWidth(1);
        }
}
void config_mc(TH1D* h, TH1D* err, Color_t color,Color_t color2, bool dash = 0){
        for(int i=1; i<h->GetNbinsX()+1;i++){
                float e1 = err->GetBinError(i);
                float e2 = h->GetBinError(i);
                float e = pow(e1*e1+e2*e2,0.5);
                err->SetBinError(i,e);
        }
		config2(h,err,color,color2,dash);
}

void simple_plotting_macro(){

		

		auto c2 = new TCanvas("c2", "", 510, 510);
		ratio_data->SetLineColor  (kRed+2);
		ratio_data->SetMarkerColor(kRed+2);
		config_err(ratio_data_err, kRed+2);
		//auto ge_p6_r = makeGraph(ratio_mc, ratio_mc_err, kAzure+2, kAzure+2, 1);
		//combineError(ratio_mc_err, ratio_mc);
		config_mc(ratio_mc, ratio_mc_err, kAzure+3, kAzure+2);
		config_mc(js_ratio_p8_all, js_ratio_p8_err_all, kGreen+3,kGreen+2, 1);
		config_mc(ratio_herwig, ratio_herwig_err, kViolet+4,kViolet-2);

		ratio_mc->SetAxisRange(.5, 2.49, "Y");
		ratio_mc->SetAxisRange(0, .99, "X");
		if(drawP){
				ratio_data->GetYaxis()->SetTitle("#rho(#Delta#font[12]{r})_{b}/#rho(#Delta#font[12]{r})_{incl}");
				config_frame(ratio_mc, "#rho(#Delta#font[12]{r})_{b}/#rho(#Delta)_{incl}");
		}
		else{
				//              ratio_data->GetYaxis()->SetTitleFont(40);
				ratio_data->GetYaxis()->SetTitle("#font[40]{rho(#Delta#font[12]{r})_{b}/#rho(#Delta#font[12]{r})_{incl}}");
				config_frame(ratio_mc, "#rho(#Delta#font[12]{r})_{b}/#rho(#Delta#font[12]{r})_{incl}");
		}
		ratio_mc->Draw("hist same");
		ratio_mc_err->Draw("e2 same");

		js_ratio_p8_all->Draw("hist same");
		js_ratio_p8_err_all->Draw("e2 same");
		ratio_herwig->Draw("hist same");
		ratio_herwig_err->Draw("e2 same");
		ratio_data->Draw("same");
		ratio_data_err->Draw("e2 same");
		tl->Draw();
}
