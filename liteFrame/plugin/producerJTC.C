
#include <cstdarg>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <vector>
#include "TMath.h"
#include "TFile.h"
#include "TRandom.h"
#include "TBenchmark.h"
#include "myProcesses/liteFrame/plugin/producerJTC.h"
#include <map>

class candidate{
	public : 
		candidate(){};
		candidate(xTagger tg, bool rec, float pt0,float eta0,float phi0, float w= 1){ 
			set(tg, rec,pt0,eta0,phi0,w);
		}
		void copy(candidate &c){
			pt=c.pt;eta=c.eta;phi=c.phi;weight=c.weight;tag=c.tag;isValid = c.isValid;
		}
		candidate & operator = (candidate &rhs){
			this->copy(rhs);
			return (*this);
		}
		~candidate(){};
		void set(xTagger &tg, bool rec, float pt0,float eta0,float phi0, float w= 1){
			pt=pt0; 
			eta=eta0;
			phi=phi0;
			tag= tg;
			isReco= rec;
			weight=w;
			isValid = 1;
		}
		void clear(){isValid = 0;}

		float pt, eta, phi, weight;
		xTagger tag;
		bool isValid= 0, isReco = 0;
};

template<typename event, typename config>
producerJTC<event, config>::~producerJTC(){ 
	if(domixing){
		for(int i=0; i<nvz_mix;++i){
			for(int j=0; j<ncent_mix; ++j){
				std::vector<unsigned int>().swap(*(mixTable[i+j*nvz_mix]));
			}
		}
		delete mixTable;
	}
}

//template<typename event, typename config>
//bool producerJTC<event, config>::recoTrkCuts(event *em, int j){
//	//return 1 to skip
//	if(em->trkpt[j] < 1 || em->trkpt[j]>400) return 1;
//	if(TMath::Abs(em->trketa[j]) >= 2.4) return 1;
//	if(!(em->highPurity[j])) return 1;
//	if(em->trknhit[j]< 11) return 1;
//	if(em->trkchi2[j]/em->trkndof[j]/em->trknlayer[j] > 0.15) return 1;
//	float et = (em->pfEcal[j] + em->pfHcal[j])/TMath::CosH(em->trketa[j]);
//	if( !(em->trkpt[j]<20.0 || et>0.5*(em->trkpt[j]))) return 1;
//	return 0;	
//}
//template<typename event, typename config>
//bool producerJTC<event, config>::recoJtCuts(event *em, int j){
//	//return 1 to skip
//	if(em->jetpt[j] < 120.0) return 1;
//	//		if(em->ref_jetpt[j] < 50.0) return 1;
//	if(TMath::Abs(em->jeteta[j]) > 1.6) return 1;
//	return 0;
//}
//template<typename event, typename config>
//bool producerJTC<event, config>::genParticleCuts(event *em, int j){
//	//return 1 to skip
//	if(em->gppt(j) < 1.0) return 1;
//	if(TMath::Abs(em->gpeta(j)) > 2.4) return 1;
//	if(em->gpchg(j)==0) return 1;
//	return 0;
//}
//template<typename event, typename config>
//bool producerJTC<event, config>::genJetCuts(event * em, int j){
//	//return 1 to skip
//	if( em->genjetpt[j] < 120 ) return 1;
//	if( TMath::Abs(em->genjet_wta_eta[j]) > 1.6 ) return 1;
//	return 0;
//}

template<typename event, typename config>
void producerJTC<event, config>::trkSelection(std::vector<candidate>&cands, event *em){
	cands.reserve(em->nTrk());
	for(int i=0; i< em->nTrk(); ++i){
		xTagger tag = this->_cfg->src->tagTrk(em, i);
		if(tag.tag==0) continue; // tag 0 means invalid
		float w = this->isMC ? this->_cfg->weight->trkWeight(em, i, tag) : 1;
		candidate cc(tag, 1, em->trkpt[i],em->trketa[i],em->trkphi[i],1);
		cands.emplace_back(cc);
	}
}

template<typename event, typename config>
void producerJTC<event, config>::genParticleSelection(std::vector<candidate>&cands, event *em){
	cands.reserve(em->nGP());
	for(int i=0; i< em->nGP(); ++i){
		xTagger tag = this->_cfg->src->tagGenParticle(em, i);
		if(tag.tag==0) continue; // tag 0 means invalid
		float w = this->_cfg->weight->genParticleWeight(em, i, tag);
		candidate cc(tag, 0, em->gppt(i),em->gpeta(i),em->gpphi(i),w);
		cands.emplace_back(cc);
	}
}


template<typename event, typename config>
void producerJTC<event, config>::recoJetSelection(std::vector<candidate>&cands, event *em){
	cands.reserve(em->nJet());
	for(int i=0; i< em->nJet(); ++i){
		xTagger tag = this->_cfg->src->tagRecoJet(em, i);
		if(tag.tag==0) continue; // tag 0 means invalid
		float w = this->isMC ? this->_cfg->weight->recoJetWeight(em, i, tag): 1;
		if(smearSigma >0) w = w*JESSmearing();
		candidate cc2(tag,1,em->jetpt[i], em->jet_wta_eta[i], em->jet_wta_phi[i], w);
		cands.emplace_back(cc2);
	}
}


template<typename event, typename config>
void producerJTC<event, config>::genJetSelection(std::vector<candidate>&cands, event *em){
	// if the jets have n types, need to add n times...
	cands.reserve(em->nGenJet());
	for(int i=0; i< em->nGenJet(); ++i){
		xTagger tag = this->_cfg->src->tagGenJet(em, i);
		if(tag.tag==0) continue; // tag 0 means invalid
		float w =this->_cfg->weight->genJetWeight(em, i, tag);
		candidate cc2(tag,0, em->genjetpt[i], em->genjet_wta_eta[i], em->genjet_wta_phi[i], w);
		cands.emplace_back(cc2);
		//-------------
	}
}

template<typename event, typename config>
void producerJTC<event, config>::addJtcSet(TString name, xTagger jetTg, xTagger trkTg, bool domixing){
	addJetQASet(name, jetTg);
	addJtcSet(name+"_RecoJet_RecoTrk", name+"_RecoJet_RecoTrk", jetTg, 1, trkTg, 1, domixing);
	if(this->_cfg->ps->isMC){
		addJtcSet(name+"_RecoJet_GenTrk",name+"_RecoJet_GenTrk", jetTg, 1, trkTg, 0,domixing);
		addJtcSet(name+"_GenJet_GenTrk" ,name+"_GenJet_GenTrk" , jetTg, 0, trkTg, 0,domixing);
	}
}

template<typename event, typename config>
void producerJTC<event, config>::addJtcSet(TString name, TString dir, xTagger jetTg, bool recoJt, xTagger trkTg, bool recoTk, bool domixing){
	int nHistoBinsX = 500;
	int nHistoBinsY = 200;
	jtcList.emplace_back(jtcSet());
	jtcSet & hc = jtcList.back();	
	hc.domixing = domixing;
	hc.jetTag = jetTg; hc.trkTag=trkTg;
	hc.isRecoJet = recoJt; hc.isRecoTrk=recoTk;
	//	hc.mix_trkmap=new TH2D*[nPt*nCent];
	hc.sig= new TH2D*[nPt*nCent];
	hc.sig_pTweighted= new TH2D*[nPt*nCent];
	if(domixing) hc.mixing= new TH2D*[nPt*nCent];

	name =dir+"/"+name;
	for(int j=0; j<nCent; ++j){
		for(int i=0; i<nPt; ++i){
			TString tmp = centLabel[j]+", "+ptLabel[i];
			hc.sig[i+j*nPt] = this->hm->template regHist<TH2D>(name+Form("_P%d_C%d",i, j), "signal: "+tmp,
					nHistoBinsX,-5,5,nHistoBinsY,-TMath::Pi()/2,3*TMath::Pi()/2);
			hc.sig_pTweighted[i+j*nPt] = this->hm->template regHist<TH2D>(name+Form("_pTweighted_P%d_C%d",i, j), "signal pTweighted: "+tmp,
					nHistoBinsX,-5,5, nHistoBinsY,-TMath::Pi()/2,3*TMath::Pi()/2);
			if(domixing){
				hc.mixing[i+j*nPt] = this->hm->template regHist<TH2D>(name+Form("_mixing_P%d_C%d",i, j), "mixing: "+tmp,
						nHistoBinsX,-5,5, nHistoBinsY,-TMath::Pi()/2,3*TMath::Pi()/2);
				//hc.mix_trkmap[i+j*nPt] = this->hm->regHist<TH2D>(name+Form("_mix_tkMap_P%d_C%d",i, j), "tkMap: "+tmp,
				//	50,-2.5,2.5, 50,-TMath::Pi(),TMath::Pi());
			}
		}
	}
}

template<typename event, typename config>
void producerJTC<event, config>::addJetQASet(TString name, xTagger jetTg){
	addJetQASet(name+"_RecoLevel", jetTg, 1);
	if(isMC) addJetQASet(name+"_GenLevel", jetTg, 0);
}

template<typename event, typename config>
void producerJTC<event, config>::addJetQASet(TString name, xTagger jetTg, bool recLev){
	const float newbin [21] = {110, 120, 136, 152, 168, 184, 200, 216, 232, 248, 264, 280, 296, 312, 328, 344, 360, 380, 400, 432, 500};
	int nbin = 20;
	name="jetQASets/"+name;
	jetQAs.emplace_back(jetQASet());
	jetQASet &hc = jetQAs.back();
	hc.jetTag=jetTg;
	hc.isReco=recLev;
	hc.jet_pt  = new TH1D*[nCent];
	hc.jet_eta = new TH1D*[nCent];
	hc.jet_phi = new TH1D*[nCent];
	for(int j=0; j<nCent; ++j){
		TString tmp = centLabel[j];
		hc.jet_pt[j]  = this->hm->template regHist<TH1D>(name+Form("_pt_C%d",j), tmp, nbin, newbin);
		hc.jet_eta[j] = this->hm->template regHist<TH1D>(name+Form("_eta_C%d",j), tmp, 100, -2.0, 2.0);
		hc.jet_phi[j] = this->hm->template regHist<TH1D>(name+Form("_phi_C%d",j), tmp, 72, -TMath::Pi(), TMath::Pi());
	}
}

template<typename event, typename config>
void producerJTC<event, config>::fillJetKinematic(std::vector<candidate>&jetCand, float evtW){
	for(unsigned int i = 0;i<jetCand.size(); i++){
		for(unsigned int k=0; k<jetQAs.size(); ++k){
			if(jetQAs[k].isReco != jetCand[i].isReco) continue;
			//if(jetQAs[k].jetTag.tag == 2) cout<<"tag: "<<jetQAs[k].jet_pt[centj]->GetName()<<": "<<jetCand[i].tag.tag<<": "<<jetQAs[k].jetTag.tag<<endl;
			if(!(jetCand[i].tag.select(jetQAs[k].jetTag))) continue;
			//if(jetQAs[k].jetTag.tag == 2) cout<<"passed"<<endl;
			jetQAs[k].jet_pt[centj]->Fill(jetCand [i].pt , (jetCand[i].weight)*evtW);
			jetQAs[k].jet_eta[centj]->Fill(jetCand[i].eta, (jetCand[i].weight)*evtW);
			jetQAs[k].jet_phi[centj]->Fill(jetCand[i].phi, (jetCand[i].weight)*evtW);
		}
	}
}


template<typename event, typename config>
void producerJTC<event, config>::fillHistCase(jtcSet &hc, candidate&jet, candidate&trk, float evtW, bool fillMix){
	int ptj = ptax->findBin(safeValue(trk.pt,ptbins[nPt]-1));
	//	cout<<"pt: "<<trk.pt<<" ; "<<ptj<<endl;
	float dphic=jet.phi-trk.phi;
	while(dphic>(1.5*TMath::Pi())){dphic+= -2*TMath::Pi();}
	while(dphic<(-0.5*TMath::Pi())){dphic+= 2*TMath::Pi();}

	float weightc = evtW*(jet.weight)*(trk.weight);
	float detac = jet.eta-trk.eta;
	//cout<<"filling: "<<ptj<<" : "<<detac<<" : "<<dphic<<", weight: "<<weightc<<endl;
	if(fillMix){
		hc.mixing[ptj+nPt*centj]->Fill(detac, dphic, weightc);
		//hc.mix_trkmap[ptj+nPt*centj]->Fill(trk.eta, trk.phi, weightc);
	}else {
		hc.sig[ptj+nPt*centj]->Fill(detac, dphic, weightc);
		hc.sig_pTweighted[ptj+nPt*centj]->Fill(detac, dphic, trk.pt*weightc);
	}
}

template<typename event, typename config>
bool producerJTC<event, config>::checkJtcPair(jtcSet &secl, candidate&jet,candidate&trk){
	return (jet.tag.select(secl.jetTag) && trk.tag.select(secl.trkTag));
}

template<typename event, typename config>
void producerJTC<event, config>::produce(std::vector<candidate>&jetCand, std::vector<candidate>&trkCand,float evtW, bool fillMix){
	for(unsigned int k=0; k<jtcList.size(); ++k){
		for(unsigned int j = 0;j<trkCand.size(); j++){
			for(unsigned int i = 0;i<jetCand.size(); i++){
				if(jtcList[k].isRecoJet!=jetCand[i].isReco) continue;
				if(jtcList[k].isRecoTrk!=trkCand[j].isReco) continue;
				//if(jtcList[k].jetTag.tag == 1) cout<<jtcList[k].sig[0]->GetName()<<"; jet tag: "<<jetCand[i].tag.tag<<"; "<<jetCand[i].isReco<<endl;
				if(fillMix && !(jtcList[k].domixing)) continue;
				if(checkJtcPair(jtcList[k], jetCand[i], trkCand[j])){
					//if(jtcList[k].jetTag.tag == 1) cout<<"pass"<<endl;
					fillHistCase(jtcList[k],jetCand[i], trkCand[j], evtW,fillMix);}
			}
		}
	}
}

template<typename event, typename config>
float producerJTC<event, config>::safeValue(float in, float max){
	//to prevent the overflow.
	if(in > max) return max;
	return in;
}

template<typename event, typename config>
void producerJTC<event, config>::fillEventInfo(float evtW){
	hvz->Fill(this->evt->vz, evtW);
	if(isMC) hpthat->Fill(safeValue(this->evt->pthat,399), evtW);
	if(!ispp)hcent->Fill(this->evt->hiBin, evtW);
}

template<typename event, typename config>
void producerJTC<event, config>::add_evtInfo_hist(){
	//this->hm = new histManager();
	ptax = new xAxis(nPt, ptbins);
	centax= new xAxis(nCent, centbins);
	hvz = this->hm->template regHist<TH1D>("vzInfo", "", 200, -20, 20);
	if(!ispp)hcent = this->hm->template regHist<TH1D>("centInfo","",  50, 0, 200);
	if(isMC) hpthat = this->hm->template regHist<TH1D>("pthatInfo", "", 100, 0, 400);
	if(doDvzDebug){hdvz = new TH2D*[nCent];
		for(int j=0; j<nCent; ++j){
			hdvz[j]= this->hm->template regHist<TH2D>(Form("mixing_dvz_C%d",j), "dvz "+centLabel[j], 60, -15, 15, 60, -1.5,1.5);
		}
	}
}

template<typename event, typename config>
void producerJTC<event, config>::write(std::string name){
	auto wf = TFile::Open(name.c_str(), "recreate");
	wf->cd();
	this->hm->write(wf);
	wf->Close();
}

//template<typename event, typename config>
//bool producerJTC<event, config>::evtCut(event *em){
//	if(em->checkEventFilter()) return 1;
//	if(TMath::Abs(em->vz) > 15) return 1;
//	//if(em->pthat < 50) return 1;
//	return 0;
//}

template<typename event, typename config>
bool producerJTC<event, config>::mixEvtCut(event *em){
	if(this->_cfg->src->evtCut(em)) return 1;
	//	if(isMC) if(em->pthat > 40) return 1;
	return 0;
}

template<typename event, typename config>
bool producerJTC<event, config>::quick_mixing_buff(){
	setup_mixingTable(nvz_mix, vzmin_mix, vzmax_mix, ncent_mix, hibinmin_mix, hibinmax_mix);
	if(scanMixingTable()) return 1;
	build_mixing_buff();
	load_mixing_buffTree(mixing_buffer_name);
	return checkMixingTable();
}

template<typename event, typename config>
void producerJTC<event, config>::linkMixingTarget(std::vector<candidate>&jetCand){
	mixingCollection.emplace_back(&jetCand);
}

template<typename event, typename config>
void producerJTC<event, config>::runMixing(std::vector<Long64_t> & mixing_list,float evtW){
	std::vector<candidate> gpmix, trkmix;
	int kevt = int(gRandom->Rndm()*mixing_list.size());
	for(int kmix = 0; kmix<nPerTrig; ++kmix){
		if(kevt == int(mixing_list.size())) kevt=0;
		Long64_t index = mixing_list.at(kevt);
		kevt++;
		if(index == voidIndex) continue; // to avoid the auto correlation in any case
		mbuff->GetEntry(index);
		//if( em->vz - mix_vz > 0.5 ) continue;
		//if( em->vz - mix_vz < -0.5) continue;
		if(doDvzDebug) hdvz[centj]->Fill(this->evt->vz, this->evt->vz-mix_vz, evtW);
		if(isMC) load_buff_gp(gpmix);
		load_buff_trk(trkmix);
		produce(reco_jet_candidate, trkmix, evtW, 1);
		if(!isMC) continue;
		produce(reco_jet_candidate, gpmix, evtW, 1);
		produce(gen_jet_candidate, gpmix, evtW, 1);
	}
}

template<typename event, typename config>
void producerJTC<event, config>::mixingLoop(float evtW){
	int vzIndex = vzAx.findBin(this->evt->vz);
	int centIndex = centAx.findBin(float(this->evt->hiBin));
	int kevt = int(gRandom->Rndm()*mixTable[vzIndex+centIndex*nvz_mix]->size());
	int addvz = 1, addcent=-1;
	if( float(vzIndex) > float(nvz_mix)/2) addvz = -1;
	if( centIndex == 0) addvz = 1;
	int nmix_candidate = 0;
	mixing_list.clear();
	while (nmix_candidate < 3*mix_min_size){
		int nme = int(mixTable[vzIndex+centIndex*nvz_mix]->size());
		for(int i=0; i<nme; ++i){
			mixing_list.emplace_back(mixTable[vzIndex+centIndex*nvz_mix]->at(i));
		}
		nmix_candidate+=nme;
		vzIndex +=addvz;
	}
	runMixing(mixing_list, evtW);
}


template<typename event, typename config>
void producerJTC<event, config>::run(){

	float evtW= this->evtWeight;
	//	centj = centax->find_bin_in_range(em->hiBin);
	centj = this->jcent;
	fillEventInfo(evtW);
	trkSelection(trk_candidate, this->evt);
	recoJetSelection(reco_jet_candidate, this->evt);
	fillJetKinematic(reco_jet_candidate, evtW);
	produce(reco_jet_candidate, trk_candidate, evtW);
	if(isMC){
		genJetSelection(gen_jet_candidate, this->evt);
		genParticleSelection(gen_particle_candidate, this->evt);
		fillJetKinematic(gen_jet_candidate, evtW);
		produce(reco_jet_candidate, gen_particle_candidate, evtW);
		produce(gen_jet_candidate, gen_particle_candidate, evtW);
		gen_particle_candidate.clear(); 
	}
	//free the track memory before the mixing loop;
	trk_candidate.clear();
	if(domixing){
		mixingLoop(evtW);
	}
	//don't forget to clear the space
	if(isMC) gen_jet_candidate.clear(); 
	reco_jet_candidate.clear();
}

template<typename event, typename config>
TTree* producerJTC<event, config>::init_mixing_buffer_tree(){
	auto mbufft= new TTree("mixing", "");
	mbufft->Branch("vz", &mix_vz);
	mbufft->Branch("hibin", &mix_hibin);
	mbufft->Branch("ntrks", &ntrks);
	mbufft->Branch("trktag", &trktag, "trktag[ntrks]/I");
	mbufft->Branch("trkpt" , &trkpt ,"trkpt[ntrks]/F");
	mbufft->Branch("trkphi", &trkphi,"trkphi[ntrks]/F");
	mbufft->Branch("trketa", &trketa,"trketa[ntrks]/F");
	mbufft->Branch("trkw"  , &trkw,"trkw[ntrks]/F");
	if(isMC){
		mbufft->Branch("ngps", &ngps);
		mbufft->Branch("gptag", &gptag,"gptag[ngps]/I");
		mbufft->Branch("gppt" , &gppt ,"gppt[ngps]/F");
		mbufft->Branch("gpphi", &gpphi,"gpphi[ngps]/F");
		mbufft->Branch("gpeta", &gpeta,"gpeta[ngps]/F");
		mbufft->Branch("gpw"  , &gpw  ,"gpw[ngps]/F");
	}
	return mbufft;
}

template<typename event, typename config>
void producerJTC<event, config>::build_mixing_buff(){
	buff=TFile::Open(mixing_buffer_name, "recreate");
	mbuff = init_mixing_buffer_tree();
	std::vector<candidate> gpmix, trkmix;
	int jobper= 10;	
	for(int i=0; i<nvz_mix;++i){
		for(int j=0; j<ncent_mix; ++j){
			int nreport = int(float(i*ncent_mix+j)/ncent_mix/nvz_mix*100);
			if(nreport > jobper ){
				std::cout<<"dumped "<<nreport<<"% mixing buff.."<<std::endl;
				jobper+=10;
			}
			//cout<<i<<" : "<<j<<endl;
			for(unsigned int k=0; k<mixTable[i+j*nvz_mix]->size(); k++){
				Long64_t index = mixTable[i+j*nvz_mix]->at(k);
				mixem->evtTree->GetEntry(index);
				add_buff_evtInfo(mixem->vz, mixem->hiBin);
				if(isMC){
					genParticleSelection(gpmix, mixem);
					add_buff_gp(gpmix);
				}
				trkSelection(trkmix, mixem);
				add_buff_trk(trkmix);
				//fill the tree and clean the buff
				mbuff->Fill();
				gpmix.clear();
				trkmix.clear();
				//updates the new index in the buff files
			}
			//cout<<"done"<<endl;
		}
	}
	buff->cd();
	mbuff->Write();
	buff->Close();
	mixem->_file->Close();
}

template<typename event, typename config>
void producerJTC<event, config>::add_buff_evtInfo(float vz0, int hibin0){
	mix_vz = vz0;  mix_hibin = hibin0; 
}
template<typename event, typename config>
void producerJTC<event, config>::add_buff_trk(std::vector<candidate> &trk){
	ntrks = trk.size();
	for(unsigned int ptr=0; ptr<trk.size(); ptr++){
		trkpt[ptr]  = trk[ptr].pt;	
		trketa[ptr] = trk[ptr].eta;	
		trkphi[ptr] = trk[ptr].phi;	
		trktag[ptr] = trk[ptr].tag.tag;
		trkw[ptr]   = trk[ptr].weight;
	}
}
template<typename event, typename config>
void producerJTC<event, config>::add_buff_gp(std::vector<candidate> &trk){
	ngps=trk.size();
	for(unsigned int ptr=0; ptr<trk.size(); ptr++){
		gppt[ptr]  = trk[ptr].pt;	
		gpeta[ptr] = trk[ptr].eta;	
		gpphi[ptr] = trk[ptr].phi;	
		gptag[ptr] = trk[ptr].tag.tag;
		gpw[ptr]   = trk[ptr].weight;
	}
}
template<typename event, typename config>
void producerJTC<event, config>::load_buff_trk(std::vector<candidate> &trk){
	trk.clear(); trk.reserve(ntrks);
	for(int i=0; i<ntrks; ++i){
		xTagger tg(trktag[i]);
		candidate tk(tg, 1,trkpt[i],trketa[i],trkphi[i],trkw[i]);
		trk.emplace_back(tk);
	}
}
template<typename event, typename config>
void producerJTC<event, config>::load_buff_gp(std::vector<candidate> &trk){
	trk.clear(); trk.reserve(ngps);
	for(int i=0; i<ngps; ++i){
		xTagger tg(gptag[i]);
		candidate tk(tg, 0,gppt[i],gpeta[i],gpphi[i],gpw[i]);
		trk.emplace_back(tk);
	}
}
template<typename event, typename config>
void producerJTC<event, config>::load_mixing_buffTree(TString path){
	buff = TFile::Open(path);
	mbuff = (TTree*) buff->Get("mixing");
	//if(mbuff == nullptr) mbuff= new TChain("mixing");
	//mbuff->Add(path);
	mbuff->SetBranchAddress("vz", &mix_vz);
	mbuff->SetBranchAddress("hibin", &mix_hibin);
	mbuff->SetBranchAddress("ntrks", &ntrks);
	mbuff->SetBranchAddress("trktag", trktag );
	mbuff->SetBranchAddress("trkpt" , trkpt   );
	mbuff->SetBranchAddress("trkphi", trkphi  );
	mbuff->SetBranchAddress("trketa", trketa  );
	mbuff->SetBranchAddress("trkw"  , trkw    );
	if(isMC){
		mbuff->SetBranchAddress("ngps", &ngps);
		mbuff->SetBranchAddress("gptag", gptag );
		mbuff->SetBranchAddress("gppt" , gppt   );
		mbuff->SetBranchAddress("gpphi", gpphi  );
		mbuff->SetBranchAddress("gpeta", gpeta  );
		mbuff->SetBranchAddress("gpw"  , gpw    );
	}
	if(mixTable == nullptr) {
		mixTable = new std::vector<unsigned int>*[nvz_mix*ncent_mix];
		for(int i=0; i<nvz_mix;++i){
			for(int j=0; j<ncent_mix; j++){
				mixTable[i+nvz_mix*j]=new std::vector<unsigned int>();
				mixTable[i+nvz_mix*j]->reserve(5);
			}
		}
	}else {
		for(int i=0; i<nvz_mix;++i){
			for(int j=0; j<ncent_mix; j++){
				mixTable[i+nvz_mix*j]->clear();
			}
		}
	}


	Long64_t nevt = mbuff->GetEntries();
	int iper = int(nevt)/20;
	int nper =0;
	if(iper ==0) nper = 1;
	cout<<iper<<endl;
	std::cout<<"loaded: "<<nevt<<" events for mixing.."<<std::endl;

	for(int i=0; i<nevt; i++){

		if(i %iper ==0){
			std::cout<<"mixing buffer loaded: "<<nper*5<<"\%.."<<std::endl;
			nper++;
		}

		mbuff->GetEntry(i);
		if(mix_vz < vzmin_mix || mix_vz > vzmax_mix || mix_hibin >= hibinmax_mix || mix_hibin < hibinmin_mix) continue;
		int ivz = vzAx.findBin(mix_vz);
		int ihibin = centAx.findBin(mix_hibin);
		if(int(mixTable[ivz+nvz_mix*ihibin]->size())< nsize)mixTable[ivz+nvz_mix*ihibin]->emplace_back(i);
	}
}

template<typename event, typename config>
void producerJTC<event, config>::setup_mixingTable(int nvz, float vzmin, float vzmax, int ncent, float centmin, float centmax){
	vzAx.makeAxis(nvz, vzmin, vzmax);
	centAx.makeAxis(ncent, centmin, centmax);
}
template<typename event, typename config>
void producerJTC<event, config>::setup_mixingTable(){
	setup_mixingTable(nvz_mix, vzmin_mix, vzmax_mix, ncent_mix, hibinmin_mix, hibinmax_mix);
}
template<typename event, typename config>
void producerJTC<event, config>::setMixTableSize(int n){
	//set the size of the each bin in mixingTable, default is 50 but needs to be shrinked for large mixing samples
	nsize = n;
}


template<typename event, typename config>
bool producerJTC<event, config>::checkMixingTable(bool doReport){
	bool pass = true;
	float whibin = float(hibinmax_mix-hibinmin_mix)/ncent_mix;
	float wvzbin = float(vzmax_mix-vzmin_mix)/nvz_mix;
	std::cout<<"mixingTable statitcs problem: "<<endl;
	for(int i= 0; i<nvz_mix; ++i){
		for(int j= 0; j<ncent_mix; ++j){
			if(mixTable[i+nvz_mix*j]->size()<mix_min_size){
				pass = false;
				std::cout<<vzmin_mix+i*wvzbin<<" < vz < ";
				std::cout<<vzmin_mix+(i+1)*wvzbin<<"; "<<j*whibin<<" < cent < "<<(j+1)*whibin<<": "<<mixTable[i+nvz_mix*j]->size()<<std::endl;
			}
		}
	}
	if(doReport){
		std::cout<<"mixing scan report:";
		for(int i= 0; i<nvz_mix; ++i){
			for(int j= 0; j<ncent_mix; ++j){
				std::cout<<vzmin_mix+i*wvzbin<<" < vz < ";
				std::cout<<vzmin_mix+(i+1)*wvzbin<<"; "<<j*whibin<<" < cent < "<<(j+1)*whibin<<": "<<mixTable[i+nvz_mix*j]->size()<<std::endl;
			}
		}}

	if(!pass ){
		std::cout<<" WARNING, statistics isn't enough!."<<std::endl;
		return 1;
	}else std::cout<<" passed the statistics check, continue generating the buff..."<<std::endl;
	return 0;
}

// the logic to the mixing buff is that we filter all the events are going to be used into 
// a canidate format and dump them into a local root file (buff). 
// Then the program will read these information from the buff when we need.
template<typename event, typename config>
bool producerJTC<event, config>::scanMixingTable(bool docheck ){
	mixTable = new std::vector<unsigned int>*[nvz_mix*ncent_mix];
	for(int i=0; i<nvz_mix;++i){
		for(int j=0; j<ncent_mix; j++){
			mixTable[i+nvz_mix*j]=new std::vector<unsigned int>();
			mixTable[i+nvz_mix*j]->reserve(5);
		}
	}
	Long64_t nentries = mixem->evtTree->GetEntries();
	std::cout<<"total events in mixing = "<<nentries<<std::endl;
	for(Long64_t jevt = 0; jevt<nentries; ++jevt){
		if(jevt%1000 == 0 ) std::cout<<"scan "<<jevt<<" events in mixing tree..."<<std::endl;
		mixem->getEvent(jevt);
		if(mixEvtCut(mixem)) continue;
		//cout<<mixem->vz<<endl;
		if( mixem->vz < vzmin_mix || mixem->vz > vzmax_mix || mixem->hiBin >= hibinmax_mix || mixem->hiBin < hibinmin_mix) continue;
		int ivz = vzAx.findBin(mixem->vz);
		int ihibin = centAx.findBin(mixem->hiBin);
		//cout<<"vz = "<<mixem->vz<<", ivz = "<<ivz<<endl;
		//cout<<"ivz = "<<ivz<<", ihibin = "<<ihibin<<endl;
		if(int(mixTable[ivz+nvz_mix*ihibin]->size())< nsize)mixTable[ivz+nvz_mix*ihibin]->emplace_back(jevt);
	}

	if(docheck) if(checkMixingTable()) return 1;
	return 0;
}


template<typename event, typename config>
void producerJTC<event, config>::dump_mixing_buffer(TString path){
	auto newbuff=TFile::Open(path, "recreate");
	auto dumpbuff = init_mixing_buffer_tree();
	int jobper= 10;	
	for(int i=0; i<nvz_mix;++i){
		for(int j=0; j<ncent_mix; ++j){
			int nreport = int(float(i*ncent_mix+j)/ncent_mix/nvz_mix*100);
			if(nreport > jobper ){
				std::cout<<"dumped "<<nreport<<"% mixing buff.."<<std::endl;
				jobper+=10;
			}
			for(unsigned int k=0; k<mixTable[i+j*nvz_mix]->size(); k++){
				Long64_t index = mixTable[i+j*nvz_mix]->at(k);
				mbuff->GetEntry(index);
				if(isMC && ngps ==0 ) cout<<"0 gen particles"<<endl;
				dumpbuff->Fill();
			}
		}
	}
	newbuff->cd();
	dumpbuff->Write();
	newbuff->Close();
}

