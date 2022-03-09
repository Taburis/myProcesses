
#include "myProcesses/liteFrame/plugin/plots/plotManager.h"
#include "myProcesses/jtc/plugin/jtcTH1Player.h"

void spectra(){
		auto fpp = TFile::Open("../data/HIN18020/bJTC_pp_data_5TeV_wtaAxis_8May19.root");
		auto fpb= TFile::Open("../data/correlation/newbin/bjtc_data_hardprobe_jet80or100_60mixing.root");
		auto hptpb = new jtcTH1Player("jetPt_Pb_*_*", 1, 3);
		auto hratio = new jtcTH1Player("ratio_*_*", 1, 3);
		auto hptpp = new jtcTH1Player("jetPt_pp_*_*", 1, 3);
		for (auto i=0; i<3; i++){
				auto h = (TH1D*) fpb->Get(Form("jetQASets/incl_RecoLevel_pt_C%d",i))->Clone(Form("incl_Pb_C%d",i));
				h->Scale(1.0/h->Integral());
				TString title = h->GetTitle();
				hptpb->add(h, 0,i);
				auto h2= (TH1D*) fpp->Get("inclJet_corrpt_0")->Clone(Form("incl_pp_C%d", i));
				h2->Scale(1.0/h2->Integral());
				h2->SetTitle(title);
				hptpp->add(h2,0,i);
				auto hr =(TH1D*) h->Clone(Form("ratio_%d",i));
				hr->Divide(h2);
				hratio->add(hr, 0,i);
				hr->GetXaxis()->SetTitle("p_{T}");
		}

		auto c = new plotManager();
		c->initSquarePad("c", "", 1, 3);
		//c->initOverlayPad("c", "", 1, 3);
		c->addm2TH1(hptpp, "pp", "pfl");
		c->addm2TH1(hptpb, "pb", "pfl");
		c->setRatioYrange(0.5, 1.5);
		c->draw();
		c->drawLegend();
		c->save("/eos/user/w/wangx/www/test/spectra.png");

		auto c2 = new plotManager();
		c2->initSquarePad("c", "", 1, 3);
		//c->initOverlayPad("c", "", 1, 3);
		c2->addm2TH1(hratio, "PbPb/pp", "pfl");
		c2->setYrange(0.8, 2.);
		c2->draw();
		c2->drawLegend();
		c2->save("/eos/user/w/wangx/www/test/spectra_ratio.png");

}
