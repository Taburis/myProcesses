
//skim from hiforest to skim.root
//
#ifndef PRODUCERSKIMER_H
#define PRODUCERSKIMER_H
#include "myProcesses/liteFrame/plugin/liteFrame.h"
#include "myProcesses/hiforest/plugin/eventMap_hiForest.h"


template<typename event, typename config>
class producerSkimer : public producerBase<event, config>{
	public :
		producerSkimer(const char * name) : producerBase<event, config>(name)
	{}
		~producerSkimer(){}

		bool linkFrame(liteFrame<event, config> * frame){
			this->evt->isLegecy = 1;
			frame->doGenParticle = this->_cfg->ps->isMC;
			return 0;
		}
		void run(){
			//int jcent = this->_frame->getCentIndex();
			//if(jcent<0) return;
			//float weight = this->getEvtWeight();
			//for(int i=0; i<this->evt->nJet(); ++i){
			//}
			otree->Fill();
		}

		void beginJob(){
cout<<"here"<<endl;
			this->evt->loadJet(this->_cfg->ps->jetSetName);
			//of = TFile::Open(output, "recreate");
			otree = new TTree("mixing_tree", "");
			auto evt= this->evt;
			otree->Branch("vz",&(evt->vz));
			if(evt->isMC){
				otree->Branch("weight",&(evt->weight));
				otree->Branch("pthat",&(evt->pthat));
				otree->Branch("pt",  &evt->gpptp);
				otree->Branch("eta", &evt->gpetap);
				otree->Branch("phi", &evt->gpphip);
				otree->Branch("chg", &evt->gpchgp);
				otree->Branch("pdg", &evt->gppdgIDp);
				otree->Branch("nref" ,  &evt->njet);
				otree->Branch("ngj",    &evt->ngj);
				otree->Branch("geneta", &(evt->genjeteta)     ,"genjteta[ngj]/F");
				otree->Branch("genphi", &(evt->genjetphi)     ,"genjtphi[ngj]/F");
				otree->Branch("genpt" , &(evt->genjetpt)      ,"genjtpt[ngj]/F");
				otree->Branch("matchedHadronFlavor", &(evt->matchedHadronFlavor),"matchedHadronFlavor[nref]/I");
				otree->Branch("matchedPartonFlavor", &(evt->flavor_forb),"matchedPartonFlavor[nref]/I");
				otree->Branch("refpt",  &(evt->ref_jetpt) ,"refpt[nref]/F");
			}
			otree->Branch("jteta",  &(evt->jeteta),"jteta[nref]/F");
			otree->Branch("jtphi",  &(evt->jetphi),"jtphi[nref]/F");
			otree->Branch("jtpt" ,  &(evt->jetpt ),"jtpt[nref]/F");
		}
		void endJob(){
			otree->Write();
		}

		TString output;
		TTree* otree;
		//		event *evt;
		TFile *of;
		bool ismc, isHI;
};

#endif
