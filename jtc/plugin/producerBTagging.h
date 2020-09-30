
#ifndef PRODUCERBTAGGING_H
#define PRODUCERBTAGGING_H
#include "myProcesses/liteFrame/plugin/liteFrame.h"
#include "myProcesses/jtc/plugin/QATools.h"

template<typename event, typename config>
class producerBTagging : public producerBase<event, config>{
	enum jetType{
		inclJet, negTagJet, lJet, cJet, bJet, lJet_tag, cJet_tag, bJet_tag
	};
	public :
	producerBTagging(const char * name) : producerBase<event, config>(name)
	{
	}
	~producerBTagging(){}

	bool linkFrame(liteFrame<event, config> * frame){
		frame->doJet=1;	return 0;
	}
	bool initialCheck(){return 0;}
	void run(){
		int jcent = this->_frame->getCentIndex();
		if(jcent<0) return;
		float weight = this->getEvtWeight();
		for(int i=0; i<this->evt->nJet(); ++i){
			xTagger tag = tagRecoJet(this->evt, i);
//			if(default_selection) tag = tagRecoJet(this->evt, i);
//			else this->_cfg->src->tagRecoJet(this->evt,i);
			if(tag.tag == 0) continue;
			for(auto & it : jetSets){
				it->fillHist(tag,jcent,this->evt->jetpt[i],this->evt->jeteta[i],this->evt->jetphi[i],weight);
			}
		}

	}
	void beginJob(){
		ljs = new jetQASet("bTagging", "udsgJet", jetType::lJet, this->_cfg->ps->ncent, this->hm);
		cjs = new jetQASet("bTagging", "cJet", jetType::cJet, this->_cfg->ps->ncent, this->hm);
		bjs = new jetQASet("bTagging", "bJet", jetType::bJet, this->_cfg->ps->ncent, this->hm);
		ljs_tag = new jetQASet("bTagging", "udsgJetTag", jetType::lJet_tag, this->_cfg->ps->ncent, this->hm);
		cjs_tag = new jetQASet("bTagging", "cJetTag", jetType::cJet_tag, this->_cfg->ps->ncent, this->hm);
		bjs_tag = new jetQASet("bTagging", "bJetTag", jetType::bJet_tag, this->_cfg->ps->ncent, this->hm);
		jetSets.emplace_back(ljs);
		jetSets.emplace_back(cjs);
		jetSets.emplace_back(bjs);
		jetSets.emplace_back(ljs_tag);
		jetSets.emplace_back(cjs_tag);
		jetSets.emplace_back(bjs_tag);
	}
	void endJob(){}

	xTagger tagGenJet(eventMap *em, int i){
		xTagger tag;
		if( em->genjetpt[i] < jetptmin ) return tag;
		if( TMath::Abs(em->genjet_wta_eta[i]) > jetetamax) return tag;

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
				tag.addTag(jetType::bJet);
			else if(TMath::Abs(em->flavor_forb[index]) == 4)
				tag.addTag(jetType::cJet);
			else tag.addTag(jetType::lJet);
			if(em->disc_csvV2[index] > csv_point){
				if(TMath::Abs(em->flavor_forb[index]) == 5)
					tag.addTag(jetType::bJet_tag);
				else if(TMath::Abs(em->flavor_forb[index]) == 4)
					tag.addTag(jetType::cJet_tag);
				else tag.addTag(jetType::lJet_tag);
			}
			if(em->ndisc_csvV2[index] > csv_point){
				tag.addTag(jetType::negTagJet);
			}
		}
		return tag;
	}

	xTagger tagRecoJet(eventMap *em, int i){
		xTagger tag;
		//kinematic selection:
		float jetpt = em->jetpt[i];
		if( jetpt < jetptmin) return tag;
		if(TMath::Abs(em->jeteta[i]) > jetetamax) return tag;

		//other selections
		tag.addTag(jetType::inclJet);
		if(em->ndisc_csvV2[i] > csv_point){
			tag.addTag(jetType::negTagJet);
		}
		if(em->isMC){
			if(TMath::Abs(em->flavor_forb[i]) == 5)
				tag.addTag(jetType::bJet);
			else if(TMath::Abs(em->flavor_forb[i]) == 4)
				tag.addTag(jetType::cJet);
			else tag.addTag(jetType::lJet);
			if(em->disc_csvV2[i] > csv_point){
				if(TMath::Abs(em->flavor_forb[i]) == 5)
					tag.addTag(jetType::bJet_tag);
				else if(TMath::Abs(em->flavor_forb[i]) == 4)
					tag.addTag(jetType::cJet_tag);
				else tag.addTag(jetType::lJet_tag);
			}
		}
		return tag;
	}

	bool default_selection = 1;
	float csv_point = 0.9, jetptmin = 120, jetetamax = 1.6;
	jetQASet *ljs, *cjs, *bjs, *ljs_tag, *cjs_tag, *bjs_tag;
	std::vector<jetQASet*> jetSets;
};

#endif
