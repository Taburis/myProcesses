
#include "myProcesses/jtc/plugin/workflow.h"
#include "myProcesses/jtc/plugin/Utility.h"
#include "myProcesses/jtc/plugin/plotLib.h"
#include "myProcesses/jtc/plugin/jtcTH1Player.h"

class bjtc_step3_analyzer: public analyzer{
	public:
		bjtc_step3_analyzer(TString name, workflow &b0, ParaSet &ps0 ):analyzer(name,b0, ps0){
			npt = ps0.getPara<int>("npt");
			ncent = ps0.getPara<int>("ncent");
		}
		~bjtc_step3_analyzer(){}
		jtcTH1Player* get_tracking_corr(TString sname);
		jtcTH1Player* get_jff_corr(TString sname);
		jtcTH1Player* get_tagging_biasCorr();
		jtcTH1Player* get_cont_biasCorr();
		virtual void analyze();

		TString format = ".jpg";
		int npt, ncent;
};

jtcTH1Player* bjtc_step3_analyzer::get_tagging_biasCorr(){
	jtcTH1Player tagb("correlations_bjetMC/tagTrue"+reco_tag(0,0)+"_sig_p1_dr_*_*",npt, ncent);
	//jtcTH1Player tagb("correlations_bjetMC/tagged"+reco_tag(0,0)+"_sig_p1_dr_*_*",npt, ncent);
	//jtcTH1Player tag("tagTrue"+reco_tag(1,0)+"_sig_p0_dr_*_*",npt, ncent);
	jtcTH1Player genb("correlations_bjetMC/trueB"+reco_tag(0,0)+"_sig_p1_dr_*_*",npt, ncent);
	//jtcTH1Player tag("tagTrue"+reco_tag(1,0)+"_sig_p1_dr_*_*",npt, ncent);
	//jtcTH1Player gen("trueB"+reco_tag(0,0)+"_sig_p1_dr_*_*",npt, ncent);
	tagb.autoLoad(base->wf);
	genb.autoLoad(base->wf);
	jtcTH1Player tagd("correlations_djetMC/tagTrue"+reco_tag(0,0)+"_sig_p1_dr_*_*",npt, ncent);
	//jtcTH1Player tagd("correlations_djetMC/tagged"+reco_tag(1,0)+"_sig_p1_dr_*_*",npt, ncent);
	jtcTH1Player gend("correlations_djetMC/trueB"+reco_tag(0,0)+"_sig_p1_dr_*_*",npt, ncent);
	tagd.autoLoad(base->wf);
	gend.autoLoad(base->wf);
	
	auto gen =(jtcTH1Player*) (gend+genb);
	auto tag =(jtcTH1Player*) (tagd+tagb);
	auto gen0 = gen->contractY("contractY_gen_true");
	auto tag0 = tag->contractY("contractY_gen_tag");
	gen0->duplicateY("summed_gen_true", ncent);
	tag0->duplicateY("summed_gen_tag", ncent);
	jtcTH1Player* js =(jtcTH1Player*) tag0->divide(*gen0, "B");
	jtcTH1Player* bias =(jtcTH1Player*) tag->divide(*gen, "B");
	TString corr_name = "tagBias";
	js->setName(corr_name);
	auto eff_smth  = js->clone("tagBias_smth");
	eff_smth->smooth();
	//auto c =new multi_pads<base_pad>("bias_"+corr_name, "", npt, 1);
	auto c =new multi_pads<base_pad>("bias_"+corr_name, "", npt, ncent);
	c->setXrange(0,2.49);
	c->setYrange(0.5,2.5);
	c->xtitle="#Delta r";
	c->ytitle="Tagging Bias";
	c->doHIarrange = 1;
//	c->addm2TH1(js);
	c->addm2TH1(eff_smth);
	c->addm2TH1(bias);
	c->addLegend("upperright");
	c->labelHist("Smthed bias",0);
	c->labelHist("tag bias",1);
	c->addhLine(1);
	c->draw();
	c->SaveAs(fig_output+"/recoJet_"+corr_name+format);
	base->wf->cd();
	js->write();
	eff_smth->write();
	return js;
}

jtcTH1Player* bjtc_step3_analyzer::get_tracking_corr(TString sname){
	TString corr_name = sname+"_trkEff";
	jtcTH1Player rec1("correlations_djetMC/"+sname+reco_tag(1,1)+"_sig_p1_dr_*_*",npt, ncent);
	jtcTH1Player gen1("correlations_djetMC/"+sname+reco_tag(1,0)+"_sig_p1_dr_*_*",npt, ncent);
	rec1.autoLoad(base->wf);
	gen1.autoLoad(base->wf);
	auto trk1 = (jtcTH1Player*) rec1.divide(gen1, "B");
	jtcTH1Player reco("correlations_djetMC/"+sname+reco_tag(1,1)+"_sig_p0_dr_*_*",npt, ncent);
	jtcTH1Player gen ("correlations_djetMC/"+sname+reco_tag(1,0)+"_sig_p0_dr_*_*",npt, ncent);
	reco.autoLoad(base->wf);
	gen.autoLoad(base->wf);
	jtcTH1Player* js =(jtcTH1Player*) reco.divide(gen, "B");
	js->setName(corr_name);
	auto tkcorr = js->clone(sname+"_trkEff0_smth");
	tkcorr->smooth();
	trk1->smooth();
	trk1->setName(sname+"_trkEff_smth");
	auto c =new multi_pads<base_pad>("canvas_"+corr_name, "", npt, ncent);
	c->setXrange(0,2.49);
	c->setYrange(0.3,1);
	c->ytitle="Tracking Efficiency";
	c->xtitle="#Delta r";
	c->doHIarrange = 1;
	c->addm2TH1(js);
	c->addm2TH1(tkcorr);
	c->addm2TH1(trk1);
	c->addLegend("upperright");
	c->labelHist("MC based",0);
	c->labelHist("Smoothed",1);
	c->addhLine(1);
	c->draw();
	c->SaveAs(fig_output+"/tracking_"+corr_name+format);
	base->wf->cd();
	js->write();
	tkcorr->write();
	return js;
}

jtcTH1Player* bjtc_step3_analyzer::get_jff_corr(TString sname){
	TString corr_name = sname+"_JffEffect";
	jtcTH1Player reco("correlations_djetMC/"+sname+reco_tag(1,0)+"_sig_p1_dr_*_*",npt, ncent);
	jtcTH1Player gen ("correlations_djetMC/"+sname+reco_tag(0,0)+"_sig_p1_dr_*_*",npt, ncent);
	reco.autoLoad(base->wf);
	gen.autoLoad(base->wf);
	jtcTH1Player* js =(jtcTH1Player*) reco.divide(gen);
	js->setName(corr_name);
	auto c =new multi_pads<base_pad>("JFFCorr_"+corr_name, "", npt, ncent);
	c->setXrange(0,2.49);
	c->setYrange(0.5,1.5);
	c->addhLine(1);
	c->doHIarrange = 1;
	c->xtitle="#Delta r";
	c->addm2TH1(js);
	c->addhLine(1);
	c->draw();
	c->SaveAs(fig_output+"/JFF_"+corr_name+format);
	base->wf->cd();
	js->write();
	return js;
}

void bjtc_step3_analyzer::analyze(){
	//get_jff_corr("incl");
	//get_jff_corr("tagged");
	get_tracking_corr("incl");
	get_tracking_corr("tagged");
	get_tagging_biasCorr();
	//get_tracking_corr("incl");
}

