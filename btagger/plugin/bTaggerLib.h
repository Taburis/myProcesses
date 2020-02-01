
#include "myProcesses/jtc/plugin/PLOTLIB/multi_pads.h"

namespace btagger_utility{
	enum flavorID {udsg, c, b, unknown};

	struct jecSet {TH1* hincl = nullptr, *hb = nullptr, *htg = nullptr;};
	struct histBundle {TString name; TH1 *hudsg=nullptr, *hc=nullptr, *hb=nullptr, *hdata=nullptr; bool kData=0, kMC=0;};

	void negativeTagger(TString name, TString title, TH2* hneg, TH2* hpos, TH1* neg_data, TH1* pos_data){
		TH1D *hframe = new TH1D(name+"_frame",title, 1, -1.1, 1.1);

	}

	TH1D* flip_TH1(TH1* neg){
		TString name = neg->GetName();
		TH1D * h = new TH1D(name+"_flipped","", 110, -1.1, 0);
		int nbin = neg->GetNbinsX();
		for(int i=1; i<nbin+1; i++){
			double cent = neg->GetXaxis()->GetBinUpEdge(i);
			int negi = h->FindBin(-cent);
			h->SetBinContent(negi, neg->GetBinContent(i));
			h->SetBinError(negi, neg->GetBinError(i));
		}
		return h;
	}

	void flipBundle(histBundle &hb1, histBundle &hb2){
		if(hb2.kMC ){
			hb1.hudsg=flip_TH1(hb2.hudsg);
			hb1.hb   =flip_TH1(hb2.hb);
			hb1.hc   =flip_TH1(hb2.hc);
			hb1.kMC =1;
		}else if(hb2.kData){
			hb1.hdata=flip_TH1(hb2.hdata);
			hb1.kData =1;
		}
	}

	void projFlavor(histBundle &hb, TH2* h, bool isData){
		hb.name = h->GetName();
		if(isData){
			hb.hdata = (TH1*)h->ProjectionX(hb.name+"_data", flavorID::unknown+1, flavorID::unknown+1);
			hb.kData= 1;
		}else {
			hb.hudsg = (TH1*) h->ProjectionX(hb.name+"_udsg", flavorID::udsg+1, flavorID::udsg+1);
			hb.hc = (TH1*) h->ProjectionX(hb.name+"_c", flavorID::c+1, flavorID::c+1);
			hb.hb = (TH1*) h->ProjectionX(hb.name+"_b", flavorID::b+1, flavorID::b+1);
			hb.kMC= 1;
		}
	}

	void cleanHistBundle(histBundle &hc){
		if(hc.kMC ){
			hc.hudsg->Delete();
			hc.hb   ->Delete();
			hc.hc   ->Delete();
		}else if(hc.kData) hc.hdata->Delete();
		hc.kData = 0; hc.kMC= 0;
	}

//	TH1* draw_scaleFactor_stack(TString name,TString title, histBundle &hmcp, histBundle &hmcn){
//		auto hframe = new TH1F(name, title, 1, -1.2, 1.2);
//
//	}

	//	multi_pads<overlay_pads>(TSTring name, TH2* hmc, TH2* hdata){
	//		histBundle hbmc, hbdata;
	//		projFlavor(hbmc, hmc, 0);
	//		projFlavor(hbdata, hdata, 1);
	//		return 
	//	}
}

