//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Tue Oct  1 05:09:55 2019 by ROOT version 6.12/07
// from TTree HltTree/
// found on file: /eos/cms/store/group/phys_heavyions/jviinika/PbPbData2018-HIRun2018A-HIHardProbes-04Apr2019-v1_80or100caloJetTrigger_newJetsPt25_103X_2019-09-13/part0/HiForestAOD_PbPbData2018_104.root
//////////////////////////////////////////////////////////

#ifndef hltTree_h
#define hltTree_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.

class hltTree {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

// Fixed size dimensions of array or collections stored in the TTree if any.

   // Declaration of leaf types
   Int_t           superFilterPath;
   Int_t           ana_step;
   Int_t           pclusterCompatibilityFilter;
   Int_t           pprimaryVertexFilter;
   Int_t           pBeamScrapingFilter;
   Int_t           collisionEventSelectionAOD;
   Int_t           collisionEventSelectionAODv2;
   Int_t           phfCoincFilter1Th3;
   Int_t           phfCoincFilter2Th3;
   Int_t           phfCoincFilter3Th3;
   Int_t           phfCoincFilter4Th3;
   Int_t           phfCoincFilter5Th3;
   Int_t           phfCoincFilter1Th4;
   Int_t           phfCoincFilter2Th4;
   Int_t           phfCoincFilter3Th4;
   Int_t           phfCoincFilter4Th4;
   Int_t           phfCoincFilter5Th4;
   Int_t           phfCoincFilter1Th5;
   Int_t           phfCoincFilter4Th2;
   Int_t           pVertexFilterCutG;
   Int_t           pVertexFilterCutGloose;
   Int_t           pVertexFilterCutGtight;
   Int_t           pVertexFilterCutGplus;
   Int_t           pVertexFilterCutE;
   Int_t           pVertexFilterCutEandG;
   Int_t           pHBHENoiseFilterResultProducer;
   Int_t           HBHENoiseFilterResult;
   Int_t           HBHENoiseFilterResultRun1;
   Int_t           HBHENoiseFilterResultRun2Loose;
   Int_t           HBHENoiseFilterResultRun2Tight;
   Int_t           HBHEIsoNoiseFilterResult;

   // List of branches
   TBranch        *b_superFilterPath;   //!
   TBranch        *b_ana_step;   //!
   TBranch        *b_pclusterCompatibilityFilter;   //!
   TBranch        *b_pprimaryVertexFilter;   //!
   TBranch        *b_pBeamScrapingFilter;   //!
   TBranch        *b_collisionEventSelectionAOD;   //!
   TBranch        *b_collisionEventSelectionAODv2;   //!
   TBranch        *b_phfCoincFilter1Th3;   //!
   TBranch        *b_phfCoincFilter2Th3;   //!
   TBranch        *b_phfCoincFilter3Th3;   //!
   TBranch        *b_phfCoincFilter4Th3;   //!
   TBranch        *b_phfCoincFilter5Th3;   //!
   TBranch        *b_phfCoincFilter1Th4;   //!
   TBranch        *b_phfCoincFilter2Th4;   //!
   TBranch        *b_phfCoincFilter3Th4;   //!
   TBranch        *b_phfCoincFilter4Th4;   //!
   TBranch        *b_phfCoincFilter5Th4;   //!
   TBranch        *b_phfCoincFilter1Th5;   //!
   TBranch        *b_phfCoincFilter4Th2;   //!
   TBranch        *b_pVertexFilterCutG;   //!
   TBranch        *b_pVertexFilterCutGloose;   //!
   TBranch        *b_pVertexFilterCutGtight;   //!
   TBranch        *b_pVertexFilterCutGplus;   //!
   TBranch        *b_pVertexFilterCutE;   //!
   TBranch        *b_pVertexFilterCutEandG;   //!
   TBranch        *b_pHBHENoiseFilterResultProducer;   //!
   TBranch        *b_HBHENoiseFilterResult;   //!
   TBranch        *b_HBHENoiseFilterResultRun1;   //!
   TBranch        *b_HBHENoiseFilterResultRun2Loose;   //!
   TBranch        *b_HBHENoiseFilterResultRun2Tight;   //!
   TBranch        *b_HBHEIsoNoiseFilterResult;   //!

   hltTree(TTree *tree=0);
   virtual ~hltTree();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
//   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

hltTree::hltTree(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      }else {
	      Init(tree);
   }
}

hltTree::~hltTree()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t hltTree::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t hltTree::LoadTree(Long64_t entry)
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

void hltTree::Init(TTree *tree)
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

   fChain->SetBranchAddress("superFilterPath", &superFilterPath, &b_superFilterPath);
   fChain->SetBranchAddress("ana_step", &ana_step, &b_ana_step);
   fChain->SetBranchAddress("pclusterCompatibilityFilter", &pclusterCompatibilityFilter, &b_pclusterCompatibilityFilter);
   fChain->SetBranchAddress("pprimaryVertexFilter", &pprimaryVertexFilter, &b_pprimaryVertexFilter);
   fChain->SetBranchAddress("pBeamScrapingFilter", &pBeamScrapingFilter, &b_pBeamScrapingFilter);
   fChain->SetBranchAddress("collisionEventSelectionAOD", &collisionEventSelectionAOD, &b_collisionEventSelectionAOD);
   fChain->SetBranchAddress("collisionEventSelectionAODv2", &collisionEventSelectionAODv2, &b_collisionEventSelectionAODv2);
   fChain->SetBranchAddress("phfCoincFilter1Th3", &phfCoincFilter1Th3, &b_phfCoincFilter1Th3);
   fChain->SetBranchAddress("phfCoincFilter2Th3", &phfCoincFilter2Th3, &b_phfCoincFilter2Th3);
   fChain->SetBranchAddress("phfCoincFilter3Th3", &phfCoincFilter3Th3, &b_phfCoincFilter3Th3);
   fChain->SetBranchAddress("phfCoincFilter4Th3", &phfCoincFilter4Th3, &b_phfCoincFilter4Th3);
   fChain->SetBranchAddress("phfCoincFilter5Th3", &phfCoincFilter5Th3, &b_phfCoincFilter5Th3);
   fChain->SetBranchAddress("phfCoincFilter1Th4", &phfCoincFilter1Th4, &b_phfCoincFilter1Th4);
   fChain->SetBranchAddress("phfCoincFilter2Th4", &phfCoincFilter2Th4, &b_phfCoincFilter2Th4);
   fChain->SetBranchAddress("phfCoincFilter3Th4", &phfCoincFilter3Th4, &b_phfCoincFilter3Th4);
   fChain->SetBranchAddress("phfCoincFilter4Th4", &phfCoincFilter4Th4, &b_phfCoincFilter4Th4);
   fChain->SetBranchAddress("phfCoincFilter5Th4", &phfCoincFilter5Th4, &b_phfCoincFilter5Th4);
   fChain->SetBranchAddress("phfCoincFilter1Th5", &phfCoincFilter1Th5, &b_phfCoincFilter1Th5);
   fChain->SetBranchAddress("phfCoincFilter4Th2", &phfCoincFilter4Th2, &b_phfCoincFilter4Th2);
   fChain->SetBranchAddress("pVertexFilterCutG", &pVertexFilterCutG, &b_pVertexFilterCutG);
   fChain->SetBranchAddress("pVertexFilterCutGloose", &pVertexFilterCutGloose, &b_pVertexFilterCutGloose);
   fChain->SetBranchAddress("pVertexFilterCutGtight", &pVertexFilterCutGtight, &b_pVertexFilterCutGtight);
   fChain->SetBranchAddress("pVertexFilterCutGplus", &pVertexFilterCutGplus, &b_pVertexFilterCutGplus);
   fChain->SetBranchAddress("pVertexFilterCutE", &pVertexFilterCutE, &b_pVertexFilterCutE);
   fChain->SetBranchAddress("pVertexFilterCutEandG", &pVertexFilterCutEandG, &b_pVertexFilterCutEandG);
   fChain->SetBranchAddress("pHBHENoiseFilterResultProducer", &pHBHENoiseFilterResultProducer, &b_pHBHENoiseFilterResultProducer);
   fChain->SetBranchAddress("HBHENoiseFilterResult", &HBHENoiseFilterResult, &b_HBHENoiseFilterResult);
   fChain->SetBranchAddress("HBHENoiseFilterResultRun1", &HBHENoiseFilterResultRun1, &b_HBHENoiseFilterResultRun1);
   fChain->SetBranchAddress("HBHENoiseFilterResultRun2Loose", &HBHENoiseFilterResultRun2Loose, &b_HBHENoiseFilterResultRun2Loose);
   fChain->SetBranchAddress("HBHENoiseFilterResultRun2Tight", &HBHENoiseFilterResultRun2Tight, &b_HBHENoiseFilterResultRun2Tight);
   fChain->SetBranchAddress("HBHEIsoNoiseFilterResult", &HBHEIsoNoiseFilterResult, &b_HBHEIsoNoiseFilterResult); Notify();
}

Bool_t hltTree::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void hltTree::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t hltTree::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
	if(pprimaryVertexFilter == 0 )  return -1;
	if(HBHENoiseFilterResultRun2Loose == 0 )  return -1;
	if(phfCoincFilter3Th3 == 0 )  return -1;
	if(collisionEventSelectionAODv2 == 0 )  return -1;
	if(pclusterCompatibilityFilter == 0 )  return -1;
   return 1;
}
#endif // #ifdef hltTree_cxx
