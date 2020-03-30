
#ifndef JTCLIB_H
#define JTCLIB_H
#include "myProcesses/jtc/plugin/Utility.h"
namespace jtc_default{
	//float drbins[] = {0.,0.05,0.1,0.15,0.2,0.3,0.4,0.6,0.8,1.,1.2, 1.5, 2, 2.5};
	//int ndrbin = 13;
	float drbins[] = {0.,0.05,0.1,0.15,0.2,0.25,0.3,0.35,0.45,0.6,0.8,1.,1.2, 1.5, 2, 2.5};
	int ndrbin = 15;
	Double_t etabin[24] ={-3.5, -3, -2.5,-2.,-1.5, -1., -0.8, -0.6, -0.4, -0.3, -0.2, -0.1, 0.1, 0.2, 0.3, 0.4, 0.6, 0.8, 1., 1.5,2.,2.5, 3, 3.5};

	Double_t phibin[18] ={-1.50796, -1.00531,-0.879646, -.75398, -0.628319,-0.502655, -0.376991, -0.251327, -0.125664, 0.125664, 0.251327, 0.376991, 0.502655, 0.628319,.75398, 0.879646, 1.00531,1.50796};
}
namespace jtc{

	TH2D* mixingTableMaker(TH2D* mix, bool doSmooth = 1){
		float midLeft = -0.1;
		float midRight = 0.1;
		//make the mixing table invariant
		mix->Scale(1.0/mix->Integral()/mix->GetXaxis()->GetBinWidth(1)/mix->GetYaxis()->GetBinWidth(1));
		TString name = mix->GetName();
		name = name+"_p1";
		TH2D* ME = (TH2D*) mix->Clone(name);
		float mean=0;
		//		temp->Scale(1.0/mean);
		if(doSmooth){
			TH1D* temp = (TH1D*)mix->ProjectionX("_eta");
			int binLeft = temp->FindBin(midLeft);
			int binRight= temp->FindBin(midRight)+1;
			for(int i=binLeft;i<binRight; i++){
				mean += temp->GetBinContent(i);
			}
			mean = mean /(binRight-binLeft);
			for(int ix=1; ix<mix->GetNbinsX()+1; ix++){
				for(int iy=1; iy<mix->GetNbinsY()+1; iy++){
					if( ix< binRight && ix>= binLeft){
						ME->SetBinContent(ix, iy, 1);
						ME->SetBinError(ix, iy, 0);
					}
					else{
						ME->SetBinContent(ix, iy, temp->GetBinContent(ix)/mean);
						ME->SetBinError(ix, iy, temp->GetBinError(ix)*sqrt(mix->GetNbinsY())/mean);
					}
				}
			}
			delete temp; 
		} else { //only fill the empty bin of the mixing by the content we have
			// this part is still under developing, it depends on the normalization method you want.
			int binLeft = mix->GetXaxis()->FindBin(midLeft);
			int binRight= mix->GetXaxis()->FindBin(midRight)+1;
			int binDown = mix->GetYaxis()->FindBin(midLeft);
			int binUpper= mix->GetYaxis()->FindBin(midRight)+1;
			int nbins = 0;
			for(int i=binLeft;i<binRight; i++){
				for(int j=binDown;j<binUpper; j++){
					mean += mix->GetBinContent(i,j);
					nbins++;
				}
			}
			mean = mean /nbins;
			ME->Scale(1.0/mean);
		}
		return ME;
	}

	void scale_Y_TF1(TH2* h, TF1 *f){
		for(int ix=1; ix<h->GetNbinsX()+1; ix++){
			float x = h->GetXaxis()->GetBinCenter(ix);
			float s = f->Eval(x);
			for(int iy=1; iy<h->GetNbinsY()+1; iy++){
				float bin = h->GetBinContent(ix,iy);
				float err = h->GetBinError(ix,iy);
				h->SetBinContent(ix,iy, bin/s);
				h->SetBinError(ix,iy, err/pow(s,0.5));
			}
		}
	}
	TH2D* getV2Bkg(TH2D* signal, float sideMin,float sideMax){
		TString stemp = signal->GetName();
		TH2D* bkg = (TH2D*)signal->Clone("bkg_"+stemp);
		int outterRight= signal->GetXaxis()->FindBin(sideMax);
		int innerRight = signal->GetXaxis()->FindBin(sideMin);
		int outterLeft = signal->GetXaxis()->FindBin(-signal->GetXaxis()->GetBinCenter(outterRight));
		int innerLeft  = signal->GetXaxis()->FindBin(-signal->GetXaxis()->GetBinCenter(innerRight));
		TH1D* temp = (TH1D*)bkg->ProjectionY("bkg_sideBandProj", innerRight, outterRight, "e");
		temp->Add(bkg->ProjectionY("", outterLeft, innerLeft, "e"));

		TH2D* aux_bkg = (TH2D*)signal->Clone("aux_bkg");
		for(int iy=1; iy<bkg->GetNbinsY()+1; iy++){
			for(int ix=1; ix<bkg->GetNbinsX()+1; ix++){
				aux_bkg->SetBinContent(ix, iy, 1);
			}
		}
		TH1D* aux_proj = (TH1D*)aux_bkg->ProjectionY("aux_proj", innerRight, outterRight);
		aux_proj->Add(aux_bkg->ProjectionY("", outterLeft, innerLeft));
		for(int iy=1; iy<bkg->GetNbinsY()+1; iy++){
			int nbin = int(aux_proj->GetBinContent(iy));
			for(int ix=1; ix<bkg->GetNbinsX()+1; ix++){
				bkg->SetBinContent(ix, iy, temp->GetBinContent(iy)/nbin);
				bkg->SetBinError(ix, iy, temp->GetBinError(iy)/sqrt(nbin));
			}
		}
		temp->Delete();
		aux_bkg->Delete();
		aux_proj->Delete();
		return bkg;
	}
	void drIntegral(TH2D* signal, TH1D* drDist){
		// this should return the invariant dr distribution (dr bin width will be divided out )
		float content, error, width, dr;
		float xwidth, ywidth;
		drDist->Sumw2();
		for(int i=1; i<drDist->GetNbinsX()+1;i++){
			drDist->SetBinContent(i, 0);
			drDist->SetBinError(i, 0);
		}
		for(int jx=0; jx<signal->GetNbinsX(); jx++){
			for(int jy=0; jy<signal->GetNbinsY(); jy++){
				dr = sqrt( pow(signal->GetXaxis()->GetBinCenter(jx),2) +\
						pow(signal->GetYaxis()->GetBinCenter(jy),2));
				xwidth = signal->GetXaxis()->GetBinWidth(jx);
				ywidth = signal->GetYaxis()->GetBinWidth(jy);
				// integrand f(x,y)dxdy
				content = signal->GetBinContent(jx,jy)*xwidth*ywidth;
				if( content!=0 ) {
					error = sqrt(pow(drDist->GetBinError(drDist->FindBin(dr)),2)+\
							pow(signal->GetBinError(jx,jy)*xwidth*ywidth,2));
					drDist->Fill(dr, content);
					drDist->SetBinError(drDist->FindBin(dr), error);
				}
			}
		}
		// make the histogram invariant
		divide_bin_size(drDist);
		return;
	}
	TH1D* doDrIntegral(TString name , TH2D* sig, int ndr, float * drbin){
		TString title = sig->GetTitle();
		TH1D* drDist = new TH1D(name, title, ndr, drbin);
		drIntegral(sig, drDist);
		drDist->GetXaxis()->SetTitle("#Deltar");
		return drDist;
	}
	TH1* projectionX(TH2D* h, float x, float y, TString opt){
		int xbin = h->GetYaxis()->FindBin(x);
		int ybin = h->GetYaxis()->FindBin(y);
		ybin = h->GetYaxis()->FindBin(y-h->GetYaxis()->GetBinWidth(ybin));
		TString name = h->GetName(); name = "projX_"+name;
		return h->ProjectionX(name, xbin, ybin , opt);
	}
	TH1* projectionY(TH2D* h, float x, float y, TString opt){
		int xbin = h->GetXaxis()->FindBin(x);
		int ybin = h->GetXaxis()->FindBin(y)-1;
		ybin = h->GetXaxis()->FindBin(y-h->GetXaxis()->GetBinWidth(ybin));
		TString name = h->GetName(); name = "projY_"+name;
		return h->ProjectionY(name, xbin, ybin ,opt);
	}
	TH1* projX(bool doRebin, TH2D *h2, float x, float y, TString opt){
		// here h2 needs to be invariant
		TH1* h=projectionX(h2, x, y, opt);
		h->Scale(h2->GetYaxis()->GetBinWidth(1));
		if(doRebin){
			TString name = h->GetName();
			name = "rebined_"+name;
			TH1* hh=h;
			h=invariantRebin(h,name, 23, jtc_default::etabin);
			delete hh;
		}
		return h;
	}
	TH1* projY(bool doRebin, TH2D*h2, float x, float y, TString opt){
		// here h2 needs to be invariant
		TH1* h=projectionY(h2, x, y, opt);
		h->Scale(h2->GetXaxis()->GetBinWidth(1));
		if(doRebin){
			TString name = h->GetName();
			name = "rebined_"+name;
			TH1* hh=h;
			h=invariantRebin(h,name, 17, jtc_default::phibin);
			delete hh;
		}
		return h;
	}
	TH2D* rotate2D(TString name, TH2D* h0){
		//auto h = new TH2D(name, "", yax->GetSize(), newx, xax->GetSize(), newy);
		auto h = new TH2D(name, "", 500, -5, 5, 200, -TMath::Pi()/2, 3*TMath::Pi()/2);

		for(int i=1; i<h0->GetNbinsX()+1; ++i){
			for(int j=1; j<h0->GetNbinsY()+1; ++j){
				Double_t cont = h0->GetBinContent(i,j);
				Double_t err = h0->GetBinError(i,j);
				h->SetBinContent(j,i, cont);
				h->SetBinError(j,i, err);
			}
		}
		return h;
	}

	void scaleTF1(TF1*f, float c, int n){
		for(int i=0; i< n; ++i){
			float p = f->GetParameter(i);
			f->SetParameter(i,p*c);
		}
	}
}


#endif
