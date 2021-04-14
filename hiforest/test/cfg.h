
#include "myProcesses/liteFrame/plugin/toolkit.h"
#include "myProcesses/liteFrame/plugin/liteFrame.h"
#include "myProcesses/jtc/plugin/jtcLib.h"

#if defined(event_content_skim)
#include "myProcesses/hiforest/plugin/eventMap_skim.h"
#elif defined(event_content_forest)
#include "myProcesses/hiforest/plugin/eventMap_hiForest.h"
#endif
#include "myProcesses/HIN-20-003/JEC2018PbPb/JECUncert.h"
#include "myProcesses/HIN-20-003/JEC2018PbPb/JECorr.h"

#include "TF1.h"

enum trkType {inclTrk, sube0,suben0};

namespace config_skim_pp{


	class pset_nominal_forest{
		public :
			pset_nominal_forest():
				centbin {0,  180},
				ptbin{1, 2, 3,4,8, 12, 300}
			{
				std::stringstream s1, s2;
				for(int i=0; i<nptbin; i++){
					s1.str(std::string());
					s2.str(std::string());
					s1<<ptbin[i];
					s2<<ptbin[i+1];
					ptLabel[i] =TString("Pt:"+s1.str()+"-"+s2.str());
				}
				centLabel = toolkit::makeCentLabels(ncent, centbin);

			}
			~pset_nominal_forest(){}
			TString *centLabel, ptLabel[6];
			const int ncent = 1, nptbin = 6;
			const float centbin[2], ptbin[7];
			TString jetSetName="ak4PFJetAnalyzer";
			bool isMC = 0, isHI=0;
			std::string *evtFilterString=0;
			std::string *trigger=0;
			int ntrigs= 0;
			int nfilters=0;
	};

	class selections {	
		public : 
			selections(){
			}
			~selections(){}
			xTagger tagRecoJet(eventMap *em, int i){
				xTagger tag;
				//kinematic selection:
				float jetpt = em->jetpt[i];
				if( jetpt < 120) return tag;
				if(TMath::Abs(em->jeteta[i]) > 1.6) return tag;

				//other selections
				//cout<<em->pthat;
				return tag;
			}

			xTagger tagGenJet(eventMap *em, int i){
				xTagger tag;
				if( em->genjetpt[i] < 120 ) return tag;
				if( TMath::Abs(em->genjet_wta_eta[i]) > 1.6 ) return tag;

				//add incl jet tag
				return tag;
			}

			xTagger tagTrk(eventMap *em, int j){
				xTagger tag; 
				if(em->trkpt[j] < 1 || em->trkpt[j]>400) return tag;
				if(TMath::Abs(em->trketa[j]) >= 2.4) return tag;
				if(!(em->highPurity[j])) return tag;
				if(em->trknhit[j]< 11) return tag;
				if(em->trkchi2[j]/em->trkndof[j]/em->trknlayer[j] > 0.15) return tag;
				float et = (em->pfEcal[j] + em->pfHcal[j])/TMath::CosH(em->trketa[j]);
				if( !(em->trkpt[j]<20.0 || et>0.5*(em->trkpt[j]))) return tag;

				tag.addTag(trkType::inclTrk);
				return tag;
			}

			xTagger tagGenParticle(eventMap *em, int j){
				xTagger tag;

				if(em->gppt(j) < 1.0) return tag;
				if(TMath::Abs(em->gpeta(j)) > 2.4) return tag;
				if(em->gpchg(j)==0) return tag;

				tag.addTag(trkType::inclTrk);
				if(em->gpSube(j) ==0 ) tag.addTag(trkType::sube0);
				else tag.addTag(trkType::suben0);
				return tag;
			}

			bool evtCut(eventMap *em){
				return 0;
			}

			jtc::JEUncertTool jeutool;
			JetUncertainty JEU;
			bool doJEUUp = 0, doJEUDown = 0;
	};

	class weight_dummy{
		public :

			weight_dummy(){
			}
			~weight_dummy(){}
			float evtWeight(eventMap* e){return 1;}
			//float evtWeight(eventMap* e){return (e->weight)*(fvzw1->Eval(e->vz))*(fcentw1->Eval(e->hiBin-10));}
			float trkWeight(eventMap* e, int i, xTagger &tag){return 1;}
			float recoJetWeight(eventMap* e, int i, xTagger &tag){
				return 1;
			}
			float genJetWeight(eventMap* e, int i, xTagger &tag){return 1;}
			float genParticleWeight(eventMap* e, int i, xTagger &tag){return 1;}

			TF1 * fvzw1, *fvzw2;
			TF1 * fcentw1, *fcentw2;
	};
}
