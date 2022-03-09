
#include "myProcesses/jtc/plugin/jtcTH1Player.h"

void merge(TString folder, TString name, TFile *f, TFile *wf){
	auto jsubeS = new jtcTH1Player(folder+"/incl_sube0_"+name+"_pTweighted_P*_C*", 6,3);
	auto jNsubeS= new jtcTH1Player(folder+"/incl_subeN0_"+name+"_pTweighted_P*_C*", 6,3);
	auto jsubeM = new jtcTH1Player(folder+"/incl_sube0_"+name+"_mixing_P*_C*", 6,3);
	auto jNsubeM= new jtcTH1Player(folder+"/incl_subeN0_"+name+"_mixing_P*_C*", 6,3);
	jsubeS->autoLoad(f);
	jNsubeS->autoLoad(f);
	jsubeM->autoLoad(f);
	jNsubeM->autoLoad(f);
	
	auto js = (jtcTH1Player*) jsubeS->add2("incl_"+name+"_pTweighted", *jNsubeS, 1,1);
	auto jm = (jtcTH1Player*) jsubeM->add2("incl_"+name+"_mixing", *jNsubeM, 1,1);
	js->setName("incl_"+name+"_pTweighted_P*_C*");
	jm->setName("incl_"+name+"_mixing_P*_C*");
	auto dir = wf->mkdir(folder);
	wf->cd(folder);
	js->setDirectory(dir);
	js->write();
	jm->setDirectory(dir);
	jm->write();
}
void merge_sube2std(){
	auto f = TFile::Open("../data/correlation/newbin/bjtc_djetMC_sube_c5shift_c3bin_new.root");
	auto wf = new TFile("bjtc_inclusive_combined.root", "recreate");
	merge("incl_GenJet_GenTrk", "GenJet_GenTrk", f, wf);
	merge("incl_RecoJet_GenTrk", "RecoJet_GenTrk", f, wf);

	auto hvz = (TH1D*) f->Get("vzInfo");
	auto hc = (TH1D*) f->Get("centInfo");
	auto hpthat = (TH1D*) f->Get("pthatInfo");
	wf->cd();
	hc->Write();
	hvz->Write();
	hpthat->Write();
	auto dir = wf->mkdir("jetQASets");
	wf->cd("jetQASets");
	for(auto i=0; i<3; i++){
		auto h = (TH1D*) f->Get(Form("jetQASets/incl_RecoLevel_pt_C%d",i));
		h->Write();
		h = (TH1D*) f->Get(Form("jetQASets/incl_RecoLevel_eta_C%d",i));
		h->Write();
		h = (TH1D*) f->Get(Form("jetQASets/incl_RecoLevel_phi_C%d",i));
		h->Write();
		h = (TH1D*) f->Get(Form("jetQASets/incl_GenLevel_pt_C%d",i));
		h->Write();
		h = (TH1D*) f->Get(Form("jetQASets/incl_GenLevel_eta_C%d",i));
		h->Write();
		h = (TH1D*) f->Get(Form("jetQASets/incl_GenLevel_phi_C%d",i));
		h->Write();
	}
}
