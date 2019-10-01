

namespace utility{
	const Double_t etabin[24] ={-3.5, -3, -2.5,-2.,-1.5, -1., -0.8, -0.6, -0.4, -0.3, -0.2, -0.1, 0.1, 0.2, 0.3, 0.4, 0.6, 0.8, 1., 1.5,2.,2.5, 3, 3.5};

	const Double_t phibin[18] ={-1.50796, -1.00531,-0.879646, -.75398, -0.628319,-0.502655, -0.376991, -0.251327, -0.125664, 0.125664, 0.251327, 0.376991, 0.502655, 0.628319,.75398, 0.879646, 1.00531,1.50796};

	const Double_t trkptbin[9] = {0.7, 1, 2, 3, 4, 8, 12, 20, 50};

	float findDr(double eta1, double phi1, double eta2, double phi2){
		double dphi = phi1 - phi2;
		while (dphi > M_PI) dphi-= 2*M_PI;
		while (dphi <= -M_PI) dphi += 2*M_PI;
		return sqrt(pow(dphi,2)+pow(eta1-eta2,2));
	}
}
