
#ifndef CONFIG_AA2018_BJET_H
#define CONFIG_AA2018_BJET_H

namespace AA2018bJet{
	int ncent = 4;
	//drop the tails
	float centbins[] = {0, 20, 60, 100, 180};
	//float centbins[] = {0, 20, 60, 100, 200};
	bool evtCentCuts(eventMap* em){
		if(em->hiBin >= centbins[ncent] ||em->hiBin < centbins[0]) return 1;
		return 0; 
	}
	bool recoJetCut(eventMap *em, int j){
		if(em->jetpt[j]< 80) return 1;
		//if(em->jetpt[j]< 120) return 1;
		if(TMath::Abs(em->jeteta[j])> 1.6) return 1;
		return 0;
	}
	bool basicEvtCuts(eventMap *em){
		if(em->checkEventFilter()) return 1;
		if(TMath::Abs(em->vz) > 15) return 1;
		return evtCentCuts(em);
	}
	std::string filters[] = {"collisionEventSelectionAODv2","HBHENoiseFilterResultRun2Loose", "pprimaryVertexFilter","phfCoincFilter3Th3", "pclusterCompatibilityFilter"};
	int nfilter = 5;
	TF1 fvzw("fvz","pol6",-15,15);
	TF1 fcentw("fcent","pol6",0,180);
	void config_init(){
		fcentw.SetParameters(4.64945,-0.201337/2, 0.00435794/4,-7.00799e-05/8,8.18299e-07/16,-5.52604e-09/32,1.54472e-11/64);
		fvzw.SetParameters(1.00656, -0.0193651, 0.000976851, -1.043e-05, -9.79808e-06, 9.07733e-08, 1.79165e-08);
	}
	float inclJetEvtWeight(eventMap *em){return (fcentw.Eval(em->hiBin))*(fvzw.Eval(em->vz));}
}

#endif
