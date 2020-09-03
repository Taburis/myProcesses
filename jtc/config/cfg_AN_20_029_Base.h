
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
				if(e->isMC && e->pthat < 50) return 1;
				return 0;
			}
			bool recoJetCut(event *e, int j){
				if(TMath::Abs(e->jeteta[j])> jeteta_max) return 1;	
				if(TMath::Abs(e->jetpt[j]) < jetpt_min ) return 1;	
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
	template <typename event>
		class weight_pythia_c5shift{	
			public :
				weight_pythia_c5shift(){
					fvzw   = new TF1("fvzw", "pol5", -15, 15);
					fcentw = new TF1("fcentw", "pol6",0, 180);
					fvzw->SetParameters(0.998703,-0.0241091,-0.0012585,0.000150923,2.07724e-05, -1.23832e-06);
					fcentw->SetParameters(0.774353,0.0268645, -0.000126846,-4.60746e-06,4.04572e-08,-8.98259e-11);
				}
				~weight_pythia_c5shift(){}	
				float evtWeight(event* e){return (e->weight)*(fvzw->Eval(e->vz))*(fcentw->Eval(e->hiBin-10));}
				TF1 * fvzw;
				TF1 * fcentw;
		};
}

#endif
