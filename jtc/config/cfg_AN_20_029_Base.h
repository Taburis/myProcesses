
#ifndef cfg_AN_20_029_Cut_H
#define cfg_AN_20_029_Cut_H

#include "myProcesses/jtc/plugin/configBase.h"
#include "TF1.h"
namespace configuration{

	class pset_nominalpp{
		public :
			pset_nominalpp():
				centLabel{""},
				centbin {-100, 180},
				ptbin{1, 2, 3,4,8, 12, 300}
			{
				evtFilterString = new std::string[3];
				evtFilterString[0]="pPAprimaryVertexFilter";
				evtFilterString[1]="HBHENoiseFilterResultRun2Loose";
				evtFilterString[2]="pBeamScrapingFilter";
			}
			const TString centLabel[2];
			const int ncent = 1, npt = 6;
			const float centbin[2], ptbin[7];
			TString jetSetName="ak4PFJetAnalyzer";
			bool isMC = 0, isHI=0;
			std::string *evtFilterString;
			int nfilters=3;
	};

	class pset_nominalHI_skim{
		public :
			pset_nominalHI_skim():
				centLabel{"Cent: 0-30%","Cent: 30-90%"},
				centbin {0,60, 180},
				ptbin{1, 2, 3,4,8, 12, 300}
			{}
			const TString centLabel[2];
			const int ncent = 2, npt = 6;
			const float centbin[3], ptbin[7];
			TString jetSetName="akFlowCsPu4PFJetAnalyzer";
			bool isMC = 0, isHI=1;
			std::string *evtFilterString=0;
			int nfilters=0;
	};

	class pset_jtc_c5shift_skim{
		//config used for correlation and centrality shifted by 5 percent
		public :
			pset_jtc_c5shift_skim():
				centLabel{"Cent: 5-35%","Cent: 35-95%"},
				centbin {10,70, 190},
				ptbin{1, 2, 3,4,8, 12, 300}
			{}
			const TString centLabel[2];
			const int ncent = 2, npt = 6;
			const float centbin[3], ptbin[7];
			TString jetSetName="akFlowCsPu4PFJetAnalyzer";
			bool isMC = 0, isHI=1;
			std::string *evtFilterString=0;
			int nfilters=0;
	};

	template <typename event>
		struct cuts_nominal{
			bool evtCut(event*e){
				if(fabs(e->vz)>15) return 1;
				if(e->isHI){
					if(e->hiBin > centMax || e->hiBin < centMin) return 1;
				}
				if(e->checkEventFilter()) return 1;
				if(e->isMC && e->pthat < 80) return 1;
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
		class weight_Hydjet_c5shift{	
			public :
				weight_Hydjet_c5shift(){
					fvzw   = new TF1("fvzw", "pol5", -15, 15);
					fcentw = new TF1("fcentw", "pol7",0, 180);
					fvzw->SetParameters(1.00402,-0.0193632,0.000837705,-2.68852e-05,-4.17499e-06,1.22989e-07);
					fcentw->SetParameters(3.20695,-0.0579797,0.000905921,-1.64399e-05,1.86091e-07,-1.1416e-09, 3.5897e-12, -4.56036e-15);
				}
				~weight_Hydjet_c5shift(){}	
				float evtWeight(event* e){return (e->weight)*(fvzw->Eval(e->vz))*(fcentw->Eval(e->hiBin-10));}
				TF1 * fvzw;
				TF1 * fcentw;
		};


	enum jetType {inclJet, gspJet, bJet, bHadronJet};

	template<typename evt>
	xTagger bTagJetSelection(evt* em , int j){
		xTagger tag;
		tag.addTag(jetType::inclJet);
		if(fabs(em->flavor_forb[j])==5) tag.addTag(jetType::bJet);
		if(em->bHadronNumber[j]>0 ) tag.addTag(jetType::bHadronJet);
		if(em->bHadronNumber[j]==2){ tag.addTag(jetType::gspJet);
			//cout<<"find gsp jet"<<endl;
		}
		return tag;
	}

}

#endif
