
#ifndef CONFIG_HIN_18_020_H
#define CONFIG_HIN_18_020_H
#include "TROOT.h"
#include <list>
#include "myProcesses/jtc/plugin/Utility.h"
#include "TF1.h"

class eventMap;
namespace AA2018bJet{
	int npt = 6;
	float ptbins[] = {1, 2, 3,4,8, 12, 300};
	int ncent = 2;
	TString* ptLabels, *centLabels;
	//drop the tails
	float centbins[] = {0, 60, 180};
	float jetpt_min = 120, jeteta_max =1.6;
	bool recoJetCuts(eventMap *em, int j){
		if(em->jetpt[j]< jetpt_min) return 1;
		if(TMath::Abs(em->jeteta[j])> jeteta_max) return 1;
		return 0;
	}
	bool basicEvtCuts(eventMap *em){
		if(em->checkEventFilter()) return 1;
		if(TMath::Abs(em->vz) > 15) return 1;
		if(em->hiBin >= centbins[ncent] ||em->hiBin < centbins[0]) return 1;
		return 0;
	}
	float jetWeight(float pt, float eta, float phi){
		return 1;
	}
	std::string evtFilters_skimPatch[] = {"HBHENoiseFilterResultRun2Loose", "pprimaryVertexFilter","phfCoincFilter2Th4", "pclusterCompatibilityFilter"};
	std::string evtFilters[] = {"collisionEventSelectionAODv2","HBHENoiseFilterResultRun2Loose", "pprimaryVertexFilter","phfCoincFilter2Th4", "pclusterCompatibilityFilter"};
	int nEvtFilter = 5;
	TF1 fvzw("fvz","pol6",-15,15);
	TF1 fcentw("fcent","pol6",0,180);
	centralityHelper centHelper(ncent, centbins);

	void config_init(){
		std::stringstream s1, s2;
		centLabels = centHelper.makeLabels();
		ptLabels= new TString[npt+1];
		for(int i=0; i<npt+1; i++){
			s1.str(std::string());
			s2.str(std::string());
			s1<<ptbins[i];
			s2<<ptbins[i+1];
			ptLabels[i] =TString("Pt:"+s1.str()+"-"+s2.str());
		}
		fcentw.SetParameters(7.85523,-.289198,.00449837,-3.77752e-05,1.80269e-07,-4.62581e-10,4.97671e-13);
		fvzw.SetParameters(1.00656, -0.0193651, 0.000976851, -1.043e-05, -9.79808e-06, 9.07733e-08, 1.79165e-08);
	}
	float evtWeight(eventMap *em){return (fcentw.Eval(em->hiBin))*(fvzw.Eval(em->vz))*em->weight;}

	//mixing setup
	int nhibin_mix = 36;
	int nvz_mix = 30;
	float hibin_min_mix = 0, hibin_max_mix = 180;


	bool trkCuts( eventMap * em, int j){
		if(em->trkpt[j] < 1 || em->trkpt[j]>400) return 1;
		if(TMath::Abs(em->trketa[j]) >= 2.4) return 1;
		if(!(em->highPurity[j])) return 1;
		if(em->trknhit[j]< 11) return 1;
		if(em->trkchi2[j]/em->trkndof[j]/em->trknlayer[j] > 0.15) return 1;
		float et = (em->pfEcal[j] + em->pfHcal[j])/TMath::CosH(em->trketa[j]);
		if( !(em->trkpt[j]<20.0 || et>0.5*(em->trkpt[j]))) return 1;
		return 0;
	}
}

#endif
