
#ifndef cfg_AN_20_029_Cut_H
#define cfg_AN_20_029_Cut_H

#include "myProcesses/jtc/plugin/configBase.h"
namespace configuration{

	class pset_nominal{
		public :
			pset_nominal():
				centLabel{"Cent: 0-30%","Cent: 30-90%"},
				centbin {0,60, 180},
				ptbin{1, 2, 3,4,8, 12, 300}
			{}
			const TString centLabel[2];
			const int ncent = 2, npt = 6;
			const float centbin[3], ptbin[7];
			TString jetSetName;
	};

	class pset_jtc_c5shift{
		//config used for correlation and centrality shifted by 5 percent
		public :
			pset_jtc_c5shift():
				centLabel{"Cent: 5-35%","Cent: 35-95%"},
				centbin {10,70, 190},
				ptbin{1, 2, 3,4,8, 12, 300}
			{}
			const TString centLabel[2];
			const int ncent = 2, npt = 6;
			const float centbin[3], ptbin[7];
			TString jetSetName;
	};

	template <typename event>
		struct cuts_nominal{
			bool evtCut(event*e){
				if(fabs(e->vz)>15) return 1;
				if(e->hiBin > centMax || e->hiBin < centMin) return 1;
				return 0;
			}
			bool recoJetCut(event *e, int j){
				if(TMath::Abs(e->jeteta[j])> jeteta_max) return 1;	
				if(TMath::Abs(e->jetpt[j]) > jetpt_min ) return 1;	
				return 0;
			}
			bool trkCuts( event* em, int j){
				if(em->trkpt[j] < 1 || em->trkpt[j]>400) return 1;
				if(TMath::Abs(em->trketa[j]) >= 2.4) return 1;
				if(!(em->highPurity[j])) return 1;
				if(em->trknhit[j]< 11) return 1;
				if(em->trkchi2[j]/em->trkndof[j]/em->trknlayer[j] > 0.15) return 1;
				float et = (em->pfEcal[j] + em->pfHcal[j])/TMath::CosH(em->trketa[j]);
				if( !(em->trkpt[j]<20.0 || et>0.5*(em->trkpt[j]))) return 1;
				return 0;
			}

			float centMax = 180, centMin = 0, jeteta_max = 1.6, jetpt_min = 50;
		};


	template <typename event>
		class weight_dummy{	
			public :
				weight_dummy(){}	
				~weight_dummy(){}	
				float evtWeight(event* e){return 1;}
		};
	template <typename event>
		class weight_pthat{	
			public :
				weight_pthat(){}	
				~weight_pthat(){}	
				float evtWeight(event* e){return e->weight;}
		};

}

#endif
