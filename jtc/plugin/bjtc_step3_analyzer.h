
#include "myProcesses/jtc/plugin/workflow.h"
#include "myProcesses/jtc/plugin/Utility.h"
#include "myProcesses/jtc/plugin/plotLib.h"
#include "myProcesses/jtc/plugin/jtcTH1Player.h"

class bjtc_step3_analyzer: public analyzer{
	public:
	bjtc_step3_analyzer(TString name, workflow &b0, ParaSet &ps ):analyzer(name,b0, ps){}
	~bjtc_step3_analyzer(){}
	jtcTH1Player* get_tracking_corr(TString sname, TString);
	jtcTH1Player* get_jff_corr(TString sname, TString);
	virtual void analyze();
	
	TString format = ".jpg";
};

jtcTH1Player* bjtc_step3_analyzer::get_tracking_corr(TString sname, TString corr_name){
	jtcTH1Player reco(sname+reco_tag(1,1)+"_sig_p1_dr_*_*",base->npt, base->ncent);
	jtcTH1Player gen(sname+reco_tag(1,0)+"_sig_p1_dr_*_*",base->npt, base->ncent);
	reco.autoLoad(base->wf);
	gen.autoLoad(base->wf);
	jtcTH1Player* js =(jtcTH1Player*) reco.divide(gen);
	js->setName(corr_name);
	auto c =new multi_pads<base_pad>("trkCorr_"+corr_name, "", base->npt, base->ncent);
	c->setXrange(0,0.99);
	c->setYrange(0,1);
	c->doHIarrange = 1;
	c->addm2TH1(js);
	c->draw();
	c->SaveAs(fig_output+"/trkCorr_"+corr_name+format);
	base->wf->cd();
	js->write();
	return js;
}

jtcTH1Player* bjtc_step3_analyzer::get_jff_corr(TString sname, TString corr_name){
	jtcTH1Player reco(sname+reco_tag(1,0)+"_sig_p1_dr_*_*",base->npt, base->ncent);
	jtcTH1Player gen(sname+reco_tag(0,0)+"_sig_p1_dr_*_*",base->npt, base->ncent);
	reco.autoLoad(base->wf);
	gen.autoLoad(base->wf);
	jtcTH1Player* js =(jtcTH1Player*) reco.divide(gen);
	js->setName(corr_name);
	auto c =new multi_pads<base_pad>("JFFCorr_"+corr_name, "", base->npt, base->ncent);
	c->setXrange(0,0.99);
	c->setYrange(0,1);
	c->doHIarrange = 1;
	c->addm2TH1(js);
	c->draw();
	c->SaveAs(fig_output+"/JFFCorr_"+corr_name+format);
	base->wf->cd();
	js->write();
	return js;
}

void bjtc_step3_analyzer::analyze(){
	get_jff_corr("incl", "incl_JFFCorr");
	get_tracking_corr("incl", "incl_trkCorr");
	//get_tracking_corr("incl");
}

