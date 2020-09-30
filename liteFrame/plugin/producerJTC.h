
#ifndef PRODUCERJTC_H
#define PRODUCERJTC_H
#include <cstdarg>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <vector>
#include "TMath.h"
#include "TFile.h"
#include "TBenchmark.h"
#include "myProcesses/liteFrame/plugin/liteFrame.h"
//#include "myProcesses/liteFrame/plugin/jtcUti.h"
#include <map>

//enum jetType {inclJet, trueBJet};
//enum trkType {inclTrk};

class candidate;

template<typename event, typename config>
class producerJTC : public producerBase<event, config> {

	// reco object is 1, and 0 for gen level object
	struct jtcSet {
		xTagger jetTag, trkTag;
		bool isRecoJet, isRecoTrk, domixing;
		TH2D** sig;
		TH2D** sig_pTweighted;
		TH2D** mixing, **mix_trkmap;
		//		xTagger jtRecoLevel, trkRecoLevel;
	};

	struct jetQASet{
		xTagger jetTag;
		bool isReco;
		TH1D** jet_pt;
		TH1D** jet_eta;
		TH1D** jet_phi;
	};

	public : 
	producerJTC(const char* name):producerBase<event, config>(name){};
	~producerJTC();
	//	virtual bool evtCut(event*);
	virtual bool mixEvtCut(event*);
	//	virtual bool recoTrkCuts(event *em, int j);
	//	virtual bool recoJtCuts(event *em, int j);
	//	virtual bool genParticleCuts(event *em, int j);
	//	virtual bool genJetCuts(event * em, int j);
	virtual void recoJetSelection(std::vector<candidate>&cands, event *em);
	virtual void genParticleSelection(std::vector<candidate>&cands, event *em);
	virtual void genJetSelection(std::vector<candidate>&cands, event *em);
	virtual void trkSelection(std::vector<candidate>&cands, event *em);
	void add_evtInfo_hist();
	void addJtcSet(TString name, xTagger, xTagger, bool domixing = 1);
	void addJtcSet(TString name, TString dir, xTagger, bool, xTagger, bool , bool domixing=1);
	void addJetQASet(TString name, xTagger, bool);
	void addJetQASet(TString name, xTagger);
	virtual void fillJetKinematic(std::vector<candidate>&jetCand, float evtW);
	virtual void fillHistCase(jtcSet &hc, candidate&jet, candidate&trk, float evtW, bool fillMix);
	bool checkJtcPair(jtcSet &secl, candidate&jet,candidate&trk);
	virtual void produce(std::vector<candidate>&jetCand, std::vector<candidate>&trkCand,float evtW, bool fillMix = 0);
	float safeValue(float in, float max);
	void fillEventInfo(float evtW= 1);
	void write(std::string name);

	void linkMixingTarget(std::vector<candidate>&jetCand);
	bool quick_mixing_buff();
	void mixingLoop(float evtW);
	void runMixing(std::vector<Long64_t> &, float evtW);
	virtual void run() override;

	//float (*jetWeight)(float pt, float eta, float phi) = 0; //return 1 to skip
	bool isMC, ispp;
	// switch it off to test the weights
	bool domixing = 0;
	int nPt=6, nCent= 1;
	Long64_t nevt = -1; 
	xAxis *ptax , *centax; 
	TH1D* hvz, *hcent, *hpthat;
	TH2D** hdvz;
	std::vector<std::vector<candidate>*> mixingCollection;
	std::vector<jtcSet> jtcList;
	std::vector<jetQASet> jetQAs;
	std::vector<candidate> gen_jet_candidate, reco_jet_candidate;
	std::vector<candidate> gen_particle_candidate, trk_candidate;

	TTree* init_mixing_buffer_tree();
	void build_mixing_buff();
	void dump_mixing_buffer(TString fname); //write the mixing buffer contained in the mixing table into a root file
	void setMixTableSize(int n);
	bool scanMixingTable(bool docheck = 0);
	bool checkMixingTable(bool doReport = 0);
	void load_mixing_buffTree(TString path);
	void load_buff_trk(std::vector<candidate> &);
	void add_buff_evtInfo(float, int hibin = 0);
	void add_buff_trk(std::vector<candidate> &trk);
	void setup_mixingTable(int nvz, float vzmin, float vzmax, int ncent, float centmin, float centmax);
	void setup_mixingTable();
	void add_buff_gp(std::vector<candidate> &trk);
	void load_buff_gp(std::vector<candidate> &);

	void setCentrality(int ncent, const float *bins, TString* centlabel){
		centLabel = centlabel;
		nCent = ncent; centbins = bins;
		ispp = 0;
		hibinmin_mix = bins[0];
		hibinmax_mix = bins[ncent];
	}
	void setPtBins(int npt, const float *bins, TString* label){
		nPt = npt; ptbins = bins; ptLabel = label;
	}

	bool linkFrame(liteFrame<event, config> *frame)override{
		frame->doTrk = 1;
		frame->doJet = 1;
		if(this->evt->isMC) frame->doGenParticle=1;
		return 0;
	}

	virtual bool initialCheck()override{

		setCentrality(this->_cfg->ps->ncent, this->_cfg->ps->centbin, this->_cfg->ps->centLabel);
		setPtBins(this->_cfg->ps->nptbin, this->_cfg->ps->ptbin, this->_cfg->ps->ptLabels);

		if(ptLabel==0){
			std::cout<<"Error: ptLabel haven't be assigend, abort!"<<std::endl;
			return 1;
		}
		if(centLabel==0){ std::cout<<"Error: centLabel haven't be assigend, abort!"<<std::endl;
			return 1;
		}
		return 0;
	}

	const float *ptbins     =0;
	const float *centbins   =0;
	TString *ptLabel  =0;
	TString *centLabel=0;
	TString mixing_buffer_name="mixing_buff.root";
	//histCase inclCase, trueBCase;
	event *mixem;

	//global variables for transfer data
	int centj =0;
	Long64_t voidIndex;
	std::string outputName = "correlation.root";

	//mxing part
	int nPerTrig = 40;
	TTree* mbuff;
	int ntrks=0, ngps=0;
	float mix_vz=0;
	int mix_hibin=-1;
	int trktag[9999];
	float trkpt[9999], trketa[9999], trkphi[9999], trkw[9999];
	int gptag[9999];
	float gppt[9999], gpeta[9999], gpphi[9999], gpw[9999];
	int ncent_mix=1, nvz_mix = 30, nsize=50, mix_min_size = 2;
	float vzmin_mix=-15, vzmax_mix=15, hibinmin_mix=0, hibinmax_mix=200; 
	std::vector<unsigned int>** mixTable = nullptr;
	std::vector<Long64_t> mixing_list;
	TFile*buff;
	xAxis vzAx, centAx;
};


#endif
