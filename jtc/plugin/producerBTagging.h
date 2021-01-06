
#ifndef PRODUCERBTAGGING_H
#define PRODUCERBTAGGING_H
#include "myProcesses/liteFrame/plugin/liteFrame.h"
#include "myProcesses/jtc/plugin/QATools.h"


namespace qaconvention{
	Double_t ptbin[21] = {110, 120, 136, 152, 168, 184, 200, 216, 232, 248, 264, 280, 296, 312, 328, 344, 360, 380, 400, 432, 500};

	Double_t neg_ptbin[6] = {120, 130, 150, 200, 250, 500};
}

template<typename event, typename config>
class producerBTagging : public producerBase<event, config>{
	enum bTaggingJetType{
		inclJet, negTaggedJet, taggedJet, bJet, cJet, lJet, gspJet
	};
	public :
	producerBTagging(const char * name) : producerBase<event, config>(name)
	{}
	~producerBTagging(){}

	bool linkFrame(liteFrame<event, config> * frame){
		return 0;
	}
	void run(){
		int jcent = this->_frame->getCentIndex();
		if(jcent<0) return;
		float weight = this->getEvtWeight();
		for(int i=0; i<this->evt->nJet(); ++i){
			xTagger tag = tagRecoJet(this->evt, i);
			if(tag.tag == 0) continue;
			int flavor = 0;
			if(ismc){
				if(tag.select(1<<bTaggingJetType::lJet)) flavor = 1; 
				else if(tag.select(1<<bTaggingJetType::cJet)) flavor = 2; 
				else if(tag.select(1<<bTaggingJetType::bJet)) flavor = 3; 
			}
			qaPt ->fill(tag, this->evt->hiBin, this->evt->jetpt[i], weight, flavor);
			qaEta->fill(tag, this->evt->hiBin, this->evt->jeteta[i], weight, flavor);
			qaPhi->fill(tag, this->evt->hiBin, this->evt->jetphi[i], weight, flavor);
			qaTagger->fill(tag, this->evt->hiBin, this->evt->disc_csvV2[i], weight, flavor);
			qaNTagger->fill(tag, this->evt->hiBin, this->evt->ndisc_csvV2[i], weight, flavor);
			pt_neg->fill(tag, this->evt->hiBin, this->evt->jetpt[i], weight, flavor);
			pt_all->fill(tag, this->evt->hiBin, this->evt->jetpt[i], weight, flavor);

			bJetSet->fill(bjetpt , tag, this->evt->hiBin, this->evt->jetpt [i], weight);
			bJetSet->fill(bjeteta, tag, this->evt->hiBin, this->evt->jeteta[i], weight);
			bJetSet->fill(bjetphi, tag, this->evt->hiBin, this->evt->jetphi[i], weight);
			// GSP reweighting
			//if(tag.select(1<<bTaggingJetType::gspJet)) weight = 2*weight;
			gspJetSet->fill(gsppt , tag, this->evt->hiBin, this->evt->jetpt [i], weight);
			gspJetSet->fill(gspeta, tag, this->evt->hiBin, this->evt->jeteta[i], weight);
			gspJetSet->fill(gspphi, tag, this->evt->hiBin, this->evt->jetphi[i], weight);
		}
	}

	void beginJob(){
		this->evt->loadJet(this->_cfg->ps->jetSetName);

		TString dir = "bTaggingQA";
		ismc = this->_cfg->ps->isMC;
		//qaPt  = new qaTSetFlavor<TH1D>(dir+"/jetpt" ,  "jetpt", this->hm, bTaggingJetType::inclJet, this->_cfg->ps->ncent, this->_cfg->ps->centbin, 20, 100, 200,ismc);
		qaPt  = new qaTSetFlavor<TH1D>(dir+"/jetpt" ,  "jetpt", this->hm, bTaggingJetType::inclJet, this->_cfg->ps->ncent, this->_cfg->ps->centbin, 20, qaconvention::ptbin, ismc);
		qaEta = new qaTSetFlavor<TH1D>(dir+"/jeteta", "jeteta", this->hm, bTaggingJetType::inclJet, this->_cfg->ps->ncent, this->_cfg->ps->centbin, 50, -2, 2, ismc);
		qaPhi = new qaTSetFlavor<TH1D>(dir+"/jetphi", "jetphi", this->hm, bTaggingJetType::inclJet, this->_cfg->ps->ncent, this->_cfg->ps->centbin, 36, -TMath::Pi(), TMath::Pi(), ismc);
		qaTagger = new qaTSetFlavor<TH1D>(dir+"/tagger", "tagger value", this->hm, bTaggingJetType::inclJet, this->_cfg->ps->ncent, this->_cfg->ps->centbin, 50, 0, 1, ismc);
		qaNTagger = new qaTSetFlavor<TH1D>(dir+"/nTagger", "negative tagger value", this->hm, bTaggingJetType::inclJet, this->_cfg->ps->ncent, this->_cfg->ps->centbin, 50, 0, 1, ismc);

		pt_neg = new qaTSetFlavor<TH1D>(dir+"/jetpt_negTag", "jet spectra (neg. tagged)", this->hm, bTaggingJetType::negTaggedJet, this->_cfg->ps->ncent, this->_cfg->ps->centbin, 5, qaconvention::neg_ptbin, ismc);
		pt_all = new qaTSetFlavor<TH1D>(dir+"/jetpt_all", "jet spectra (all)", this->hm, bTaggingJetType::inclJet, this->_cfg->ps->ncent, this->_cfg->ps->centbin, 5, qaconvention::neg_ptbin, ismc);
		gspJetSet = new xTSetBase("bTaggingSet", this->hm, bTaggingJetType::gspJet, this->_cfg->ps->ncent, this->_cfg->ps->centbin);
		bJetSet = new xTSetBase("bTaggingSet", this->hm, bTaggingJetType::bJet, this->_cfg->ps->ncent, this->_cfg->ps->centbin);
		gsppt  = gspJetSet->regHist<TH1D>("gspPt" ,"GSP jets",  20, qaconvention::ptbin);
		gspeta = gspJetSet->regHist<TH1D>("gspEta","GSP jets",  50, -2, 2);
		gspphi = gspJetSet->regHist<TH1D>("gspPhi","GSP jets",  36, -TMath::Pi(), TMath::Pi());
		bjetpt  = bJetSet->regHist<TH1D>("bjetPt" ,"b jets",  20, qaconvention::ptbin);
		bjeteta = bJetSet->regHist<TH1D>("bjetEta","b jets",  50, -2, 2);
		bjetphi = bJetSet->regHist<TH1D>("bjetPhi","b jets",  36, -TMath::Pi(), TMath::Pi());
	}
	void endJob(){}

	xTagger tagGenJet(event *em, int i){
		xTagger tag;
		if( em->genjetpt[i] < jetptmin ) return tag;
		if( TMath::Abs(em->genjet_wta_eta[i]) > jetetamax) return tag;

		//add incl jet tag
		tag.addTag(bTaggingJetType::inclJet);

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
			if(TMath::Abs(em->matchedHadronFlavor[index]) == 5){
				tag.addTag(bTaggingJetType::bJet);
				if(em->bHadronNumber[index] == 2) tag.addTag(bTaggingJetType::gspJet);
			} else if(TMath::Abs(em->matchedHadronFlavor[index]) == 4)
				tag.addTag(bTaggingJetType::cJet);
			else tag.addTag(bTaggingJetType::lJet);
			if(em->disc_csvV2[index] > csv_point){
				tag.addTag(bTaggingJetType::taggedJet);
			}
			if(em->ndisc_csvV2[index] > csv_point){
				tag.addTag(bTaggingJetType::negTaggedJet);
			}
		}
		return tag;
	}

	xTagger tagRecoJet(eventMap *em, int i){
		xTagger tag;
		//kinematic selection:
		float jetpt = em->jetpt[i];
		//	if( jetpt < jetptmin) return tag;
		//	if(TMath::Abs(em->jeteta[i]) > jetetamax) return tag;

		//other selections
		tag.addTag(bTaggingJetType::inclJet);
		if(em->ndisc_csvV2[i] > csv_point){
			tag.addTag(bTaggingJetType::negTaggedJet);
		}
		if(em->disc_csvV2[i] > csv_point){
			tag.addTag(bTaggingJetType::taggedJet);
		}
		if(em->isMC){
			if(TMath::Abs(em->matchedHadronFlavor[i]) == 5){
				tag.addTag(bTaggingJetType::bJet);
				if(em->bHadronNumber[i] == 2) tag.addTag(bTaggingJetType::gspJet);
			}
			else if(TMath::Abs(em->matchedHadronFlavor[i]) == 4){
				tag.addTag(bTaggingJetType::cJet);
			}
			else tag.addTag(bTaggingJetType::lJet);
		}
		return tag;
	}

	bool default_selection = 1;
	qaTSetFlavor<TH1D> *qaPt, *qaEta, *qaPhi, *qaTagger, *qaNTagger;
	qaTSetFlavor<TH1D> *pt_neg, *pt_all; //for the scale factor study
	float csv_point = 0.9, jetptmin = 120, jetetamax = 1.6;
	bool ismc;
	xTSetBase *bJetSet, *gspJetSet;
	TH1D **gsppt, **gspeta, **gspphi;
	TH1D **bjetpt, **bjeteta, **bjetphi;
};

#endif
