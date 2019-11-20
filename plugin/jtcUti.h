

enum correlationType { recojet, genjet, recotrk, gentrk, data};

TString type2name(correlationType t1){
	if( correlationType::recojet == t1) return "RecoJet";
	else if(correlationType::recotrk == t1) return "RecoTrk";
	else if(correlationType::genjet == t1) return "GenJet";
	else if(correlationType::gentrk == t1) return "GenTrk";
	else if( correlationType::data == t1) return "Data";
	else return "unknown";
}


double findDr(double eta1, double phi1, double eta2, double phi2){
	double dphi = phi1 - phi2;
	while (dphi > M_PI) dphi-= 2*M_PI;
	while (dphi <= -M_PI) dphi += 2*M_PI;
	return sqrt(pow(dphi,2)+pow(eta1-eta2,2));
}
