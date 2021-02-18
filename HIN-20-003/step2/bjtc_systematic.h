
#include "myProcesses/jtc/plugin/workflow.h"
#include "myProcesses/jtc/plugin/jtcTH1Player.h"
//#include "myProcesses/jtc/plugin/Utility.h"
//#include "myProcesses/jtc/plugin/plotLib.h"
#include "myProcesses/jtc/plugin/plotSetting.h"
#include "myProcesses/liteFrame/plugin/plots/plotManager.h"

class bjtc_systematic: public analyzer{
	public:
		bjtc_systematic(TString name, workflow &b0, ParaSet &ps0 ):analyzer(name,b0, ps0){
			npt = ps0.getPara<int>("npt");
			ncent = ps0.getPara<int>("ncent");
		}
		~bjtc_systematic(){}

		void trigger_study();
		virtual void analyze();


		TString format = ".jpg", step2fname, step2uncert, systematic;
		int npt, ncent;
		TDirectory* _dir_; //working folder.
		TFile *fstep2, *funcert, *fsystem;
};

float dr_fine[] = {0.,0.02, 0.04,0.06, 0.08,0.1,0.125,0.15,0.2,0.25,0.3,0.35,0.4,0.5,0.6,0.7,0.8,0.9,1.,1.2, 1.5, 2, 2.5};
float dr_trk[] = {0.,0.02, 0.04,0.06, 0.08,0.1,0.125,0.15,0.2,0.25,0.3,0.35,0.4,0.5,0.6,0.8,1., 2.5};
int ndr_trk = 17;
int ndr_fine = 22;

void bjtc_systematic::trigger_study(){
	auto jet60 = new jtcTH1Player("correlations_HIHardProbeLower_jet60/tagged"+reco_tag(1,1)+"_sig_p2_dr_*_*",npt, ncent);
	auto jet80 = new jtcTH1Player("correlations_HIHardProbeLower_jet80and100/tagged"+reco_tag(1,1)+"_sig_p2_dr_*_*",npt, ncent);
	jet60->autoLoad(fstep2);
	jet80->autoLoad(fstep2);
	auto j60 = jet60->contractX("jet60");
	auto j80 = jet80->contractX("jet80");
	auto inc_jet60 = new jtcTH1Player("correlations_HIHardProbeLower_jet60/incl"+reco_tag(1,1)+"_sig_p2_dr_*_*",npt, ncent);
	auto inc_jet80 = new jtcTH1Player("correlations_HIHardProbeLower_jet80and100/incl"+reco_tag(1,1)+"_sig_p2_dr_*_*",npt, ncent);
	inc_jet60->autoLoad(fstep2);
	inc_jet80->autoLoad(fstep2);
	auto ij60 = inc_jet60->contractX("jet60");
	auto ij80 = inc_jet80->contractX("jet80");

	plot_overlay("systUncert_incl_trigger",fig_output, ij60, "jet60", ij80, "jet80and100",0, 0.99);
	plot_overlay("systUncert_tagged_trigger",fig_output, j60, "jet60", j80, "jet80and100",0, 0.99);
}

void bjtc_systematic::analyze(){
	cout<<"Opening file: "<<output+"/"+step2fname+".root"<<endl;
	fstep2= TFile::Open(output+"/"+step2fname+".root");
	cout<<"Opening file: "<<output+"/"+step2uncert+".root"<<endl;
	funcert= TFile::Open(output+"/"+step2uncert+".root");
	_dir_ = f->mkdir(_name_);
	if(_dir_==0) _dir_=(TDirectory*) f->Get(_name_);
	f->cd(_name_);

	trigger_study();
}

