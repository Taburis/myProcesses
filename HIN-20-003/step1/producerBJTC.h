
#ifndef PRODUCERBJTC_H
#define PRODUCERBJTC_H

#include "myProcesses/liteFrame/plugin/producerJTC.C"
//#include "myProcesses/jtc/plugin/jtcUti.h"
#include "myProcesses/jtc/JEC2018PbPb/JECorr.h"
#include "myProcesses/jtc/JEC2018PbPb/JECUncert.h"
#include <vector>
#include "TRandom.h"

//enum jetType {inclJet, trueBJet, taggedJet, negTagJet, cJet};
//enum trkType {inclTrk, sube0,suben0};

template<typename event, typename config>
class producerBJTC: public producerJTC<event, config>{
	public : producerBJTC(const char *name):producerJTC<event, config>(name){}
		 ~producerBJTC(){}

		 //		 virtual bool evtCut(event *em) override {
		 //			 if(em->checkEventFilter()) return 1;
		 //			 if(TMath::Abs(em->vz) > 15) return 1;
		 //			 if(isMC) if( em->pthat < 50) return 1;
		 //			 //if(isMC) if( em->pthat < 40) return 1;
		 //			 return 0;
		 //		 }
		 virtual bool mixEvtCut(event *evt) override {
			 if( this->_cfg->src->evtCut(evt) ) return 1;
			 if(evt->isMC) if(evt->pthat > 50) return 1;
			 return 0;
		 }
		 void addJtcSetForSube(TString name, xTagger jetTg, bool domix){
			 this->addJetQASet(name, jetTg);
			 this->addJtcSet(name+"_sube0_RecoJet_GenTrk",name+"_RecoJet_GenTrk", jetTg, 1, sube0TrkTg, 0,domix);
			 this->addJtcSet(name+"_sube0_GenJet_GenTrk" ,name+"_GenJet_GenTrk" , jetTg, 0, sube0TrkTg, 0,domix);
			 this->addJtcSet(name+"_subeN0_RecoJet_GenTrk",name+"_RecoJet_GenTrk", jetTg, 1, subeNTrkTg, 0, domix);
			 this->addJtcSet(name+"_subeN0_GenJet_GenTrk" ,name+"_GenJet_GenTrk" , jetTg, 0, subeNTrkTg, 0, domix);
			 //addJtcSet(name+"_GenJet_GenTrk" , name+"_GenJet_GenTrk"  , jetTg, 0, inclTrkTg, 0, domix);
			 //addJtcSet(name+"_RecoJet_GenTrk" ,name+"_RecoJet_GenTrk" , jetTg, 1, inclTrkTg, 0, domix);
			 //addJtcSet(name+"_RecoJet_RecoTrk",name+"_RecoJet_RecoTrk", jetTg, 1, inclTrkTg, 1, domix);
		 }
		 virtual void beginJob() override {
			 this->evt ->loadTrack();
			 if(this->_cfg->ps->isMC) this->evt ->loadGenParticle();
			 this->evt ->loadJet(this->_cfg->ps->jetSetName);
			 this->add_evtInfo_hist();
			 inclJtTg   .addTag(jetType::inclJet);
			 trueBJtTg  .addTag(jetType::trueBJet);
			 taggedJtTg .addTag(jetType::taggedJet);
			 tagTrueJtTg.addTag(jetType::taggedJet); 
			 tagTrueJtTg.addTag(jetType::trueBJet);
			 inclTrkTg  .addTag(trkType::inclTrk);
			 negTagJtTg .addTag(jetType::negTagJet);
			 cJtTg      .addTag(jetType::cJet);
			 sube0TrkTg .addTag(trkType::sube0);
			 subeNTrkTg .addTag(trkType::suben0);

			 contJtTg.addTag(jetType::contJet);
			 //			 if(dosube) domixing = 0;
			 if(dosube && this->_cfg->ps->isMC){
				 //addJtcSetForSube("incl", inclJtTg, this->domixing);
				 //addJtcSetForSube("tagged", taggedJtTg, this->domixing);
				 addJtcSetForSube("negTag", negTagJtTg, this->domixing);
				 addJtcSetForSube("tagTrue", tagTrueJtTg,this->domixing);
				 addJtcSetForSube("trueB" , trueBJtTg, this->domixing);
				 //addJtcSetForSube("cont" , contJtTg, this->domixing);
			 }else{
				 this->addJtcSet("incl"  , inclJtTg, inclTrkTg);
				 this->addJtcSet("tagged", taggedJtTg, inclTrkTg);
				 //this->addJtcSet("negTag", negTagJtTg, inclTrkTg);
				 if(this->_cfg->ps->isMC){
				 this->addJtcSet("tagTrue", tagTrueJtTg, inclTrkTg);
				 this->addJtcSet("trueB" , trueBJtTg, inclTrkTg);
				 this->addJtcSet("cont" , contJtTg, inclTrkTg);
				 }
			 }
		 };

		 virtual void endJob() override {}

		 double get_pt_JEU(event *evt, int ijet){
			 float pt = evt->jetpt[ijet];
			 JEU.SetJetPT(evt->jetpt[ijet]);
			 JEU.SetJetEta(evt->jeteta[ijet]);
			 JEU.SetJetPhi(evt->jetphi[ijet]);
			 if(jecDown) return pt*(1-JEU.GetUncertainty().first);
			 else return pt*(1+JEU.GetUncertainty().second);
		 }

		 double get_correctedPt(event *evt, int ijet){
			 JEC.SetJetPT (evt->jetpt[ijet]);
			 JEC.SetJetEta(evt->jeteta[ijet]);
			 JEC.SetJetPhi(evt->jetphi[ijet]);
			 return JEC.GetCorrectedPT();
		 }

		 void loadJEC(){
			 jecFiles.emplace_back("myProcesses/jtc/JEC2018PbPb/Autumn18_HI_V6_DATA_L2Relative_AK4PF.txt");
			 jecFiles.emplace_back("myProcesses/jtc/JEC2018PbPb/Autumn18_HI_V6_DATA_L2L3Residual_AK4PF.txt");
			 addJEC = 1;
			 JEC.Initialize(jecFiles);
		 }
		 void loadJEU(){
			 doJEU=1;
			 JEU.Initialize("myProcesses/jtc/JEC2018PbPb/Autumn18_HI_V6_DATA_Uncertainty_AK4PF.txt");
		 }
		 void setJEU_Up(){ loadJEU(); jecUp=1; jecDown=0;}
		 void setJEU_Down(){ loadJEU(); jecUp=0; jecDown=1;}

		 xTagger sube0TrkTg, subeNTrkTg;
		 xTagger inclJtTg, trueBJtTg, taggedJtTg, negTagJtTg, tagTrueJtTg, inclTrkTg, cJtTg, contJtTg;

		 //histCase inclCase, trueBCase;
		 bool  dosube=0, jecUp=0, jecDown=0, doJEU=0;
		 float csv_cut = 0.9;
		 float jtpt_min = 120.0, jteta_max = 1.6;
		 bool addJEC = 0, doJERSmear = 0;
		 TRandom random;
		 JetCorrector JEC;
		 JetUncertainty JEU;
		 std::vector<string> jecFiles;
};

#endif
