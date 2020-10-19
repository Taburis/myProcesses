
#include "myProcesses/liteFrame/plugin/toolkit.h"
#include "myProcesses/liteFrame/plugin/liteFrame.h"
//#include "myProcesses/jtc/plugin/producerBJTC.h"

#if defined(event_content_skim)
#include "myProcesses/hiforest/plugin/eventMap_skim.h"
#elif defined(event_content_forest)
#include "myProcesses/hiforest/plugin/eventMap_hiForest.h"
#endif

#include "TF1.h"

enum jetType {inclJet, trueBJet, taggedJet, negTagJet, cjet};
enum trkType {inclTrk, sube0,suben0};

namespace config_AN20029{


	class pset_nominalHI_skim{
		public :
			pset_nominalHI_skim():
				centbin {0,60, 180},
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
			~pset_nominalHI_skim(){}
			TString *centLabel, ptLabel[6];
			const int ncent = 2, nptbin = 6;
			const float centbin[3], ptbin[7];
			TString jetSetName="akFlowPuCs4PFJetAnalyzer";
			bool isMC = 0, isHI=1;
			std::string *evtFilterString=0;
			int nfilters=0;
	};

	class pset_nominalHI_forest{
		public :
			pset_nominalHI_forest():
				centbin {0,60, 180},
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
			~pset_nominalHI_forest(){}
			TString *centLabel, ptLabel[6];
			const int ncent = 2, nptbin = 6;
			const float centbin[3], ptbin[7];
			TString jetSetName="akFlowPuCs4PFJetAnalyzer";
			bool isMC = 0, isHI=1;
			std::string *evtFilterString=0;
			int nfilters=0;
	};


	struct selections {
		xTagger tagRecoJet(eventMap *em, int i){
			xTagger tag;
			//kinematic selection:
			float jetpt = em->jetpt[i];
			if( jetpt < 120) return tag;
			if(TMath::Abs(em->jeteta[i]) > 1.6) return tag;

			//other selections
			tag.addTag(jetType::inclJet);
			if(em->ndisc_csvV2[i] > 0.9){
				tag.addTag(jetType::negTagJet);
			}
			if(em->disc_csvV2[i] > 0.9) tag.addTag(jetType::taggedJet);
			if(em->isMC) if(TMath::Abs(em->matchedHadronFlavor[i]) == 5){
				tag.addTag(jetType::trueBJet);
			}
			if(em->isMC) if(TMath::Abs(em->matchedHadronFlavor[i]) == 5){
				tag.addTag(jetType::cjet);
			}
			return tag;
		}

		xTagger tagGenJet(eventMap *em, int i){
			xTagger tag;
			if( em->genjetpt[i] < 120 ) return tag;
			if( TMath::Abs(em->genjet_wta_eta[i]) > 1.6 ) return tag;

			//add incl jet tag
			tag.addTag(jetType::inclJet);

			//add true b jet tag
			int index = -1;
			float ptdiff = 20, drmin=1;
			for(int j=0; j<em->nJet(); ++j){
				float dr = toolkit::findDr(em->genjeteta[i],em->genjetphi[i],em->jeteta[j],em->jetphi[j]);
				if(dr > 0.4) continue;
				if(dr < drmin && fabs(em->genjetpt[i]-em->jetpt[j]) > ptdiff ) {
					drmin = dr; index = j;
				}
			}
			//int index = em->genMatchIndex[i];
			if(index > -1){
				if(TMath::Abs(em->matchedHadronFlavor[index]) == 5)
					tag.addTag(jetType::trueBJet);
				if(em->disc_csvV2[index] > 0.9)
					tag.addTag(jetType::taggedJet);
				if(em->ndisc_csvV2[index] > 0.9){
					tag.addTag(jetType::negTagJet);
				}
				if(TMath::Abs(em->matchedHadronFlavor[index]) == 4)
					tag.addTag(jetType::cjet);
			}
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
			if(em->checkEventFilter()) return 1;
			if(TMath::Abs(em->vz) > 15) return 1;
			if(em->isMC) if( em->pthat < 50) return 1;
			return 0;
		}
	};

	class weight_Hydjet_c5shift{
		public :
			weight_Hydjet_c5shift(){
				fvzw   = new TF1("fvzw", "pol5", -15, 15);
				fcentw = new TF1("fcentw", "pol7",0, 180);
				fvzw->SetParameters(1.00402,-0.0193632,0.000837705,-2.68852e-05,-4.17499e-06,1.22989e-07);
				fcentw->SetParameters(3.20695,-0.0579797,0.000905921,-1.64399e-05,1.86091e-07,-1.1416e-09, 3.5897e-12, -4.56036e-15);
			}
			~weight_Hydjet_c5shift(){}
			float evtWeight(eventMap* e){return (e->weight)*(fvzw->Eval(e->vz))*(fcentw->Eval(e->hiBin-10));}
			float trkWeight(eventMap* e, int i, xTagger &tag){return 1;}
			float recoJetWeight(eventMap* e, int i, xTagger &tag){return 1;}
			float genJetWeight(eventMap* e, int i, xTagger &tag){return 1;}
			float genParticleWeight(eventMap* e, int i, xTagger &tag){return 1;}

			TF1 * fvzw;
			TF1 * fcentw;
	};

}
