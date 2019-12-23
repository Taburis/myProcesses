
#include <cstdarg>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <vector>
#include "TMath.h"
#include "TFile.h"
#include "TBenchmark.h"
#include "myProcesses/jtc/plugin/histManager.h"
#include "myProcesses/jtc/plugin/jtcFastProducer.h"

class candidate{
	public : 
		candidate(){};
		candidate(xTagger tg, float pt0,float eta0,float phi0, float w= 1){ 
			set(tg, pt0,eta0,phi0,w);
		}
		void copy(candidate &c){
			pt=c.pt;eta=c.eta;phi=c.phi;weight=c.weight;tag=c.tag;isValid = c.isValid;
		}
		candidate & operator = (candidate &rhs){
			this->copy(rhs);
			return (*this);
		}
		~candidate(){};
		void set(xTagger &tg, float pt0,float eta0,float phi0, float w= 1){
			pt=pt0; 
			eta=eta0;
			phi=phi0;
			tag= tg;
			weight=w;
			isValid = 1;
		}
		void clear(){isValid = 0;}

		float pt, eta, phi, weight;
		xTagger tag;
		bool isValid= 0;
};

jtcFastProducer::jtcFastProducer(eventMap *em0){ 
	em = em0; isMC=em->isMC;
}
jtcFastProducer::~jtcFastProducer(){ 
	if(domixing){
		for(int i=0; i<nvz_mix;++i){
			for(int j=0; j<ncent_mix; ++j){
				std::vector<unsigned int>().swap(*(mixTable[i+j*nvz_mix]));
			}
		}
		delete mixTable;
	}
}

bool jtcFastProducer::recoTrkCuts(eventMap *em, int j){
	//return 1 to skip
	if(em->trkpt[j] < 1 || em->trkpt[j]>400) return 1;
	if(TMath::Abs(em->trketa[j]) >= 2.4) return 1;
	if(!(em->highPurity[j])) return 1;
	if(em->trknhit[j]< 11) return 1;
	if(em->trkchi2[j]/em->trkndof[j]/em->trknlayer[j] > 0.15) return 1;
	float et = (em->pfEcal[j] + em->pfHcal[j])/TMath::CosH(em->trketa[j]);
	if( !(em->trkpt[j]<20.0 || et>0.5*(em->trkpt[j]))) return 1;
	return 0;	
}
bool jtcFastProducer::recoJtCuts(eventMap *em, int j){
	//return 1 to skip
	if(em->jetpt[j] < 120.0) return 1;
	//		if(em->ref_jetpt[j] < 50.0) return 1;
	if(TMath::Abs(em->jeteta[j]) > 1.6) return 1;
	return 0;
}
bool jtcFastProducer::genParticleCuts(eventMap *em, int j){
	//return 1 to skip
	if(em->gppt(j) < 1.0) return 1;
	if(TMath::Abs(em->gpeta(j)) > 2.4) return 1;
	if(em->gpchg(j)==0) return 1;
	return 0;
}
bool jtcFastProducer::genJetCuts(eventMap * em, int j){
	//return 1 to skip
	if( em->genjetpt[j] < 120 ) return 1;
	if( TMath::Abs(em->genjet_wta_eta[j]) > 1.6 ) return 1;
	return 0;
}
void jtcFastProducer::genParticleSelections(std::vector<candidate>&cands, eventMap *em){
	cands.reserve(em->nGP());
	for(int i=0; i< em->nGP(); ++i){
		if(genParticleCuts(em, i)) continue;
		xTagger tag;
		tag.addTag(0);
		candidate cc(tag, em->gppt(i),em->gpeta(i),em->gpphi(i),1);
		cands.emplace_back(cc);
	}
}
void jtcFastProducer::genJetSelections(std::vector<candidate>&cands, eventMap *em){
	// if the jets have n types, need to add n times...
	cands.reserve(em->nGenJet());
	for(int i=0; i< em->nGenJet(); ++i){
		if(genJetCuts(em, i)) continue;
		//add incl jet tag
		float weight = isMC ? jetPtWeight(em->genjetpt[i]) : 1;
		xTagger tag; tag.addTag(0);
		//-------------
	}
}
/*
void jtcFastProducer::genParticleSelections(std::vector<candidate>&cands, eventMap *em){
	cands.reserve(em->nGP());
	for(int i=0; i< em->nGP(); ++i){
		if(genParticleCuts(em, i)) continue;
		xTagger tag;
		tag.addTag(trkType::inclTrk);
		candidate cc(tag, em->gppt(i),em->gpeta(i),em->gpphi(i),1);
		cands.emplace_back(cc);
	}
}
void jtcFastProducer::genJetSelections(std::vector<candidate>&cands, eventMap *em){
	// if the jets have n types, need to add n times...
	cout<<"from Fast producer"<<endl;
	cands.reserve(em->nGenJet());
	for(int i=0; i< em->nGenJet(); ++i){
		if(genJetCuts(em, i)) continue;
		//add incl jet tag
		float weight = isMC ? jetPtWeight(em->genjetpt[i]) : 1;
		xTagger tag; tag.addTag(jetType::inclJet);

		//add true b jet tag
		int index = em->genMatchIndex[i];
		if(index > -1) if(TMath::Abs(em->flavor_forb[index]) == 5) 
			tag.addTag(jetType::trueBJet);
		candidate cc2(tag, em->genjetpt[i], em->genjet_wta_eta[i], em->genjet_wta_phi[i], weight);
		cands.emplace_back(cc2);
		//-------------
	}
}
*/
void jtcFastProducer::quickHistReg(TString cap, TString dsname,  histManager *h, histCase &hc, int nPt, int nCent){
	int nHistoBinsX = 500;
	int nHistoBinsY = 200;
	const float newbin [21] = {110, 120, 136, 152, 168, 184, 200, 216, 232, 248, 264, 280, 296, 312, 328, 344, 360,
		380, 400, 432, 500};
	int nbin = 20;

	TString tmp, name;
	name=cap;
	hc.jet_pt = new TH1D*[nCent];
	hc.jet_eta = new TH1D*[nCent];
	hc.jet_phi = new TH1D*[nCent];
	for(int j=0; j<nCent; ++j){
		tmp = centLabel[j]+" to "+centLabel[j+1];
		hc.jet_pt[j] = hm->regHist<TH1D>(name+Form("_corrpt_%d",j), tmp, nbin, newbin);
		hc.jet_eta[j] = hm->regHist<TH1D>(name+Form("_eta_%d",j), tmp, 100, -2.0, 2.0);
		hc.jet_phi[j] = hm->regHist<TH1D>(name+Form("_phi_%d",j), tmp, 72, -TMath::Pi(), TMath::Pi());
	}

	if(!dojtc) return;
	hc.sig= new TH2D*[nPt*nCent];
	hc.sig_pTweighted= new TH2D*[nPt*nCent];
	hc.mixing= new TH2D*[nPt*nCent];
	name = cap+"_"+dsname;
	for(int i=0; i<nPt; ++i){
		for(int j=0; j<nCent; ++j){
			tmp = centLabel[j]+"_"+centLabel[j+1]+"_"+ptLabel[i]+"_"+ptLabel[i+1];
			hc.sig[i+j*nPt] = hm->regHist<TH2D>(name+Form("_%d_%d",i, j), tmp,
					nHistoBinsX,-5,5,nHistoBinsY,-TMath::Pi()/2,3*TMath::Pi()/2);
			hc.sig_pTweighted[i+j*nPt] = hm->regHist<TH2D>(name+Form("_pTweighted_%d_%d",i, j), tmp,
					nHistoBinsX,-5,5, nHistoBinsY,-TMath::Pi()/2,3*TMath::Pi()/2);
			hc.mixing[i+j*nPt] = hm->regHist<TH2D>(name+Form("_mixing_%d_%d",i, j), tmp,
					nHistoBinsX,-5,5, nHistoBinsY,-TMath::Pi()/2,3*TMath::Pi()/2);
		}
	}
}

void jtcFastProducer::fillJetKinematic(std::vector<candidate>&jetCand, float evtW){
	for(unsigned int i = 0;i<jetCand.size(); i++){
		for(unsigned int k=0; k<jtcList.size(); ++k){
			if(!(jetCand[i].tag.select(jtcList[k].jetTag))) continue;
			jtcList[k].hc->jet_pt[centj]->Fill(jetCand [i].pt , (jetCand[i].weight)*evtW);
			jtcList[k].hc->jet_eta[centj]->Fill(jetCand[i].eta, (jetCand[i].weight)*evtW);
			jtcList[k].hc->jet_phi[centj]->Fill(jetCand[i].phi, (jetCand[i].weight)*evtW);
		}
	}
}


void jtcFastProducer::fillHistCase(histCase &hc, candidate&jet, candidate&trk, float evtW, bool fillMix){
	int ptj = ptax->findBin(safeValue(trk.pt,ptbins[nPt]-1));
	float dphic=(jet).phi-(trk).phi;
	while(dphic>(1.5*TMath::Pi())){dphic+= -2*TMath::Pi();}
	while(dphic<(-0.5*TMath::Pi())){dphic+= 2*TMath::Pi();}

	float weightc = evtW*(jet.weight)*(trk.weight);
	float detac = jet.eta-trk.eta;
	//cout<<"filling: "<<ptj<<" : "<<detac<<" : "<<dphic<<endl;
	if(fillMix){
		hc.mixing[ptj+nPt*centj]->Fill(detac, dphic, weightc);
	}else {
		hc.sig[ptj+nPt*centj]->Fill(detac, dphic, weightc);
		hc.sig_pTweighted[ptj+nPt*centj]->Fill(detac, dphic, trk.pt*weightc);
	}
}

bool jtcFastProducer::checkJtcPair(jtcTag &secl, candidate&jet,candidate&trk){
	return (jet.tag.select(secl.jetTag) && trk.tag.select(secl.trkTag));
}


void jtcFastProducer::produce(std::vector<candidate>&jetCand, std::vector<candidate>&trkCand,float evtW, bool fillMix){
	if(!fillMix) fillJetKinematic(jetCand, evtW);
	if(!dojtc)return;
	for(unsigned int j = 0;j<trkCand.size(); j++){
		for(unsigned int i = 0;i<jetCand.size(); i++){
			for(unsigned int k=0; k<jtcList.size(); ++k){
				if(checkJtcPair(jtcList[k], jetCand[i], trkCand[j]))
					fillHistCase(*(jtcList[k].hc),jetCand[i], trkCand[j], evtW,fillMix);
			}
		}
	}
}

float jtcFastProducer::safeValue(float in, float max){
	//to prevent the overflow.
	if(in > max) return max;
	return in;
}
void jtcFastProducer::fillEventInfo(float evtW){
	hvz->Fill(em->vz, evtW);
	if(isMC) hpthat->Fill(safeValue(em->pthat,399), evtW);
	if(!ispp)hcent->Fill(em->hiBin, evtW);
}

void jtcFastProducer::add_evtInfo_hist(){
	hm = new histManager();
	ptax = new xAxis(nPt, ptbins);
	centax= new xAxis(nCent, centbins);
	hvz = hm->regHist<TH1D>("vzInfo", "", 200, -20, 20);
	if(!ispp)hcent = hm->regHist<TH1D>("centInfo","",  50, 0, 200);
	if(isMC) hpthat = hm->regHist<TH1D>("pthatInfo", "", 100, 0, 400);
}

void jtcFastProducer::mixing_preparation(){
	setup_mixingTable(nvz_mix, vzmin_mix, vzmax_mix, ncent_mix, hibinmin_mix, hibinmax_mix);
	scanMixingTable();
	load_mixing_buffTree();
}

void jtcFastProducer::write(std::string name){
	auto wf = TFile::Open(name.c_str(), "recreate");
	wf->cd();
	hm->write();
	wf->Close();
}
bool jtcFastProducer::initialCheck(){
	// the code won't run if the initial check failed
	if(isMC){
		if( jetPtWeight == 0) {
			std::cout<<"jet pT re-weight function is missing"<<std::endl;
			return 1;
		}
		if( evtWeight == nullptr) {
			std::cout<<"event weight function is missing"<<std::endl;
			return 1;
		}
	}
	return 0;
}

bool jtcFastProducer::evtCut(eventMap *em){
	if(em->checkEventFilter()) return 1;
	if(TMath::Abs(em->vz) > 15) return 1;
	//if(em->pthat < 50) return 1;
	return 0;
}
bool jtcFastProducer::mixEvtCut(eventMap *em){
	if(evtCut(em)) return 1;
	//	if(isMC) if(em->pthat > 40) return 1;
	return 0;
}

void jtcFastProducer::linkMixingTarget(std::vector<candidate>&jetCand){
	mixingCollection.emplace_back(&jetCand);
}

void jtcFastProducer::mixingLoop(float evtW){
	int vzIndex = vzAx.findBin(em->vz);
	int centIndex = centAx.findBin(float(em->hiBin));
	//	if(mixTable[vzIndex+centIndex*nvz_mix]->size()<2){
	//		if(vzIndex == 29) vzIndex = 28;// shift mixing vz from 14, 15 to 13, 14; 
	//		else if(vzIndex == 0) vzIndex = 1;// shift mixing vz from -14,-15 to -13, -14; 
	//		else if(centIndex>0) centIndex=centIndex-1;  // shift the most prepheral to next prepheral
	//	}
	//cout<<"vzindex = "<<vzIndex<<",centIndex = "<<centIndex<<endl;
	if(mixTable[vzIndex+centIndex*nvz_mix]->size()==0) return;

	std::vector<candidate> gpmix, trkmix;
	int kevt = 0;
	for(int kmix = 0; kmix<nPerTrig; ++kmix){
		if(kevt == int(mixTable[vzIndex+centIndex*nvz_mix]->size())) kevt=0;
		//cout<<"vz = "<<vz<<", index = "<<vzIndex+centIndex*ncent_mix<<endl;
		Long64_t index = mixTable[vzIndex+centIndex*nvz_mix]->at(kevt);
		//cout<<"index = "<<mixTable[vzIndex+centIndex*ncent_mix]->at(kevt)<<endl;
		kevt++;
		if(index == voidIndex) continue; // to avoid the auto correlation in any case
		mbuff->GetEntry(index);
		load_buff_gp(gpmix);
		//cout<<"size of trks: "<<gpmix.size()<<endl;
		//cout<<"current vz: "<<vz<<", mix vz: "<<mt->vz<<endl;
		for( auto & it:mixingCollection){
			//std::cout<<"jet set length: "<<(*(it)).size()<<std::endl;
			produce(*(it), gpmix, evtW, 1);
		}
		//cout<<"mixed "<<kmix<<": "<<gpmix.size()<<endl;
	}
}
void jtcFastProducer::regJtcPair(xTagger jetTg, xTagger trkTg, histCase &hc){
	jtcTag tg;
	tg.jetTag=jetTg; tg.trkTag=trkTg; tg.hc=&hc;
	jtcList.emplace_back(tg);
}

void jtcFastProducer::loop(){
	TBenchmark clock; clock.Start("loop");

	if(initialCheck()) return;
	if(domixing) mixing_preparation();
	add_evtInfo_hist();
	beginJob();
	Long64_t nentries = nevt < 0 ? em->evtTree->GetEntriesFast() : nevt;

	std::vector<candidate> gj, gp, recoJet, trks;
	linkMixingTarget(gj);
	linkMixingTarget(recoJet);
	for(Long64_t jentry = 0; jentry< nentries; ++jentry){
		if(jentry%1000 ==0 ){
			std::cout<<"processed "<<jentry<<" events ... "<<std::endl;
			clock.Show("loop");
			clock.Reset();
			clock.Start("loop");
		}
		em->getEvent(jentry);	
		if(evtCut(em)) continue;
		float evtW= 1;
		if(isMC) evtW= evtWeight(em);
		fillEventInfo(evtW);
		genJetSelections(gj, em);
		genParticleSelections(gp, em);
		produce(gj, gp, evtW);
		//free the track memory before the mixing loop;
		gp.clear(); trks.clear();
		if(domixing && dojtc){
			mixingLoop(evtW);
		}
		//don't forget to clear the space
		gj.clear(); recoJet.clear();
	}
	write(outputName);
}

void jtcFastProducer::build_mixing_buff(){
	buff=TFile::Open("mixing_buff.root", "recreate");
	mbuff= new TTree("mixing", "");
	mbuff->Branch("vz", &vz);
	mbuff->Branch("hibin", &hibin);
	mbuff->Branch("ntrks", &ntrks);
	mbuff->Branch("trktag", &trktag, "trktag[ntrks]/I");
	mbuff->Branch("trkpt" , &trkpt ,"trkpt[ntrks]/F");
	mbuff->Branch("trkphi", &trkphi,"trkphi[ntrks]/F");
	mbuff->Branch("trketa", &trketa,"trketa[ntrks]/F");
	mbuff->Branch("trkw"  , &trkw,"trkw[ntrks]/F");
	mbuff->Branch("ngps", &ngps);
	mbuff->Branch("gptag", &gptag,"gptag[ngps]/I");
	mbuff->Branch("gppt" , &gppt ,"gppt[ngps]/F");
	mbuff->Branch("gpphi", &gpphi,"gpphi[ngps]/F");
	mbuff->Branch("gpeta", &gpeta,"gpeta[ngps]/F");
	mbuff->Branch("gpw"  , &gpw  ,"gpw[ngps]/F");
}

void jtcFastProducer::add_buff_evtInfo(float vz0, int hibin0){
	vz = vz0;  hibin = hibin0; 
}
void jtcFastProducer::add_buff_trk(std::vector<candidate> &trk){
	ntrks = trk.size();
	for(unsigned int ptr=0; ptr<trk.size(); ptr++){
		trkpt[ptr]  = trk[ptr].pt;	
		trketa[ptr] = trk[ptr].eta;	
		trkphi[ptr] = trk[ptr].phi;	
		trktag[ptr] = trk[ptr].tag.tag;
		trkw[ptr]   = trk[ptr].weight;
	}
}
void jtcFastProducer::add_buff_gp(std::vector<candidate> &trk){
	ngps=trk.size();
	for(unsigned int ptr=0; ptr<trk.size(); ptr++){
		gppt[ptr]  = trk[ptr].pt;	
		gpeta[ptr] = trk[ptr].eta;	
		gpphi[ptr] = trk[ptr].phi;	
		gptag[ptr] = trk[ptr].tag.tag;
		gpw[ptr]   = trk[ptr].weight;
	}
}
void jtcFastProducer::load_buff_trk(std::vector<candidate> &trk){
	trk.clear(); trk.reserve(ntrks);
	for(int i=0; i<ntrks; ++i){
		xTagger tg(trktag[i]);
		candidate tk(tg,trkpt[i],trketa[i],trkphi[i],trkw[i]);
		trk.emplace_back(tk);
	}
}
void jtcFastProducer::load_buff_gp(std::vector<candidate> &trk){
	trk.clear(); trk.reserve(ngps);
	for(int i=0; i<ngps; ++i){
		xTagger tg(gptag[i]);
		candidate tk(tg,gppt[i],gpeta[i],gpphi[i],gpw[i]);
		trk.emplace_back(tk);
	}
}
void jtcFastProducer::load_mixing_buffTree(){
	buff = TFile::Open("mixing_buff.root");
	mbuff = (TTree*) buff->Get("mixing");
	mbuff->SetBranchAddress("vz", &vz);
	mbuff->SetBranchAddress("hibin", &hibin);
	mbuff->SetBranchAddress("ntrks", &ntrks);
	mbuff->SetBranchAddress("trktag", trktag );
	mbuff->SetBranchAddress("trkpt" , trkpt   );
	mbuff->SetBranchAddress("trkphi", trkphi  );
	mbuff->SetBranchAddress("trketa", trketa  );
	mbuff->SetBranchAddress("trkw"  , trkw    );
	mbuff->SetBranchAddress("ngps", &ngps);
	mbuff->SetBranchAddress("gptag", gptag );
	mbuff->SetBranchAddress("gppt" , gppt   );
	mbuff->SetBranchAddress("gpphi", gpphi  );
	mbuff->SetBranchAddress("gpeta", gpeta  );
	mbuff->SetBranchAddress("gpw"  , gpw    );
}

void jtcFastProducer::write_mixing_buff(){
	buff->cd();
	mbuff->Write();
	buff->Close();
}
void jtcFastProducer::setup_mixingTable(int nvz, float vzmin, float vzmax, int ncent, float centmin, float centmax){
	vzAx.makeAxis(nvz, vzmin, vzmax);
	centAx.makeAxis(ncent, centmin, centmax);
}
void jtcFastProducer::setMixTableSize(int n){
	//set the size of the each bin in mixingTable, default is 50 but needs to be shrinked for large mixing samples
	nsize = n;
}


// the logic to the mixing buff is that we filter all the events are going to be used into 
// a canidate format and dump them into a local root file (buff). 
// Then the program will read these information from the buff when we need.
bool jtcFastProducer::scanMixingTable(){
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
		int ivz = vzAx.findBin(mixem->vz);
		int ihibin = centAx.findBin(mixem->hiBin);
		//cout<<"vz = "<<mixem->vz<<", ivz = "<<ivz<<endl;
		//cout<<"ivz = "<<ivz<<", ihibin = "<<ihibin<<endl;
		if(int(mixTable[ivz+nvz_mix*ihibin]->size())< nsize)mixTable[ivz+nvz_mix*ihibin]->emplace_back(jevt);
	}
	std::cout<<"vz statitcs problem: "<<endl;
	bool pass = true;
	for(int i= 0; i<nvz_mix; ++i){
		for(int j= 0; j<ncent_mix; ++j){
			if(mixTable[i+nvz_mix*j]->size()<3){
				std::cout<<-15+i<<" < vz < "<<-14+i<<"; "
					<<j*5<<" < cent < "<<j*5+5<<": "<<mixTable[i+nvz_mix*j]->size()<<std::endl;
				pass = false;
			}
		}
	}
	std::cout<<"mixing scan report:";

	if(!pass){
		std::cout<<" failed the due to poor statistics, return."<<std::endl;
		return 1;
	}else std::cout<<" passed the statistics check, continue generating the buff..."<<std::endl;

	std::vector<candidate> gpmix;
	Long64_t jevt = 0;
	build_mixing_buff();
	for(int i=0; i<nvz_mix;++i){
		for(int j=0; j<ncent_mix; ++j){
			//cout<<i<<" : "<<j<<endl;
			for(unsigned int k=0; k<mixTable[i+j*nvz_mix]->size(); k++){
				Long64_t index = mixTable[i+j*nvz_mix]->at(k);
				mixem->evtTree->GetEntry(index);
				genParticleSelections(gpmix, mixem);
				add_buff_evtInfo(mixem->vz, mixem->hiBin);
				add_buff_gp(gpmix);
				//fill the tree and clean the buff
				mbuff->Fill();
				gpmix.clear();
				//updates the new index in the buff files
				mixTable[i+j*nvz_mix]->at(k)=jevt;
				jevt++;
			}
			//cout<<"done"<<endl;
		}
	}
	write_mixing_buff();
	mixem->_file->Close();
	return 0;
}


