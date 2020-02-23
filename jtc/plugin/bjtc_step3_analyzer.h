
#include "myProcesses/jtc/plugin/workflow.h"
#include "myProcesses/jtc/plugin/Utility.h"
#include "myProcesses/jtc/plugin/jtcTH1Player.h"

class bjtc_step3_analyzer: public analyzer{
	public:
	bjtc_step3_analyzer(TString name, ParaSet &ps ):analyzer(name, ps){}
	~bjtc_step3_analyzer(){}
	void get_tracking_corr(TString sname);
	jtcTH1Player* get_jff_corr(TString sname, TString);
	virtual void analyze();
};

void bjtc_step3_analyzer::get_tracking_corr(TString sname){
	jtcTH1Player reco(sname+reco_tag(1,1)+"_sig_p1_dr_*_*",base->npt, base->ncent);
	jtcTH1Player gen(sname+reco_tag(1,0)+"_sig_p1_dr_*_*",base->npt, base->ncent);
	reco.autoLoad(base->wf);
	gen.autoLoad(base->wf);
}

jtcTH1Player* bjtc_step3_analyzer::get_jff_corr(TString sname, TString corr_name){
	jtcTH1Player reco(sname+reco_tag(1,0)+"_sig_p1_dr_*_*",base->npt, base->ncent);
	jtcTH1Player gen(sname+reco_tag(0,0)+"_sig_p1_dr_*_*",base->npt, base->ncent);
	reco.autoLoad(base->wf);
	gen.autoLoad(base->wf);
	jtcTH1Player* js =(jtcTH1Player*) reco.divide(gen);
	js->setName(corr_name);
	base->wf->cd();
	js->write();
	return js;
}

void bjtc_step3_analyzer::analyze(){
	get_jff_corr("incl", "incl_JFFCorr");
	//get_tracking_corr("incl");
}

