

#include "myProcesses/liteFrame/plugin/toolkit.h"
#include "myProcesses/liteFrame/plugin/liteFrame.h"

#if defined(event_content_skim)
#include "myProcesses/hiforest/plugin/eventMap_skim.h"
#elif defined(event_content_forest)
#include "myProcesses/hiforest/plugin/eventMap_hiForest.h"
#endif


namespace config_AN20029{

	int centb_data[] = {0, 60, 180};
	int ncentbin = 2;

	enum jetType {inclJet, trueBJet, taggedJet, negTagJet, cjet};
	enum trkType {inclTrk, sube0,suben0};

	class PSet_AN20029_v0 : public PSetBase{
		public :
			PSet_AN20029_v0()
			{
				this->ncent = ncentbin;
				this->centbin = centb_data;
				this->centLabel = toolkit::makeLabels(ncent, centbin);
				this->jetSetName = "akFlowPuCs4PFJetAnalyzer";
			}
			~PSet_AN20029_v0(){}
	};

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
		if(em->isMC) if(TMath::Abs(em->flavor_forb[i]) == 5){
			tag.addTag(jetType::trueBJet);
		}
		if(em->isMC) if(TMath::Abs(em->flavor_forb[i]) == 5){
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
			if(TMath::Abs(em->flavor_forb[index]) == 5)
				tag.addTag(jetType::trueBJet);
			if(em->disc_csvV2[index] > 0.9)
				tag.addTag(jetType::taggedJet);
			if(em->ndisc_csvV2[index] > 0.9){
				tag.addTag(jetType::negTagJet);
			}
			if(TMath::Abs(em->flavor_forb[index]) == 4)
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

	class selections : public funcMapBase{
		public:
			selections(){
				tagGenJet=tagGenJet;
				tagRecoJet = tagRecoJet;;
				tagTrk = tagTrk;
				tagGenParticle = tagGenParticle;

				evtWeight = dummy_weight_evt;
				recoJetWeight = dummy_weight_branch;
				genJetWeight = dummy_weight_branch;
				trkWeight = dummy_weight_branch;
				genParticleWeight = dummy_weight_branch;
			}
			~selections(){};
	};
}
