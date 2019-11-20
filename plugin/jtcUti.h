

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


double findDr(double eta1, double phi1, double eta2, double phi2){
	double dphi = phi1 - phi2;
	while (dphi > M_PI) dphi-= 2*M_PI;
	while (dphi <= -M_PI) dphi += 2*M_PI;
	return sqrt(pow(dphi,2)+pow(eta1-eta2,2));
}



#endif
