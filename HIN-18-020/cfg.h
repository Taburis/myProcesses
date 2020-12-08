
#include "myProcesses/liteFrame/plugin/toolkit.h"
#include "myProcesses/liteFrame/plugin/liteFrame.h"
#include "myProcesses/jtc/plugin/jtcLib.h"
//#include "myProcesses/jtc/plugin/producerBJTC.h"

#if defined(event_content_skim)
#include "myProcesses/hiforest/plugin/eventMap_skim.h"
#elif defined(event_content_forest)
#include "myProcesses/hiforest/plugin/eventMap_hiForest.h"
#endif

#include "TF1.h"

enum jetType {inclJet, trueBJet, taggedJet, negTagJet, cJet, lightJet, contJet, gspJet};
enum trkType {inclTrk, sube0,suben0};

namespace config_HIN18020{


	class pset_nominal_forest{
		public :
			pset_nominal_forest():
				centbin {-200, 200},
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
			int nfilters=0;
			int ntrigs =0;
			std::string *trigger;
	};


	class selections {	
		public : selections(){}
			 ~selections(){
			 }
			 xTagger tagRecoJet(eventMap *em, int i){
				 xTagger tag;
				 //kinematic selection:
				 float jetpt = em->jetpt[i];
				 if(jeutool.smearSigma >0 ) jetpt = jeutool.smearedPt(jetpt);
				 if( jetpt < 120) return tag;
				 if(TMath::Abs(em->jeteta[i]) > 1.6) return tag;

				 //other selections
				 tag.addTag(jetType::inclJet);
				 if(em->ndisc_csvV2[i] > 0.9){
					 tag.addTag(jetType::negTagJet);
				 }
				 if(em->disc_csvV2[i] > 0.9) tag.addTag(jetType::taggedJet);
				 if(em->isMC){ 
					 if(TMath::Abs(em->matchedHadronFlavor[i]) == 5){
						 tag.addTag(jetType::trueBJet);
						 if(em->bHadronNumber[i] ==2) 
							 tag.addTag(jetType::gspJet);
					 }else if(TMath::Abs(em->matchedHadronFlavor[i]) == 4)
						 tag.addTag(jetType::cJet);
					 else tag.addTag(jetType::lightJet);
					 if(TMath::Abs(em->matchedHadronFlavor[i]) != 5 && em->disc_csvV2[i] > 0.9) tag.addTag(jetType::contJet);
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
				 float drmin=1;
				 for(int j=0; j<em->nJet(); ++j){
					 float dr = toolkit::findDr(em->genjeteta[i],em->genjetphi[i],em->jeteta[j],em->jetphi[j]);
					 if(dr > 0.4) continue;
					 if(dr < drmin ) {
						 drmin = dr; index = j;
					 }
				 }
				 //int index = em->genMatchIndex[i];
				 if(index > -1){
					 if(em->disc_csvV2[index] > 0.9)
						 tag.addTag(jetType::taggedJet);
					 if(em->ndisc_csvV2[index] > 0.9){
						 tag.addTag(jetType::negTagJet);
					 }
					 if(TMath::Abs(em->matchedHadronFlavor[index]) == 5){
						 tag.addTag(jetType::trueBJet);
						 if(em->bHadronNumber[i] ==2) 
							 tag.addTag(jetType::gspJet);
					 }else if(TMath::Abs(em->matchedHadronFlavor[index]) == 4)
						 tag.addTag(jetType::cJet);
					 else tag.addTag(jetType::lightJet);
					 if(TMath::Abs(em->matchedHadronFlavor[index]) != 5 && em->disc_csvV2[index] > 0.9) tag.addTag(jetType::contJet);
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
				 return tag;
			 }

			 bool evtCut(eventMap *em){
				 if(em->checkEventFilter()) return 1;
				 if(TMath::Abs(em->vz) > 15) return 1;
				 //if(em->hiBin > 180) return 1;
				 //if(em->isMC) if( em->pthat < 50) return 1;
				 return 0;
			 }
			
			jtc::JEUncertTool jeutool;
			float jetptmin = 120;
	};

	class weight_dummy{
		public :

			weight_dummy(){
			}
			~weight_dummy(){}
			float evtWeight(eventMap* e){return 1;}
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
