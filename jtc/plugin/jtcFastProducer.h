
#ifndef JTCFASTPRODUCER_H
#define JTCFASTPRODUCER_H
#include <cstdarg>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <vector>
#include "TMath.h"
#include "TFile.h"
#include "TBenchmark.h"
#include "myProcesses/jtc/plugin/histManager.h"
#include "myProcesses/jtc/plugin/xAxis.h"
#include "myProcesses/jtc/plugin/jtcUti.h"
#include "myProcesses/jtc/plugin/xTagger.h"

//enum jetType {inclJet, trueBJet};
//enum trkType {inclTrk};

class eventMap;
class candidate;

class jtcFastProducer{
	struct jtcTag {
		xTagger jetTag, trkTag; histCase *hc;
	};
	public : 
	jtcFastProducer(eventMap *em0);
	~jtcFastProducer();
	virtual bool evtCut(eventMap*);
	virtual bool mixEvtCut(eventMap*);
	virtual bool recoTrkCuts(eventMap *em, int j);
	virtual bool recoJtCuts(eventMap *em, int j);
	virtual bool genParticleCuts(eventMap *em, int j);
	virtual bool genJetCuts(eventMap * em, int j);
	virtual void genParticleSelections(std::vector<candidate>&cands, eventMap *em);
	virtual void genJetSelections(std::vector<candidate>&cands, eventMap *em);
	virtual void trkSelections(std::vector<candidate>&cands, eventMap *em);
	void add_evtInfo_hist();
	void quickHistReg(TString cap, TString dsname,  histManager *h, histCase &hc, int nPt, int nCent, bool dropJetQa = 0);
	virtual void fillJetKinematic(std::vector<candidate>&jetCand, float evtW);
	virtual void fillHistCase(histCase &hc, candidate&jet, candidate&trk, float evtW, bool fillMix);
	bool checkJtcPair(jtcTag &secl, candidate&jet,candidate&trk);
	virtual void produce(std::vector<candidate>&jetCand, std::vector<candidate>&trkCand,float evtW, bool fillMix = 0);
	float safeValue(float in, float max);
	void fillEventInfo(float evtW= 1);
	void write(std::string name);
	virtual void beginJob()= 0;
	bool initialCheck();

	void linkMixingTarget(std::vector<candidate>&jetCand);
	bool quick_mixing_buff();
	void mixingLoop(float evtW);
	void regJtcPair(xTagger jetTg, xTagger trkTg, histCase &hc);
	void loop();

	float (*evtWeight)(eventMap *em) = nullptr;
	float (*jetWeight)(float pt, float eta, float phi) = 0; //return 1 to skip
	bool isMC, ispp;
	// this dojtc used turn off the correlation
	// switch it off to test the weights
	bool domixing = 0, dojtc = 1;
	histManager *hm;
	int nPt=6, nCent= 1;
	Long64_t nevt = -1; 
	xAxis *ptax , *centax; 
	TH1D* hvz, *hcent, *hpthat;
	std::vector<std::vector<candidate>*> mixingCollection;
	std::vector<jtcTag> jtcList;

	void build_mixing_buff();
	void setMixTableSize(int n);
	bool scanMixingTable();
	bool checkMixingTable();
	bool load_mixing_buffTree(TString path);
	void load_buff_trk(std::vector<candidate> &);
	void add_buff_evtInfo(float, int hibin = 0);
	void add_buff_trk(std::vector<candidate> &trk);
	void setup_mixingTable(int nvz, float vzmin, float vzmax, int ncent, float centmin, float centmax);
	void setup_mixingTable();
	void add_buff_gp(std::vector<candidate> &trk);
	void load_buff_gp(std::vector<candidate> &);


	float *ptbins     =0;
	float *centbins   =0;
	TString *ptLabel  =0;
	TString *centLabel=0;
	//histCase inclCase, trueBCase;
	eventMap *em, *mixem;

	//global variables for transfer data
	int centj =0;
	Long64_t voidIndex;
	std::string outputName = "correlation.root";

	//mxing part
	int nPerTrig = 40;
	TTree* mbuff;
	int ntrks=0, ngps=0;
	float vz=0;
	int hibin=-1;
	int trktag[9999];
	float trkpt[9999], trketa[9999], trkphi[9999], trkw[9999];
	int gptag[9999];
	float gppt[9999], gpeta[9999], gpphi[9999], gpw[9999];
	int ncent_mix=1, nvz_mix = 30, nsize=50;
	float vzmin_mix=-15, vzmax_mix=15, hibinmin_mix=0, hibinmax_mix=200; 
	std::vector<unsigned int>** mixTable = nullptr;
	TFile*buff;
	xAxis vzAx, centAx;		
};


#endif
