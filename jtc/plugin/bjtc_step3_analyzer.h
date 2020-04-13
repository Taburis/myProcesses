
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

		void mixing_ratio_check();
		void bias_check();
		void contamination_corr();
		void db_comparison();
		virtual void analyze();
		void debug_plot(TString savename,jtcTH1Player*j1, jtcTH1Player*j2,TString lab1="",TString lab2="", float xmin=0, float xmax = 2.49){
			auto c =new multi_pads<overlay_pad>("c_"+savename, "", base->npt, base->ncent);
			c->setXrange(xmin,xmax);
			c->xtitle="#Delta r";
			c->ytitle="#frac{dN}{d#Deltar}";
			c->doHIarrange = 1;
			c->addm2TH1(j2);
			c->addm2TH1(j1);
			c->ratio_title = "target/ref.";
			c->addLegend("upperright");
			c->labelHist(lab1, 1);
			c->labelHist(lab2, 0);
			c->setRatioYrange(0.8,1.2);
			c->draw();
			c->SaveAs(fig_output+"/"+savename+format);
		}


		TString format = ".jpg";
		int npt, ncent;
		TDirectory* _dir_; //working folder.
};

void bjtc_step3_analyzer::mixing_ratio_check(){
	auto mx_incl = new jtcTH1Player("correlations_djetMC/incl"+reco_tag(1,0)+"_mix_deta_p1_*_*",npt, ncent);
	auto mx_tag  = new jtcTH1Player("correlations_djetMC/tagged"+reco_tag(1,0)+"_mix_deta_p1_*_*",npt, ncent);
	auto mx_tagB = new jtcTH1Player("correlations_djetMC/tagTrue"+reco_tag(1,0)+"_mix_deta_p1_*_*",npt, ncent);
	auto mx_true = new jtcTH1Player("correlations_djetMC/trueB"+reco_tag(1,0)+"_mix_deta_p1_*_*",npt, ncent);
	mx_incl->autoLoad(base->wf);
	mx_tag ->autoLoad(base->wf);
	mx_tagB->autoLoad(base->wf);
	mx_true->autoLoad(base->wf);
	TString outputf = fig_output+"/mix_check";
	const int dir= system("mkdir -p "+outputf);
	debug_plot("mix_check/mix_ratio_tagged_vs_incl",mx_tag,mx_incl,"tagged.","incl.",-2.5,2.5);
	debug_plot("mix_check/mix_ratio_tagTrue_vs_incl",mx_tagB,mx_incl,"tag. & true","incl.",-2.5,2.5);
	debug_plot("mix_check/mix_ratio_true_vs_incl",mx_tagB,mx_incl,"true","incl.",-2.5, 2.5);

}

void bjtc_step3_analyzer::bias_check(){
	TString outputf = fig_output+"/bias_check";
	const int dir= system("mkdir -p "+outputf);
	auto p1_incl = new jtcTH1Player("correlations_djetMC/incl"+reco_tag(1,0)+"_sig_p0_dr_*_*",npt, ncent);
	auto p1_tag  = new jtcTH1Player("correlations_djetMC/tagged"+reco_tag(1,0)+"_sig_p0_dr_*_*",npt, ncent);
	auto p1_tagB = new jtcTH1Player("correlations_djetMC/tagTrue"+reco_tag(1,0)+"_sig_p0_dr_*_*",npt, ncent);
	auto p1_true = new jtcTH1Player("correlations_djetMC/trueB"+reco_tag(1,0)+"_sig_p0_dr_*_*",npt, ncent);
	auto p1_neg = new jtcTH1Player("correlations_djetMC/negTag"+reco_tag(1,0)+"_sig_p0_dr_*_*",npt, ncent);
	p1_incl->autoLoad(base->wf);
	p1_tag ->autoLoad(base->wf);
	p1_tagB->autoLoad(base->wf);
	p1_true->autoLoad(base->wf);
	p1_neg->autoLoad(base->wf);
	debug_plot("bias_check/p1ratio_tagTrue_vs_tag",p1_tagB,p1_true,"tag&true.","true",0,2.49);
	debug_plot("bias_check/p1ratio_neg_vs_incl",p1_neg,p1_incl,"neg.","incl",0,2.49);
}

void bjtc_step3_analyzer::db_comparison(){
	jtcTH1Player tagd_dmc("correlations_djetMC/tagTrue"+reco_tag(1,0)+"_sig_p0_dr_*_*",npt, ncent);
	jtcTH1Player tagd_bmc("correlations_bjetMC/tagTrue"+reco_tag(1,0)+"_sig_p0_dr_*_*",npt, ncent);
	tagd_dmc.autoLoad(base->wf);
	tagd_bmc.autoLoad(base->wf);
	jtcTH1Player true_dmc("correlations_djetMC/trueB"+reco_tag(1,0)+"_sig_p0_dr_*_*",npt, ncent);
	jtcTH1Player true_bmc("correlations_bjetMC/trueB"+reco_tag(1,0)+"_sig_p0_dr_*_*",npt, ncent);
	true_dmc.autoLoad(base->wf);
	true_bmc.autoLoad(base->wf);

	auto c =new multi_pads<overlay_pad>("db_tag_true", "", npt, ncent);
	c->setXrange(0,.9);
	//c->setYrange(0.5,2.5);
	c->xtitle="#Delta r";
	c->doHIarrange = 1;
	//	c->addm2TH1(js);
	c->addm2TH1(&tagd_bmc);
	c->addm2TH1(&tagd_dmc);
	c->addLegend("upperright");
	c->labelHist("tag. & b: bmc",0);
	c->labelHist("tag. & b: dmc",1);
	c->addhLine(1);
	c->draw();
	c->SaveAs(fig_output+"/debug_tagTrue"+format);
	c =new multi_pads<overlay_pad>("db_trueB", "", npt, ncent);
	c->setXrange(0,2.49);
	//c->setYrange(0.5,2.5);
	c->xtitle="#Delta r";
	c->doHIarrange = 1;
	//	c->addm2TH1(js);
	c->addm2TH1(&tagd_bmc);
	c->addm2TH1(&tagd_dmc);
	c->addLegend("upperright");
	c->labelHist("tag. & b: bmc",0);
	c->labelHist("tag. & b: dmc",1);
	c->addhLine(1);
	c->draw();
	c->SaveAs(fig_output+"/debug_trueB"+format);

}

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
	js->setDirectory(_dir_);
	eff_smth->setDirectory(_dir_);
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
//based on the mixing check, we shouldn't use the p0 signal to produce track efficiency
	auto trk1 = (jtcTH1Player*) rec1.divide(gen1, "B");
	auto trkcorr =(jtcTH1Player*)trk1->clone(sname+"_trkEff_p1_smth");
	trkcorr->smooth();
//	jtcTH1Player reco("correlations_djetMC/"+sname+reco_tag(1,1)+"_sig_p0_dr_*_*",npt, ncent);
//	jtcTH1Player gen ("correlations_djetMC/"+sname+reco_tag(1,0)+"_sig_p0_dr_*_*",npt, ncent);
//	reco.autoLoad(base->wf);
//	gen.autoLoad(base->wf);
//	jtcTH1Player* js =(jtcTH1Player*) reco.divide(gen, "B");
//	js->setName(corr_name);
//	auto tkcorr = js->clone(sname+"_trkEff_p0_smth");
//	tkcorr->smooth();
	auto c =new multi_pads<base_pad>("canvas_"+corr_name, "", npt, ncent);
	c->setXrange(0,2.49);
	c->setYrange(0.3,1);
	c->ytitle="Tracking Efficiency";
	c->xtitle="#Delta r";
	c->doHIarrange = 1;
//	c->addm2TH1(js);
	c->addm2TH1(trk1);
	c->addm2TH1(trkcorr);
	c->addLegend("upperright");
//	c->labelHist("MC based",0);
	c->labelHist("trk Eff.",0);
	c->labelHist("Smoothed Eff.",1);
	c->addhLine(1);
	c->draw();
	c->SaveAs(fig_output+"/tracking_"+corr_name+format);
	trk1->setDirectory(_dir_);
	trkcorr->setDirectory(_dir_);
	trkcorr->write();
	trk1->write();
	return trkcorr;
}

jtcTH1Player* bjtc_step3_analyzer::get_jff_corr(TString sname){
	TString corr_name = sname+"_JffEffect";
	jtcTH1Player reco("correlations_djetMC/"+sname+reco_tag(1,0)+"_sig_p2_dr_*_*",npt, ncent);
	jtcTH1Player gen ("correlations_djetMC/"+sname+reco_tag(0,0)+"_sig_p2_dr_*_*",npt, ncent);
	reco.autoLoad(base->wf);
	gen.autoLoad(base->wf);
	jtcTH1Player* js =(jtcTH1Player*) reco.divide(gen,"B");
	js->setName(corr_name);
	auto c =new multi_pads<base_pad>("JFFCorr_"+corr_name, "", npt, ncent);
	c->setXrange(0,2.49);
	c->setYrange(0.5,1.5);
	c->doHIarrange = 1;
	c->xtitle="#Delta r";
	c->addm2TH1(js);
	c->addhLine(1);
	c->draw();
	c->SaveAs(fig_output+"/JFF_"+corr_name+format);
	js->setDirectory(_dir_);
	js->write();
	return js;
}

//void bjtc_step3_analyzer::analyze(){
//	TString corr_name = sname+"_JffEffect";
//	jtcTH1Player reco("correlations_djetMC/"+sname+reco_tag(1,0)+"_sig_p2_dr_*_*",npt, ncent);
//	jtcTH1Player gen ("correlations_bjetMC/"+sname+reco_tag(0,0)+"_sig_p2_dr_*_*",npt, ncent);
//	reco.autoLoad(base->wf);
//	gen.autoLoad(base->wf);
//}

void bjtc_step3_analyzer::analyze(){
	_dir_ = base->wf->mkdir(_name_);
	if(_dir_==0) _dir_=(TDirectory*) base->wf->Get(_name_);
	base->wf->cd(_name_);
	bias_check();
	//	_dir_->cd();
	//get_jff_corr("incl");
	//get_jff_corr("tagged");
	//mixing_ratio_check();
	//db_comparison();
	//get_tracking_corr("incl");
	//get_tracking_corr("tagged");
	//get_tagging_biasCorr();
	//get_tracking_corr("incl");
}

