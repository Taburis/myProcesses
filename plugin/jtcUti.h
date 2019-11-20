

#ifndef JTCUTI_H
#define JTCUTI_H

struct histCase{
	//if want to add any hist, need to add it in the quickHistReg as well, and add the filling in the fillCase
	TH2D** sig;
	TH2D** sig_pTweighted;
	TH2D** mixing;
	TH1D** jet_pt;
	TH1D** jet_eta;
	TH1D** jet_phi;
};

enum recoType { recojet, genjet, recotrk, gentrk, data, inclJet, trueBJet};

TString type2name(recoType t1){
	if( recoType::recojet == t1) return "RecoJet";
	else if(recoType::recotrk == t1) return "RecoTrk";
	else if(recoType::genjet == t1) return "GenJet";
	else if(recoType::gentrk == t1) return "GenTrk";
	else if( recoType::data == t1) return "Data";
	else if( recoType::inclJet == t1) return "inclJet";
	else if( recoType::trueBJet == t1) return "trueBJet";
	else return "unknown";
}

TString makeRecoTypeName(recoType t1, recoType t2){
	return type2name(t1)+"_"+type2name(t2);
}


double findDr(double eta1, double phi1, double eta2, double phi2){
	double dphi = phi1 - phi2;
	while (dphi > M_PI) dphi-= 2*M_PI;
	while (dphi <= -M_PI) dphi += 2*M_PI;
	return sqrt(pow(dphi,2)+pow(eta1-eta2,2));
}
/*
void quickHistReg(histManager *h, TString cap, histCase &hc, int nPt, int nCent){
	int nHistoBinsX = 500;
	int nHistoBinsY = 200;
	hc.sig= new TH2D*[nPt*nCent];
	hc.sig_pTweighted= new TH2D*[nPt*nCent];
	hc.mixing= new TH2D*[nPt*nCent];
	TString tmp, name;
	if(isMC){
		if(doGenJet ) name= cap+"_GenJet_";
		else name=cap+"_RecoJet_";
		if(doGenTrack) name= name+"GenTrack";
		else name = name +"RecoTrack";
	} else name = cap+"_Data";
	for(int i=0; i<nPt; ++i){
		for(int j=0; j<nCent; ++j){
			tmp = centLabel[j]+"_"+centLabel[j+1]+"_"+ptLabel[i]+"_"+ptLabel[i+1];
			hc.sig[i+j*nPt] = hm->regHist<TH2D>(name+Form("_%d_%d",i, j), tmp,
					nHistoBinsX,-5,5,nHistoBinsY,-TMath::Pi()/2,3*TMath::Pi()/2);
			hc.sig_pTweighted[i+j*nPt] = hm->regHist<TH2D>(name+Form("_pTweighted_%d_%d",i, j), tmp,
					nHistoBinsX,-5,5, nHistoBinsY,-TMath::Pi()/2,3*TMath::Pi()/2);
			hc.mixing[i+j*nPt] = hm->regHist<TH2D>(name+Form("_mixing_%d_%d",i, j), tmp,
					nHistoBinsX,-5,5, nHistoBinsY,-TMath::Pi()/2,3*TMath::Pi()/2);
		}
	}

	const float newbin [21] = {110, 120, 136, 152, 168, 184, 200, 216, 232, 248, 264, 280, 296, 312, 328, 344, 360,
		380, 400, 432, 500};
	int nbin = 20;
	name=cap;
	hc.jt_wta_pt = new TH1D*[nCent];
	hc.jt_wta_eta = new TH1D*[nCent];
	hc.jt_wta_phi = new TH1D*[nCent];
	hc.jt_wta_deta = new TH1D*[nCent];
	hc.jt_wta_dphi = new TH1D*[nCent];
	hc.jet_corrpt = new TH1D*[nCent];
	hc.jet_eta = new TH1D*[nCent];
	hc.jet_phi = new TH1D*[nCent];
	const float wta_ptbin [17] = {1, 2, 3, 4,5 ,6,7 ,8, 10, 12, 14, 16, 18, 20, 50, 100, 500};
	for(int j=0; j<nCent; ++j){
		tmp = centLabel[j]+" to "+centLabel[j+1];
		hc.jet_corrpt[j] = hm->regHist<TH1D>(name+Form("_corrpt_%d",j), tmp, nbin, newbin);
		hc.jet_eta[j] = hm->regHist<TH1D>(name+Form("_eta_%d",j), tmp, 100, -2.0, 2.0);
		hc.jet_phi[j] = hm->regHist<TH1D>(name+Form("_phi_%d",j), tmp, 72, -TMath::Pi(), TMath::Pi());
	}
}
*/
#endif
