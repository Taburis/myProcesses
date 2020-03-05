
#include "myProcesses/jtc/plugin/workflow.h"
#include "myProcesses/jtc/plugin/Utility.h"
#include "myProcesses/jtc/plugin/plotLib.h"
#include "myProcesses/jtc/plugin/jtcTH1Player.h"

class bjtc_step3_analyzer: public analyzer{
	public:
		bjtc_step3_analyzer(TString name, workflow &b0, ParaSet &ps ):analyzer(name,b0, ps){}
		~bjtc_step3_analyzer(){}
		jtcTH1Player* get_tracking_corr(TString sname);
		jtcTH1Player* get_jff_corr(TString sname);
		jtcTH1Player* get_tagging_biasCorr();
		virtual void analyze();

		TString format = ".jpg";
};

jtcTH1Player* bjtc_step3_analyzer::get_tagging_biasCorr(){
	jtcTH1Player tag("tagTrue"+reco_tag(1,0)+"_sig_p0_dr_*_*",base->npt, base->ncent);
	jtcTH1Player gen("trueB"+reco_tag(0,0)+"_sig_p0_dr_*_*",base->npt, base->ncent);
	//jtcTH1Player tag("tagTrue"+reco_tag(1,0)+"_sig_p1_dr_*_*",base->npt, base->ncent);
	//jtcTH1Player gen("trueB"+reco_tag(0,0)+"_sig_p1_dr_*_*",base->npt, base->ncent);
	tag.autoLoad(base->wf);
	gen.autoLoad(base->wf);
	auto gen0 = gen.contractY("contractY_gen_true");
	auto tag0 = tag.contractY("contractY_gen_tag");
	gen0->duplicateY("summed_gen_true", base->ncent);
	tag0->duplicateY("summed_gen_tag", base->ncent);
	jtcTH1Player* js =(jtcTH1Player*) tag0->divide(*gen0, "B");
	jtcTH1Player* bias =(jtcTH1Player*) tag.divide(gen, "B");
	TString corr_name = "tagBias";
	js->setName(corr_name);
	auto eff_smth  = js->clone("tagBias_smth");
	eff_smth->smooth();
	//auto c =new multi_pads<base_pad>("bias_"+corr_name, "", base->npt, 1);
	auto c =new multi_pads<base_pad>("bias_"+corr_name, "", base->npt, base->ncent);
	c->setXrange(0,0.99);
	c->setYrange(0.5,3.);
	c->xtitle="#Delta r";
	c->ytitle="Tagging Bias";
	c->doHIarrange = 1;
//	c->addm2TH1(js);
	c->addm2TH1(eff_smth);
	c->addm2TH1(bias);
	c->addLegend("upperright");
	c->labelHist("Smthed bias",0);
	c->labelHist("tag bias",1);
	c->draw();
	c->SaveAs(fig_output+"/recoJet_"+corr_name+format);
	base->wf->cd();
	js->write();
	eff_smth->write();
	return js;
}

jtcTH1Player* bjtc_step3_analyzer::get_tracking_corr(TString sname){
	TString corr_name = sname+"_trkEff";
	jtcTH1Player reco(sname+reco_tag(1,1)+"_sig_p1_dr_*_*",base->npt, base->ncent);
	jtcTH1Player gen(sname+reco_tag(1,0)+"_sig_p1_dr_*_*",base->npt, base->ncent);
	reco.autoLoad(base->wf);
	gen.autoLoad(base->wf);
	jtcTH1Player* js =(jtcTH1Player*) reco.divide(gen, "B");
	js->setName(corr_name);
	auto tkcorr = js->clone(sname+"_trkEff_smth");
	for(int i=0; i<tkcorr->Ncol();++i){
		for(int j=0; j<tkcorr->Nrow();++j){
			tkcorr->at(j,i)->Smooth();
		}
	}
	auto c =new multi_pads<base_pad>("canvas_"+corr_name, "", base->npt, base->ncent);
	c->setXrange(0,0.99);
	c->setYrange(0.3,1);
	c->ytitle="Tracking Efficiency";
	c->xtitle="#Delta r";
	c->doHIarrange = 1;
	c->addm2TH1(js);
	c->addm2TH1(tkcorr);
	c->addLegend("upperright");
	c->labelHist("MC based",0);
	c->labelHist("Smoothed",1);
	c->draw();
	c->SaveAs(fig_output+"/tracking_"+corr_name+format);
	base->wf->cd();
	js->write();
	return js;
}

jtcTH1Player* bjtc_step3_analyzer::get_jff_corr(TString sname){
	TString corr_name = sname+"_JffEffect";
	jtcTH1Player reco(sname+reco_tag(1,0)+"_sig_p1_dr_*_*",base->npt, base->ncent);
	jtcTH1Player gen(sname+reco_tag(0,0)+"_sig_p1_dr_*_*",base->npt, base->ncent);
	reco.autoLoad(base->wf);
	gen.autoLoad(base->wf);
	jtcTH1Player* js =(jtcTH1Player*) reco.divide(gen);
	js->setName(corr_name);
	auto c =new multi_pads<base_pad>("JFFCorr_"+corr_name, "", base->npt, base->ncent);
	c->setXrange(0,0.99);
	c->setYrange(0.5,1.5);
	c->doHIarrange = 1;
	c->xtitle="#Delta r";
	c->addm2TH1(js);
	c->draw();
	c->SaveAs(fig_output+"/JFF_"+corr_name+format);
	base->wf->cd();
	js->write();
	return js;
}

void bjtc_step3_analyzer::analyze(){
	get_jff_corr("incl");
	get_tracking_corr("incl");
	get_tracking_corr("tagged");
	get_tagging_biasCorr();
	get_tracking_corr("incl");
}

