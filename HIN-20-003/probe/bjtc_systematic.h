
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
		void syst_jec(TString jetname);
		virtual void analyze();


		TString format = ".png", step2fname, step2uncert, systematic, step2nominal;
		int npt, ncent;
		TDirectory* _dir_; //working folder.
		TFile *fstep2, *funcert, *fsystem, *fstep2nominal;
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
	ij60->at(0,0)->SetTitle("Signal pT weighted, Cent:0-30%, pt>1 GeV");
	ij60->at(0,1)->SetTitle("Signal pT weighted, Cent:30-90%, pt>1 GeV");
	j60->at(0,0)->SetTitle("Signal pT weighted, Cent:0-30%, pt>1 GeV");
	j60->at(0,1)->SetTitle("Signal pT weighted, Cent:30-90%, pt>1 GeV");

	plot_overlay("systUncert_incl_trigger",fig_output, ij60, "jet60", ij80, "jet80and100",0, 0.99);
	plot_overlay("systUncert_tagged_trigger",fig_output, j60, "jet60", j80, "jet80and100",0, 0.99);
}
void bjtc_systematic::syst_jec(TString jetname){
	//auto nojs = new jtcTH1Player("correlations_HIHardProbe_allTrig/"+jetname+reco_tag(1,1)+"_sig_p2_dr_*_*",npt, ncent);
	//auto nojs = new jtcTH1Player("correlations_HIHardProbe_jet80or100_85p/"+jetname+reco_tag(1,1)+"_sig_p2_dr_*_*",npt, ncent);
	auto nojs = new jtcTH1Player("correlations_HIHardProbe_jecRef/"+jetname+reco_tag(1,1)+"_sig_p2_dr_*_*",npt, ncent);
	auto jecup = new jtcTH1Player("correlations_HIHardProbe_jecUp/"+jetname+reco_tag(1,1)+"_sig_p2_dr_*_*",npt, ncent);
	auto jecdown = new jtcTH1Player("correlations_HIHardProbe_jecDown/"+jetname+reco_tag(1,1)+"_sig_p2_dr_*_*",npt, ncent);
	jecup->autoLoad(fstep2);
	jecdown->autoLoad(fstep2);
	nojs->autoLoad(fstep2);
	auto up = jecup->contractX("jecup");
	auto down = jecdown->contractX("jecdown");
	auto nominal = nojs->contractX("nominal");

	auto c = new plotManager();
	c->initOverlayPad("canvas_jeu_"+jetname, "", 1, 2);
	for(int j=0; j<2; j++){
			
		c->addHist(nominal->at(0,j), 0, 1-j, "nominal", "pl");
		c->addHist(up->at(0,j),   0, 1-j, "JEU up", "pl");
		c->addHist(down->at(0,j), 0, 1-j, "JEU down", "pl");
		((overlayPad*)c->at(0,1-j))->rymin = 0.8;
		((overlayPad*)c->at(0,1-j))->rymax = 1.2;
	}
	nominal->at(0,0)->SetTitle("Signal pT weighted, Cent:0-30%, pt>1 GeV");
	nominal->at(0,1)->SetTitle("Signal pT weighted, Cent:30-90%, pt>1 GeV");
	c->setXrange(0,.99);
	c->draw();	
	c->drawLegend();
	c->save(fig_output+"/systUncert_JEC_"+jetname+format);

}

void bjtc_systematic::analyze(){
	cout<<"Opening file: "<<output+"/"+step2fname+".root"<<endl;
	fstep2= TFile::Open(output+"/"+step2fname+".root");
	cout<<"Opening file: "<<output+"/"+step2uncert+".root"<<endl;
	funcert= TFile::Open(output+"/"+step2uncert+".root");
	_dir_ = f->mkdir(_name_);
	fstep2nominal = TFile::Open(step2nominal);
	if(_dir_==0) _dir_=(TDirectory*) f->Get(_name_);
	f->cd(_name_);

	trigger_study();
	syst_jec("incl");
	syst_jec("tagged");
}

