//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Mon Sep 30 23:32:46 2019 by ROOT version 6.12/07
// from TTree t/ Jet Analysis Tree
// found on file: /eos/cms/store/group/phys_heavyions/jviinika/PbPbData2018-HIRun2018A-HIHardProbes-04Apr2019-v1_80or100caloJetTrigger_newJetsPt25_103X_2019-09-13/part0/HiForestAOD_PbPbData2018_103.root
//////////////////////////////////////////////////////////

#ifndef hiJet_h
#define hiJet_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.

class hiJet {
	public :
		TTree          *fChain;   //!pointer to the analyzed TTree or TChain
		Int_t           fCurrent; //!current Tree number in a TChain

		// Fixed size dimensions of array or collections stored in the TTree if any.

		// Declaration of leaf types
		Int_t           run;
		Int_t           evt;
		Int_t           lumi;
		Int_t           nref;
		Float_t         rawpt[99];   //[nref]
		Float_t         jtpt[99];   //[nref]
		Float_t         jteta[99];   //[nref]
		Float_t         jty[99];   //[nref]
		Float_t         jtphi[99];   //[nref]
		Float_t         jtpu[99];   //[nref]
		Float_t         jtm[99];   //[nref]
		Float_t         jtarea[99];   //[nref]
		Float_t         WTAeta[99];   //[nref]
		Float_t         WTAphi[99];   //[nref]
		Float_t         jtPfCHF[99];   //[nref]
		Float_t         jtPfNHF[99];   //[nref]
		Float_t         jtPfCEF[99];   //[nref]
		Float_t         jtPfNEF[99];   //[nref]
		Float_t         jtPfMUF[99];   //[nref]
		Int_t           jtPfCHM[99];   //[nref]
		Int_t           jtPfNHM[99];   //[nref]
		Int_t           jtPfCEM[99];   //[nref]
		Int_t           jtPfNEM[99];   //[nref]
		Int_t           jtPfMUM[99];   //[nref]
		Float_t         jttau1[99];   //[nref]
		Float_t         jttau2[99];   //[nref]
		Float_t         jttau3[99];   //[nref]
		Float_t         trackMax[99];   //[nref]
		Float_t         trackSum[99];   //[nref]
		Int_t           trackN[99];   //[nref]
		Float_t         trackHardSum[99];   //[nref]
		Int_t           trackHardN[99];   //[nref]
		Float_t         chargedMax[99];   //[nref]
		Float_t         chargedSum[99];   //[nref]
		Int_t           chargedN[99];   //[nref]
		Float_t         chargedHardSum[99];   //[nref]
		Int_t           chargedHardN[99];   //[nref]
		Float_t         photonMax[99];   //[nref]
		Float_t         photonSum[99];   //[nref]
		Int_t           photonN[99];   //[nref]
		Float_t         photonHardSum[99];   //[nref]
		Int_t           photonHardN[99];   //[nref]
		Float_t         neutralMax[99];   //[nref]
		Float_t         neutralSum[99];   //[nref]
		Int_t           neutralN[99];   //[nref]
		Float_t         hcalSum[99];   //[nref]
		Float_t         ecalSum[99];   //[nref]
		Float_t         eMax[99];   //[nref]
		Float_t         eSum[99];   //[nref]
		Int_t           eN[99];   //[nref]
		Float_t         muMax[99];   //[nref]
		Float_t         muSum[99];   //[nref]
		Int_t           muN[99];   //[nref]
		Float_t         discr_ssvHighEff[99];   //[nref]
		Float_t         discr_ssvHighPur[99];   //[nref]
		Float_t         discr_csvV1[99];   //[nref]
		Float_t         discr_csvV2[99];   //[nref]
		Float_t         discr_muByIp3[99];   //[nref]
		Float_t         discr_muByPt[99];   //[nref]
		Float_t         discr_prob[99];   //[nref]
		Float_t         discr_probb[99];   //[nref]
		Float_t         discr_tcHighEff[99];   //[nref]
		Float_t         discr_tcHighPur[99];   //[nref]
		Float_t         ndiscr_ssvHighEff[99];   //[nref]
		Float_t         ndiscr_ssvHighPur[99];   //[nref]
		Float_t         ndiscr_csvV1[99];   //[nref]
		Float_t         ndiscr_csvV2[99];   //[nref]
		Float_t         ndiscr_muByPt[99];   //[nref]
		Float_t         pdiscr_csvV1[99];   //[nref]
		Float_t         pdiscr_csvV2[99];   //[nref]
		Int_t           nsvtx[99];   //[nref]
		Int_t           svtxntrk[99];   //[nref]
		Float_t         svtxdl[99];   //[nref]
		Float_t         svtxdls[99];   //[nref]
		Float_t         svtxdl2d[99];   //[nref]
		Float_t         svtxdls2d[99];   //[nref]
		Float_t         svtxm[99];   //[nref]
		Float_t         svtxpt[99];   //[nref]
		Float_t         svtxmcorr[99];   //[nref]
		Int_t           nIPtrk[99];   //[nref]
		Int_t           nselIPtrk[99];   //[nref]
		Float_t         mue[99];   //[nref]
		Float_t         mupt[99];   //[nref]
		Float_t         mueta[99];   //[nref]
		Float_t         muphi[99];   //[nref]
		Float_t         mudr[99];   //[nref]
		Float_t         muptrel[99];   //[nref]
		Int_t           muchg[99];   //[nref]

		// List of branches
		TBranch        *b_run;   //!
		TBranch        *b_evt;   //!
		TBranch        *b_lumi;   //!
		TBranch        *b_nref;   //!
		TBranch        *b_rawpt;   //!
		TBranch        *b_jtpt;   //!
		TBranch        *b_jteta;   //!
		TBranch        *b_jty;   //!
		TBranch        *b_jtphi;   //!
		TBranch        *b_jtpu;   //!
		TBranch        *b_jtm;   //!
		TBranch        *b_jtarea;   //!
		TBranch        *b_WTAeta;   //!
		TBranch        *b_WTAphi;   //!
		TBranch        *b_jtPfCHF;   //!
		TBranch        *b_jtPfNHF;   //!
		TBranch        *b_jtPfCEF;   //!
		TBranch        *b_jtPfNEF;   //!
		TBranch        *b_jtPfMUF;   //!
		TBranch        *b_jtPfCHM;   //!
		TBranch        *b_jtPfNHM;   //!
		TBranch        *b_jtPfCEM;   //!
		TBranch        *b_jtPfNEM;   //!
		TBranch        *b_jtPfMUM;   //!
		TBranch        *b_jttau1;   //!
		TBranch        *b_jttau2;   //!
		TBranch        *b_jttau3;   //!
		TBranch        *b_trackMax;   //!
		TBranch        *b_trackSum;   //!
		TBranch        *b_trackN;   //!
		TBranch        *b_trackHardSum;   //!
		TBranch        *b_trackHardN;   //!
		TBranch        *b_chargedMax;   //!
		TBranch        *b_chargedSum;   //!
		TBranch        *b_chargedN;   //!
		TBranch        *b_chargedHardSum;   //!
		TBranch        *b_chargedHardN;   //!
		TBranch        *b_photonMax;   //!
		TBranch        *b_photonSum;   //!
		TBranch        *b_photonN;   //!
		TBranch        *b_photonHardSum;   //!
		TBranch        *b_photonHardN;   //!
		TBranch        *b_neutralMax;   //!
		TBranch        *b_neutralSum;   //!
		TBranch        *b_neutralN;   //!
		TBranch        *b_hcalSum;   //!
		TBranch        *b_ecalSum;   //!
		TBranch        *b_eMax;   //!
		TBranch        *b_eSum;   //!
		TBranch        *b_eN;   //!
		TBranch        *b_muMax;   //!
		TBranch        *b_muSum;   //!
		TBranch        *b_muN;   //!
		TBranch        *b_discr_ssvHighEff;   //!
		TBranch        *b_discr_ssvHighPur;   //!
		TBranch        *b_discr_csvV1;   //!
		TBranch        *b_discr_csvV2;   //!
		TBranch        *b_discr_muByIp3;   //!
		TBranch        *b_discr_muByPt;   //!
		TBranch        *b_discr_prob;   //!
		TBranch        *b_discr_probb;   //!
		TBranch        *b_discr_tcHighEff;   //!
		TBranch        *b_discr_tcHighPur;   //!
		TBranch        *b_ndiscr_ssvHighEff;   //!
		TBranch        *b_ndiscr_ssvHighPur;   //!
		TBranch        *b_ndiscr_csvV1;   //!
		TBranch        *b_ndiscr_csvV2;   //!
		TBranch        *b_ndiscr_muByPt;   //!
		TBranch        *b_pdiscr_csvV1;   //!
		TBranch        *b_pdiscr_csvV2;   //!
		TBranch        *b_nsvtx;   //!
		TBranch        *b_svtxntrk;   //!
		TBranch        *b_svtxdl;   //!
		TBranch        *b_svtxdls;   //!
		TBranch        *b_svtxdl2d;   //!
		TBranch        *b_svtxdls2d;   //!
		TBranch        *b_svtxm;   //!
		TBranch        *b_svtxpt;   //!
		TBranch        *b_svtxmcorr;   //!
		TBranch        *b_nIPtrk;   //!
		TBranch        *b_nselIPtrk;   //!
		TBranch        *b_mue;   //!
		TBranch        *b_mupt;   //!
		TBranch        *b_mueta;   //!
		TBranch        *b_muphi;   //!
		TBranch        *b_mudr;   //!
		TBranch        *b_muptrel;   //!
		TBranch        *b_muchg;   //!

		hiJet(TTree* tree);
		virtual ~hiJet();
		virtual Int_t    Cut(int ijet);
		virtual Int_t    GetEntry(Long64_t entry);
		virtual Long64_t LoadTree(Long64_t entry);
		virtual void     Init(TTree *tree);
		//   virtual void     Loop();
		virtual Bool_t   Notify();
		virtual void     Show(Long64_t entry = -1);
};

hiJet::hiJet(TTree* tree) : fChain(0) 
{
	if (tree == 0) {
	}else {
		Init(tree);
	}
}

hiJet::~hiJet()
{
	if (!fChain) return;
	delete fChain->GetCurrentFile();
}

Int_t hiJet::GetEntry(Long64_t entry)
{
	// Read contents of entry.
	if (!fChain) return 0;
	return fChain->GetEntry(entry);
}
Long64_t hiJet::LoadTree(Long64_t entry)
{
	// Set the environment to read one entry
	if (!fChain) return -5;
	Long64_t centry = fChain->LoadTree(entry);
	if (centry < 0) return centry;
	if (fChain->GetTreeNumber() != fCurrent) {
		fCurrent = fChain->GetTreeNumber();
		Notify();
	}
	return centry;
}

void hiJet::Init(TTree *tree)
{
	// The Init() function is called when the selector needs to initialize
	// a new tree or chain. Typically here the branch addresses and branch
	// pointers of the tree will be set.
	// It is normally not necessary to make changes to the generated
	// code, but the routine can be extended by the user if needed.
	// Init() will be called many times when running on PROOF
	// (once per file to be processed).

	// Set branch addresses and branch pointers
	if (!tree) return;
	fChain = tree;
	fCurrent = -1;
	fChain->SetMakeClass(1);

	fChain->SetBranchAddress("run", &run, &b_run);
	fChain->SetBranchAddress("evt", &evt, &b_evt);
	fChain->SetBranchAddress("lumi", &lumi, &b_lumi);
	fChain->SetBranchAddress("nref", &nref, &b_nref);
	fChain->SetBranchAddress("rawpt", rawpt, &b_rawpt);
	fChain->SetBranchAddress("jtpt", jtpt, &b_jtpt);
	fChain->SetBranchAddress("jteta", jteta, &b_jteta);
	fChain->SetBranchAddress("jty", jty, &b_jty);
	fChain->SetBranchAddress("jtphi", jtphi, &b_jtphi);
	fChain->SetBranchAddress("jtpu", jtpu, &b_jtpu);
	fChain->SetBranchAddress("jtm", jtm, &b_jtm);
	fChain->SetBranchAddress("jtarea", jtarea, &b_jtarea);
	fChain->SetBranchAddress("WTAeta", WTAeta, &b_WTAeta);
	fChain->SetBranchAddress("WTAphi", WTAphi, &b_WTAphi);
	fChain->SetBranchAddress("jtPfCHF", jtPfCHF, &b_jtPfCHF);
	fChain->SetBranchAddress("jtPfNHF", jtPfNHF, &b_jtPfNHF);
	fChain->SetBranchAddress("jtPfCEF", jtPfCEF, &b_jtPfCEF);
	fChain->SetBranchAddress("jtPfNEF", jtPfNEF, &b_jtPfNEF);
	fChain->SetBranchAddress("jtPfMUF", jtPfMUF, &b_jtPfMUF);
	fChain->SetBranchAddress("jtPfCHM", jtPfCHM, &b_jtPfCHM);
	fChain->SetBranchAddress("jtPfNHM", jtPfNHM, &b_jtPfNHM);
	fChain->SetBranchAddress("jtPfCEM", jtPfCEM, &b_jtPfCEM);
	fChain->SetBranchAddress("jtPfNEM", jtPfNEM, &b_jtPfNEM);
	fChain->SetBranchAddress("jtPfMUM", jtPfMUM, &b_jtPfMUM);
	fChain->SetBranchAddress("jttau1", jttau1, &b_jttau1);
	fChain->SetBranchAddress("jttau2", jttau2, &b_jttau2);
	fChain->SetBranchAddress("jttau3", jttau3, &b_jttau3);
	fChain->SetBranchAddress("trackMax", trackMax, &b_trackMax);
	fChain->SetBranchAddress("trackSum", trackSum, &b_trackSum);
	fChain->SetBranchAddress("trackN", trackN, &b_trackN);
	fChain->SetBranchAddress("trackHardSum", trackHardSum, &b_trackHardSum);
	fChain->SetBranchAddress("trackHardN", trackHardN, &b_trackHardN);
	fChain->SetBranchAddress("chargedMax", chargedMax, &b_chargedMax);
	fChain->SetBranchAddress("chargedSum", chargedSum, &b_chargedSum);
	fChain->SetBranchAddress("chargedN", chargedN, &b_chargedN);
	fChain->SetBranchAddress("chargedHardSum", chargedHardSum, &b_chargedHardSum);
	fChain->SetBranchAddress("chargedHardN", chargedHardN, &b_chargedHardN);
	fChain->SetBranchAddress("photonMax", photonMax, &b_photonMax);
	fChain->SetBranchAddress("photonSum", photonSum, &b_photonSum);
	fChain->SetBranchAddress("photonN", photonN, &b_photonN);
	fChain->SetBranchAddress("photonHardSum", photonHardSum, &b_photonHardSum);
	fChain->SetBranchAddress("photonHardN", photonHardN, &b_photonHardN);
	fChain->SetBranchAddress("neutralMax", neutralMax, &b_neutralMax);
	fChain->SetBranchAddress("neutralSum", neutralSum, &b_neutralSum);
	fChain->SetBranchAddress("neutralN", neutralN, &b_neutralN);
	fChain->SetBranchAddress("hcalSum", hcalSum, &b_hcalSum);
	fChain->SetBranchAddress("ecalSum", ecalSum, &b_ecalSum);
	fChain->SetBranchAddress("eMax", eMax, &b_eMax);
	fChain->SetBranchAddress("eSum", eSum, &b_eSum);
	fChain->SetBranchAddress("eN", eN, &b_eN);
	fChain->SetBranchAddress("muMax", muMax, &b_muMax);
	fChain->SetBranchAddress("muSum", muSum, &b_muSum);
	fChain->SetBranchAddress("muN", muN, &b_muN);
	fChain->SetBranchAddress("discr_ssvHighEff", discr_ssvHighEff, &b_discr_ssvHighEff);
	fChain->SetBranchAddress("discr_ssvHighPur", discr_ssvHighPur, &b_discr_ssvHighPur);
	fChain->SetBranchAddress("discr_csvV1", discr_csvV1, &b_discr_csvV1);
	fChain->SetBranchAddress("discr_csvV2", discr_csvV2, &b_discr_csvV2);
	fChain->SetBranchAddress("discr_muByIp3", discr_muByIp3, &b_discr_muByIp3);
	fChain->SetBranchAddress("discr_muByPt", discr_muByPt, &b_discr_muByPt);
	fChain->SetBranchAddress("discr_prob", discr_prob, &b_discr_prob);
	fChain->SetBranchAddress("discr_probb", discr_probb, &b_discr_probb);
	fChain->SetBranchAddress("discr_tcHighEff", discr_tcHighEff, &b_discr_tcHighEff);
	fChain->SetBranchAddress("discr_tcHighPur", discr_tcHighPur, &b_discr_tcHighPur);
	fChain->SetBranchAddress("ndiscr_ssvHighEff", ndiscr_ssvHighEff, &b_ndiscr_ssvHighEff);
	fChain->SetBranchAddress("ndiscr_ssvHighPur", ndiscr_ssvHighPur, &b_ndiscr_ssvHighPur);
	fChain->SetBranchAddress("ndiscr_csvV1", ndiscr_csvV1, &b_ndiscr_csvV1);
	fChain->SetBranchAddress("ndiscr_csvV2", ndiscr_csvV2, &b_ndiscr_csvV2);
	fChain->SetBranchAddress("ndiscr_muByPt", ndiscr_muByPt, &b_ndiscr_muByPt);
	fChain->SetBranchAddress("pdiscr_csvV1", pdiscr_csvV1, &b_pdiscr_csvV1);
	fChain->SetBranchAddress("pdiscr_csvV2", pdiscr_csvV2, &b_pdiscr_csvV2);
	fChain->SetBranchAddress("nsvtx", nsvtx, &b_nsvtx);
	fChain->SetBranchAddress("svtxntrk", svtxntrk, &b_svtxntrk);
	fChain->SetBranchAddress("svtxdl", svtxdl, &b_svtxdl);
	fChain->SetBranchAddress("svtxdls", svtxdls, &b_svtxdls);
	fChain->SetBranchAddress("svtxdl2d", svtxdl2d, &b_svtxdl2d);
	fChain->SetBranchAddress("svtxdls2d", svtxdls2d, &b_svtxdls2d);
	fChain->SetBranchAddress("svtxm", svtxm, &b_svtxm);
	fChain->SetBranchAddress("svtxpt", svtxpt, &b_svtxpt);
	fChain->SetBranchAddress("svtxmcorr", svtxmcorr, &b_svtxmcorr);
	fChain->SetBranchAddress("nIPtrk", nIPtrk, &b_nIPtrk);
	fChain->SetBranchAddress("nselIPtrk", nselIPtrk, &b_nselIPtrk);
	fChain->SetBranchAddress("mue", mue, &b_mue);
	fChain->SetBranchAddress("mupt", mupt, &b_mupt);
	fChain->SetBranchAddress("mueta", mueta, &b_mueta);
	fChain->SetBranchAddress("muphi", muphi, &b_muphi);
	fChain->SetBranchAddress("mudr", mudr, &b_mudr);
	fChain->SetBranchAddress("muptrel", muptrel, &b_muptrel);
	fChain->SetBranchAddress("muchg", muchg, &b_muchg);
	Notify();
}

Bool_t hiJet::Notify()
{
	// The Notify() function is called when a new file is opened. This
	// can be either for a new TTree in a TChain or when when a new TTree
	// is started when using PROOF. It is normally not necessary to make changes
	// to the generated code, but the routine can be extended by the
	// user if needed. The return value is currently not used.

	return kTRUE;
}

void hiJet::Show(Long64_t entry)
{
	// Print contents of entry.
	// If entry is not specified, print current entry
	if (!fChain) return;
	fChain->Show(entry);
}
Int_t hiJet::Cut(int i)
{
	// This function may be called from Loop.
	// returns  1 if entry is accepted.
	// returns -1 otherwise.
	if(jtpt[i] < 120) return -1;
	if(fabs(jteta[i]) > 1.6) return -1;
	return 1;
}
#endif // #ifdef hiJet_cxx
