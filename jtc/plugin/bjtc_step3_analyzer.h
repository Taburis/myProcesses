
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
		jtcTH1Player* get_tracking_corr(TString sname, TString folder);
		jtcTH1Player* get_tracking_corr2(TString sname, TString folder);
		jtcTH1Player* get_tracking_corr_finebin(TString sname, TString folder);
		jtcTH1Player* get_jff_corr(TString sname, TString corr_name);
		jtcTH1Player* get_spillOver_corr(TString sname, TString corr_name);
		jtcTH1Player* get_tagging_biasCorr();
		void get_tagging_biasCorr_uncert();
		void taggingBias_uncert();
		jtcTH1Player* biasCorr_wf001(TString name, jtcTH1Player *j1, jtcTH1Player *j2);
		jtcTH1Player* get_cont_biasCorr();
		jtcTH1Player* signalFitting(TString name, jtcTH1Player *dr);
		void fitting_tracking(TString sname, TString folder);

		void jff_check();

		void mixing_ratio_check();
		void bias_check();
		void sube_check();
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
			c->setRatioYrange(0.,2.);
			c->draw();
			c->SaveAs(fig_output+"/"+savename+format);
		}


		TString format = ".jpg", step2fname, step2uncert, systematic;
		int npt, ncent;
		TDirectory* _dir_; //working folder.
		TFile *fstep2, *funcert, *fsystem;
};

float dr_fine[] = {0.,0.02, 0.04,0.06, 0.08,0.1,0.125,0.15,0.2,0.25,0.3,0.35,0.4,0.5,0.6,0.7,0.8,0.9,1.,1.2, 1.5, 2, 2.5};
float dr_trk[] = {0.,0.02, 0.04,0.06, 0.08,0.1,0.125,0.15,0.2,0.25,0.3,0.35,0.4,0.5,0.6,0.8,1., 2.5};
int ndr_trk = 17;
int ndr_fine = 22;

void bjtc_step3_analyzer::mixing_ratio_check(){
	auto mx_incl = new jtcTH1Player("correlations_djetMC/incl"+reco_tag(1,0)+"_mix_deta_p1_*_*",npt, ncent);
	auto mx_tag  = new jtcTH1Player("correlations_djetMC/tagged"+reco_tag(1,0)+"_mix_deta_p1_*_*",npt, ncent);
	auto mx_tagB = new jtcTH1Player("correlations_djetMC/tagTrue"+reco_tag(1,0)+"_mix_deta_p1_*_*",npt, ncent);
	auto mx_true = new jtcTH1Player("correlations_djetMC/trueB"+reco_tag(1,0)+"_mix_deta_p1_*_*",npt, ncent);
	mx_incl->autoLoad(fstep2);
	mx_tag ->autoLoad(fstep2);
	mx_tagB->autoLoad(fstep2);
	mx_true->autoLoad(fstep2);
	TString outputf = fig_output+"/mix_check";
	const int dir= system("mkdir -p "+outputf);
	debug_plot("mix_check/mix_ratio_tagged_vs_incl",mx_tag,mx_incl,"tagged.","incl.",-2.5,2.5);
	debug_plot("mix_check/mix_ratio_tagTrue_vs_incl",mx_tagB,mx_incl,"tag. & true","incl.",-2.5,2.5);
	debug_plot("mix_check/mix_ratio_true_vs_incl",mx_tagB,mx_incl,"true","incl.",-2.5, 2.5);

}

void bjtc_step3_analyzer::sube_check(){
	TString outputf = fig_output+"/sube_check";
	const int dir= system("mkdir -p "+outputf);
	auto incl = new jtcTH1Player("correlations_djetMC/incl"+reco_tag(0,0)+"_sig_p2_*_*",npt, ncent);
	auto inclsube0 = new jtcTH1Player("correlations_djetMC/incl_sube0"+reco_tag(0,0)+"_sig_p2_*_*",npt, ncent);
	auto inclrec = new jtcTH1Player("correlations_djetMC/incl"+reco_tag(1,0)+"_sig_p2_*_*",npt, ncent);
	auto inclrecsube0 = new jtcTH1Player("correlations_djetMC/incl_sube0"+reco_tag(1,0)+"_sig_p2_*_*",npt, ncent);
	incl->autoLoad(fstep2);
	inclsube0->autoLoad(fstep2);
	inclrec->autoLoad(fstep2);
	inclrecsube0->autoLoad(fstep2);
	auto deta_incl = incl->projX("incl_deta_sig_p2_*_*", -1, 1, "e");
	auto deta_inclsube0 = inclsube0->projX("incl_sube0_deta_sig_p2_*_*", -1, 1, "e");
	auto deta_inclrec = inclrec->projX("incl_rec_deta_sig_p2_*_*", -1, 1, "e");
	auto deta_inclrecsube0 = inclrecsube0->projX("incl_rec_sube0_deta_sig_p2_*_*", -1, 1, "e");
	debug_plot("sube_check/GG_deta_allEvt_vs_Sube0",deta_inclsube0,deta_incl,"Sube0","allEvt",-2,1.99);
	debug_plot("sube_check/RG_deta_allEvt_vs_Sube0",deta_inclrecsube0,deta_inclrec,"Sube0","allEvt",-2,1.99);
}

void bjtc_step3_analyzer::bias_check(){
	TString outputf = fig_output+"/bias_check";
	const int dir= system("mkdir -p "+outputf);
	auto p1_incl = new jtcTH1Player("correlations_djetMC/incl"+reco_tag(1,0)+"_sig_p0_dr_*_*",npt, ncent);
	auto p1_tag  = new jtcTH1Player("correlations_djetMC/tagged"+reco_tag(1,0)+"_sig_p0_dr_*_*",npt, ncent);
	auto p1_tagB = new jtcTH1Player("correlations_djetMC/tagTrue"+reco_tag(1,0)+"_sig_p0_dr_*_*",npt, ncent);
	auto p1_true = new jtcTH1Player("correlations_djetMC/trueB"+reco_tag(1,0)+"_sig_p0_dr_*_*",npt, ncent);
	auto p1_neg = new jtcTH1Player("correlations_djetMC/negTag"+reco_tag(1,0)+"_sig_p0_dr_*_*",npt, ncent);
	p1_incl->autoLoad(fstep2);
	p1_tag ->autoLoad(fstep2);
	p1_tagB->autoLoad(fstep2);
	p1_true->autoLoad(fstep2);
	p1_neg ->autoLoad(fstep2);
	debug_plot("bias_check/p1ratio_tagTrue_vs_tag",p1_tagB,p1_true,"tag&true.","true",0,2.49);
	debug_plot("bias_check/p1ratio_neg_vs_incl",p1_neg,p1_incl,"neg.","incl",0,2.49);

	auto biasl = (jtcTH1Player*) p1_neg->divide(*p1_incl);
	auto biasb = (jtcTH1Player*)p1_tagB->divide(*p1_tag);
	debug_plot("bias_check/p1ratio_biasLight_vs_biasB",biasl,biasb,"bias-light.","bias-b",0,2.49);
}

void bjtc_step3_analyzer::db_comparison(){
	jtcTH1Player tagd_dmc("correlations_djetMC/tagTrue"+reco_tag(1,0)+"_sig_p0_dr_*_*",npt, ncent);
	jtcTH1Player tagd_bmc("correlations_bjetMC/tagTrue"+reco_tag(1,0)+"_sig_p0_dr_*_*",npt, ncent);
	tagd_dmc.autoLoad(fstep2);
	tagd_bmc.autoLoad(fstep2);
	jtcTH1Player true_dmc("correlations_djetMC/trueB"+reco_tag(1,0)+"_sig_p0_dr_*_*",npt, ncent);
	jtcTH1Player true_bmc("correlations_bjetMC/trueB"+reco_tag(1,0)+"_sig_p0_dr_*_*",npt, ncent);
	true_dmc.autoLoad(fstep2);
	true_bmc.autoLoad(fstep2);

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

void bjtc_step3_analyzer::taggingBias_uncert(){
	auto lob = new jtcTH1Player("correlations_bjetMC_sube_gsp/loB_sube0"+reco_tag(0,0)+"_sig_p0_*_*",npt, ncent);
	auto lobTg = new jtcTH1Player("correlations_bjetMC_sube_gsp/loBTagged_sube0"+reco_tag(0,0)+"_sig_p0_*_*",npt, ncent);
	auto gsp = new jtcTH1Player("correlations_bjetMC_sube_gsp/gsp_sube0"+reco_tag(0,0)+"_sig_p0_*_*",npt, ncent);
	auto gspTg = new jtcTH1Player("correlations_bjetMC_sube_gsp/gspTagged_sube0"+reco_tag(0,0)+"_sig_p0_*_*",npt, ncent);
	lob->autoLoad(funcert);
	lobTg->autoLoad(funcert);
	gsp->autoLoad(funcert);
	gspTg->autoLoad(funcert);

	auto lobdr = lob->drIntegral("dr_lob_*_*");
	auto lobTgdr = lobTg->drIntegral("dr_lobTg_*_*");
	auto gspdr = gsp->drIntegral("dr_gsp_*_*");
	auto gspTgdr = gspTg->drIntegral("dr_gspTg_*_*");

	auto lobsum = lobdr->contractX("drSum_lob_*_*");
	auto gspsum = gspdr->contractX("drSum_gsp_*_*");
	auto lobTgsum = lobTgdr->contractX("drSum_lobTg_*_*");
	auto gspTgsum = gspTgdr->contractX("drSum_gspTg_*_*");

	auto bias1 = lobTgsum->divide(*lobsum, "B");
	auto bias2 = gspTgsum->divide(*gspsum, "B");
	auto c =new multi_pads<overlay_pad>("bias_uncert", "", 1, ncent);
	c->setXrange(0.,0.99);
	c->setRatioYrange(0.8,1.2);
	c->doHIarrange = 1;
	c->xtitle="#Delta r";
	c->addm2TH1(bias1);
	c->addm2TH1(bias2);
	c->addLegend("upperright");
	c->labelHist("LO b jet",0);
	c->labelHist("GSP b jet",1);
	c->draw();
	c->SaveAs(fig_output+"/systUncert_GSP_vs_EXO"+format);
}

void bjtc_step3_analyzer::get_tagging_biasCorr_uncert(){
	auto tagb1 = new jtcTH1Player("correlations_bjetMC_sube/tagTrue_sube0"+reco_tag(1,0)+"_sig_p0_*_*",npt, ncent);
	auto genb1 = new jtcTH1Player("correlations_bjetMC_sube/trueB_sube0"+reco_tag(1,0)+"_sig_p0_*_*",npt, ncent);
	tagb1->autoLoad(fstep2);
	genb1->autoLoad(fstep2);
	auto tagb2 = new jtcTH1Player("correlations_bjetMC_sube_gsp/tagTrue_sube0"+reco_tag(1,0)+"_sig_p0_*_*",npt, ncent);
	auto genb2 = new jtcTH1Player("correlations_bjetMC_sube_gsp/trueB_sube0"+reco_tag(1,0)+"_sig_p0_*_*",npt, ncent);
	tagb2->autoLoad(funcert);
	genb2->autoLoad(funcert);
	auto genb1sum = genb1->drIntegral("dr_sig_gen1_*_*");
	auto tagb1sum = tagb1->drIntegral("dr_sig_tag1_*_*");
	auto genb2sum = genb2->drIntegral("dr_sig_gen2_*_*");
	auto tagb2sum = tagb2->drIntegral("dr_sig_tag2_*_*");

	auto genb1com = genb1sum->contractX("combined_gen1");
	auto tagb1com = tagb1sum->contractX("combined_tag1");
	auto genb2com = genb2sum->contractX("combined_gen2");
	auto tagb2com = tagb2sum->contractX("combined_tag2");

	auto bias1 = tagb1com->divide(*genb1com, "B");
	auto bias2 = tagb2com->divide(*genb2com, "B");
	auto error = bias1->divide(*bias2);
	error->setName("js_bjet_taggingBias_systUncert_*_*");
	fsystem = TFile::Open(output+"/"+systematic+".root", "update");
	error->write();
	fsystem->Close();
	auto c =new multi_pads<overlay_pad>("bias_uncert", "", 1, ncent);
	c->setXrange(0.,0.99);
	c->setRatioYrange(0.9,1.1);
	c->doHIarrange = 1;
	c->xtitle="#Delta r";
	c->addm2TH1(bias1);
	c->addm2TH1(bias2);
	c->addLegend("upperright");
	c->labelHist("nominal",0);
	c->labelHist("GSP reweighted",1);
	c->draw();
	c->SaveAs(fig_output+"/systUncert_GSPreweighted"+format);
	
}

jtcTH1Player* bjtc_step3_analyzer::get_tagging_biasCorr(){
	//jtcTH1Player tagb("correlations_bjetMC/tagged"+reco_tag(0,0)+"_sig_p1_dr_*_*",npt, ncent);
	//jtcTH1Player tag("tagTrue"+reco_tag(1,0)+"_sig_p0_dr_*_*",npt, ncent);
	//jtcTH1Player tag("tagTrue"+reco_tag(1,0)+"_sig_p1_dr_*_*",npt, ncent);
	//jtcTH1Player gen("trueB"+reco_tag(0,0)+"_sig_p1_dr_*_*",npt, ncent);
	auto tagb = new jtcTH1Player("correlations_bjetMC_sube/tagTrue_sube0"+reco_tag(1,0)+"_sig_p0_*_*",npt, ncent);
	auto genb = new jtcTH1Player("correlations_bjetMC_sube/trueB_sube0"+reco_tag(1,0)+"_sig_p0_*_*",npt, ncent);
	tagb->autoLoad(fstep2);
	genb->autoLoad(fstep2);
	return biasCorr_wf001("tagBias", tagb, genb);
}

jtcTH1Player* bjtc_step3_analyzer::biasCorr_wf001(TString corr_name, jtcTH1Player *tagb, jtcTH1Player *genb){

	jtcTH1Player* tag = tagb->drIntegral("dr_sig_tag_*_*");
	jtcTH1Player* gen = genb->drIntegral("dr_sig_gen_*_*");
	//jtcTH1Player* tag = tagb->drIntegral("dr_sig_tag_*_*",ndr_fine, dr_fine);
	//jtcTH1Player* gen = genb->drIntegral("dr_sig_gen_*_*",ndr_fine, dr_fine);
	auto gen0 = gen->contractY("contractY_gen_true");
	auto tag0 = tag->contractY("contractY_gen_tag");
	gen0->duplicateY("summed_gen_true", ncent);
	tag0->duplicateY("summed_gen_tag", ncent);
	jtcTH1Player* js =(jtcTH1Player*) tag0->divide(*gen0, "B");
	jtcTH1Player* bias =(jtcTH1Player*) tag->divide(*gen, "B");
	js->setName(corr_name);
	auto eff_smth  = bias->clone("tagBias_smth");
	//auto eff_smth  = js->clone("tagBias_smth");
	eff_smth->setAxisRange(0.1, 2., "x");
	eff_smth->smooth(1, "R");
	eff_smth->setAxisRange(0., 2., "x");
	//eff_smth->setAxisRange(0., 1., "x");
	//auto c =new multi_pads<base_pad>("bias_"+corr_name, "", npt, 1);
	auto c =new multi_pads<base_pad>("bias_"+corr_name, "", npt, ncent);
	c->setXrange(0,2.49);
	c->setYrange(0.5,2.5);
	c->xtitle="#Delta r";
	c->ytitle="Tagging Bias";
	c->doHIarrange = 1;
	//	c->addm2TH1(js);
	c->addm2TH1(bias);
	c->addm2TH1(eff_smth);
	c->addLegend("upperright");
	c->labelHist("Smthed bias",0);
	c->labelHist("tag. bias",1);
	c->addhLine(1);
	c->draw();
	c->SaveAs(fig_output+"/recoJet_"+corr_name+format);
	js->setDirectory(_dir_);
	eff_smth->setDirectory(_dir_);
	js->write();
	eff_smth->write();
	return js;
}


jtcTH1Player* bjtc_step3_analyzer::get_tracking_corr2(TString sname, TString folder){
	TString corr_name = sname+"_trkEff";
	jtcTH1Player rec2D(folder+"/"+sname+reco_tag(1,1)+"_sig_p1_*_*",npt, ncent);
	jtcTH1Player gen2D(folder+"/"+sname+reco_tag(1,0)+"_sig_p1_*_*",npt, ncent);
	rec2D.autoLoad(fstep2);
	gen2D.autoLoad(fstep2);
	auto gen_dr = gen2D.drIntegral(sname+"_sig_dr_gen_*_*", ndr_fine, dr_fine, "area");
	auto rec_dr = rec2D.drIntegral(sname+"_sig_dr_rec_*_*", ndr_fine, dr_fine, "area");
	auto gen_drsm = gen2D.drIntegral(sname+"_smooth_sig_dr_gen_*_*", ndr_fine, dr_fine, "area");
	auto rec_drsm = rec2D.drIntegral(sname+"_smooth_sig_dr_rec_*_*", ndr_fine, dr_fine, "area");
	gen_drsm->smooth(1,"R");	
	rec_drsm->smooth(1,"R");


	auto cgen =new multi_pads<base_pad>("canvas_gen_"+corr_name, "", npt, ncent);
	cgen->setXrange(0,.99);
	cgen->xtitle="#Delta r";
	cgen->doAutoYrange = 1;
	cgen->doHIarrange = 1;
	cgen->addLegend("upperright");
	cgen->addm2TH1(gen_dr);
	cgen->addm2TH1(gen_drsm);
	cgen->labelHist("gen",0);
	cgen->labelHist("Smoothed gen",1);
	cgen->draw();
	cgen->SaveAs(fig_output+"/dev_tracking_gen_smth_"+corr_name+format);

	auto crec =new multi_pads<base_pad>("canvas_rec_"+corr_name, "", npt, ncent);
	crec->setXrange(0,.99);
	crec->xtitle="#Delta r";
	crec->doAutoYrange = 1;
	crec->doHIarrange = 1;
	crec->addLegend("upperright");
	crec->addm2TH1(rec_dr);
	crec->addm2TH1(rec_drsm);
	crec->labelHist("rec",0);
	crec->labelHist("Smoothed rec",1);
	crec->draw();
	crec->SaveAs(fig_output+"/dev_tracking_rec_smth_"+corr_name+format);

	
	auto trk1 = (jtcTH1Player*) rec_dr->divide(*gen_dr, "B");
	auto trkcorr = (jtcTH1Player*) rec_drsm->divide(*gen_drsm, "B");

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
	c->SaveAs(fig_output+"/dev_tracking_"+corr_name+format);
	return trkcorr;
}

jtcTH1Player* bjtc_step3_analyzer::get_tracking_corr_finebin(TString sname, TString folder){
	TString corr_name = sname+"_trkEff_finebin";
	jtcTH1Player rec2D(folder+"/"+sname+reco_tag(1,1)+"_sig_p0_*_*",npt, ncent);
	jtcTH1Player gen2D(folder+"/"+sname+reco_tag(1,0)+"_sig_p0_*_*",npt, ncent);
	rec2D.autoLoad(fstep2);
	gen2D.autoLoad(fstep2);
	auto gen_dr = gen2D.drIntegral(sname+"_sig_dr_gen_*_*", ndr_trk, dr_trk);
	auto rec_dr = rec2D.drIntegral(sname+"_sig_dr_rec_*_*", ndr_trk, dr_trk);
	auto trk0 = (jtcTH1Player*) rec_dr->divide(*gen_dr, "B");
	auto trksm =(jtcTH1Player*)trk0->clone(sname+"_trkEff_p1_smth_fineBin");
	for(int i=0;i<trksm->Nrow(); i++){
		for(int j=0;j<trksm->Ncol(); j++){
			trksm->at(i,j)->SetAxisRange(0.04,2.5,"X");
		}}
	trksm->smooth(1, "R");
	for(int i=0;i<trksm->Nrow(); i++){
		for(int j=0;j<trksm->Ncol(); j++){
			trksm->at(i,j)->SetAxisRange(0.0,2.5,"X");
		}}

	auto c =new multi_pads<base_pad>("canvas_overlay_"+corr_name, "", npt, ncent, 450, 200);
	c->setXrange(0,2.49);
	c->setYrange(0.5,1);
	c->ytitle="Tracking Efficiency";
	c->xtitle="#Delta r";
	c->doHIarrange = 1;
	c->addm2TH1(trk0);
	c->addm2TH1(trksm);
	c->addLegend(0.55,0.55, 0.95,0.9);
	c->labelHist("Rec./Gen.",0);
	c->labelHist("Tracking Efficiency",1);
	c->addhLine(1);
	c->draw();
	c->SaveAs(fig_output+"/tracking_finebin_"+corr_name+format);
	//trk0->setDirectory(_dir_);
	//trksm->setDirectory(_dir_);
	//trk0->write();
	//trksm->write();
	return trksm;
}

jtcTH1Player* bjtc_step3_analyzer::get_tracking_corr(TString sname, TString folder){
	TString corr_name = sname+"_trkEff";
	jtcTH1Player rec2D(folder+"/"+sname+reco_tag(1,1)+"_sig_p1_*_*",npt, ncent);
	jtcTH1Player gen2D(folder+"/"+sname+reco_tag(1,0)+"_sig_p1_*_*",npt, ncent);
	rec2D.autoLoad(fstep2);
	gen2D.autoLoad(fstep2);
	auto gen_dr = gen2D.drIntegral(sname+"_sig_dr_gen_*_*");
	auto rec_dr = rec2D.drIntegral(sname+"_sig_dr_rec_*_*");
	//auto gen_dr = gen2D.drIntegral(sname+"_sig_dr_gen_*_*", ndr_fine, dr_fine);
	//auto rec_dr = rec2D.drIntegral(sname+"_sig_dr_rec_*_*", ndr_fine, dr_fine);
	auto c1 =new multi_pads<overlay_pad>("canvas_overlay_"+corr_name, "", npt, ncent);
	c1->setXrange(0,2.49);
	c1->setRatioYrange(0.5,1.);
	c1->doHIarrange = 1;
	c1->xtitle="#Delta r";
	c1->addm2TH1(gen_dr);
	c1->addm2TH1(rec_dr);
	c1->draw();
	c1->SaveAs(fig_output+"/overlay_trk_"+corr_name+format);
	//based on the mixing check, we shouldn't use the p0 signal to produce track efficiency
	auto trk1 = (jtcTH1Player*) rec_dr->divide(*gen_dr, "B");
	auto trkcorr =(jtcTH1Player*)trk1->clone(sname+"_trkEff_p1_smth");
//	for(int i=0;i<trkcorr->Nrow(); i++){
//		for(int j=0;j<trkcorr->Ncol(); j++){
//			trkcorr->at(i,j)->SetAxisRange(0.07,2.5,"X");
//		}}
//	trkcorr->smooth(1, "R");
//	for(int i=0;i<trkcorr->Nrow(); i++){
//		for(int j=0;j<trkcorr->Ncol(); j++){
//			trkcorr->at(i,j)->SetAxisRange(0.,2.5,"X");
//		}}
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
	c->labelHist("trk Eff.",0);
	c->labelHist("Smoothed Eff.",1);
	c->addhLine(1);
	c->draw();
	c->SaveAs(fig_output+"/tracking_"+corr_name+format);
	trk1->setDirectory(_dir_);
	trkcorr->setDirectory(_dir_);
	trkcorr->write();
	trk1->write();
/*
*/
	return trkcorr;
}

jtcTH1Player* bjtc_step3_analyzer::get_spillOver_corr(TString sname, TString corr_name){
	jtcTH1Player reco(sname+reco_tag(1,0)+"_sig_p2_*_*",npt, ncent);
	reco.autoLoad(fstep2);
	auto reco_dr = reco.drIntegral(corr_name+"_dr0");
	int nbin = reco_dr->at(0,0)->GetNbinsX();
	for(int i=0; i<npt; i++){
		for(int j=0; j<ncent; j++){
			for(int k=1; k<nbin+1;k++){
				double cont = reco_dr->at(i,j)->GetBinContent(k);
				double error = reco_dr->at(i,j)->GetBinError(k);
				if(cont > error*1)continue;
				reco_dr->at(i,j)->SetBinContent(k,0);
				reco_dr->at(i,j)->SetBinError(k,0);
			}
		}
	}
	reco_dr->setName(corr_name+"_*_*");
	auto c =new multi_pads<base_pad>("spillOverCorr_"+corr_name, "", npt, ncent);
	c->setXrange(0,2.49);
	//	c->setYrange(0.5,1.5);
	c->doHIarrange = 1;
	c->xtitle="#Delta r";
	c->addm2TH1(reco_dr);
	c->addhLine(0);
	c->draw();
	c->SaveAs(fig_output+"/SpillOver_"+corr_name+format);
	reco_dr->setDirectory(_dir_);
	reco_dr->write();
	return reco_dr;
}

jtcTH1Player* bjtc_step3_analyzer::get_jff_corr(TString sname, TString corr_name){
	jtcTH1Player reco(sname+reco_tag(1,0)+"_sig_p0_*_*",npt, ncent);
	jtcTH1Player gen (sname+reco_tag(0,0)+"_sig_p0_*_*",npt, ncent);
	reco.autoLoad(fstep2);
	gen .autoLoad(fstep2);
	auto reco_dr = reco.drIntegral(corr_name+"_reco_*_*");
	auto gen_dr = gen.drIntegral(corr_name+"_gen_*_*");
	//auto reco_dr = reco.drIntegral("sig_dr_jff_reco_*_*",ndr_fine, dr_fine);
	//auto gen_dr = gen.drIntegral("sig_dr_jff_gen_*_*",ndr_fine, dr_fine);
	jtcTH1Player* js =(jtcTH1Player*) reco_dr->add2("jffCorr",*gen_dr,1, -1);
	//jtcTH1Player* js =(jtcTH1Player*) reco_dr->divide(*gen_dr,"B");
	js->setName(corr_name+"_*_*");

	float drmax = 0.8;	
	for(int i=0; i<npt; i++){
		for(int j=0; j<ncent; j++){
			int nmax = js->at(i,j)->FindBin(drmax);
			for(int k=1; k<js->at(i,j)->GetNbinsX()+1;k++){
				if(k<nmax) continue;
				js->at(i,j)->SetBinContent(k,0);
				js->at(i,j)->SetBinError(k,0);
			}
		}
	}
	auto c =new multi_pads<base_pad>("JFFCorr_"+corr_name, "", npt, ncent);
	c->setXrange(0,2.49);
	//	c->setYrange(0.5,1.5);
	c->doHIarrange = 1;
	c->xtitle="#Delta r";
	c->addm2TH1(js);
	c->addhLine(0);
	c->draw();
	c->SaveAs(fig_output+"/JFF_"+corr_name+format);
	js->setDirectory(_dir_);
	js->write();
	return js;

}

//Double_t trkf(Double_t *x, Double_t *p){
//	if(x[0]>0.7) return p[0];
//	else return p[0]+p[1]*x[0]+p[2]*pow(x[0],2)+p[3]*pow(x[0],3);
//	//else return p[0]+p[1]*x[0]+p[2]*pow(x[0],2)+p[3]*pow(x[0],3)+p[4]*pow(x[0],4);
//}

Double_t trkf(Double_t *x, Double_t *p){
	if(x[0]>1) return p[0];
	else return p[0]+p[2]*TMath::Exp(-p[1]*x[0])-p[3]*TMath::Exp(-p[4]*pow(x[0]-p[5],2));
}

jtcTH1Player* bjtc_step3_analyzer::signalFitting(TString name, jtcTH1Player* dr){
	auto fg2 = new TF1("fg2", "[0]+[1]*exp(-x*[2]-pow(x,2)*[3]+[4]*x**3+[5]*x**4)", 0, 2.);
	jtcTH1Player* aux = (jtcTH1Player*) dr->clone("fit_"+name);
	jtcTH1Player* aux2 = (jtcTH1Player*) dr->clone("fit2_"+name);
	for(int i=0;i<npt; ++i){
		for(int j=0;j<ncent; ++j){
			TH1* h = aux2->at(i,j);
			for(int k=1; k<h->GetNbinsX()+1; ++k){
				double cont = h->GetBinContent(k);
				double err = h->GetBinError(k);
				double frac = err/cont;
				h->SetBinContent(k,TMath::Log(cont));
				h->SetBinError(k,TMath::Log(cont)*frac);
			}
		}
	}
	auto c1 =new multi_pads<overlay_pad>("canvas_fit_"+name, "", npt, ncent);
	auto c2 =new multi_pads<overlay_pad>("canvas_fit2_"+name, "", npt, ncent);
	for(int i=0; i<npt; ++i){
		for(int j=0; j<ncent; ++j){
			c1->CD(i,ncent-1-j);
			//TH1* h = gen_dr->at(i,j);
			TH1* h = aux2->at(i,j);
			double range = 2.;
			h->GetXaxis()->SetRangeUser(0,range);
			double drInfty = h->GetBinContent(h->FindBin(1.4));	
			double dr0 = h->GetBinContent(h->FindBin(0.01));	
			fg2->SetParameter(0, drInfty);
			fg2->SetParameter(1, dr0- drInfty);
			h->SetMarkerSize(0.5);
			h->SetMarkerStyle(20);
			auto ptr = h->Fit(fg2, "S","", 0., range);
			//gPad->SetLogy();
			TH1* h0= dr->at(i,j);
			TH1* h2= aux->at(i,j);
			for(int k=1; k<h->GetNbinsX()+1; ++k){	
				double bin = h2->GetBinCenter(k);
				double cont = 1;
				double err = h0->GetBinError(k);
				double cut = 0.1;
				if(bin >cut) {
					cont = TMath::Exp(fg2->Eval(bin));
				}else if(bin <= cut) cont = h0->GetBinContent(k);
				if(bin>1.){
					cont = h2->GetBinContent(h->FindBin(.99));
				}
				h2->SetBinContent(k,cont);
				h2->SetBinError(k,err);
			}
			c2->CD(i,ncent-1-j);
			h2->SetMarkerColor(kBlue);
			h2->SetMarkerSize(0.7);
			h2->SetMarkerStyle(20);
			h2->SetLineColor(kBlue);
			h2->Draw();
			dr->at(i,j)->SetMarkerColor(kRed);
			dr->at(i,j)->SetMarkerStyle(20);
			dr->at(i,j)->SetMarkerSize(0.55);
			dr->at(i,j)->Draw("same");
			gPad->SetLogy();
		}
	}
	c1->SaveAs(fig_output+"/signalFit1_"+name+format);
	c2->SaveAs(fig_output+"/signalFit2_"+name+format);
	return aux;
}

void bjtc_step3_analyzer::fitting_tracking(TString sname, TString folder){
	jtcTH1Player rec2D(folder+"/"+sname+reco_tag(1,1)+"_sig_p1_*_*",npt, ncent);
	jtcTH1Player gen2D(folder+"/"+sname+reco_tag(1,0)+"_sig_p1_*_*",npt, ncent);
	rec2D.autoLoad(fstep2);
	gen2D.autoLoad(fstep2);
	auto gen_dr = gen2D.drIntegral(sname+"_sig_dr_gen_*_*", ndr_fine, dr_fine, "area");
	auto rec_dr = rec2D.drIntegral(sname+"_sig_dr_rec_*_*", ndr_fine, dr_fine, "area");
	auto trk1 = (jtcTH1Player*) rec_dr->divide(*gen_dr, "B");

	auto gen_fit = signalFitting(sname+"_sig_dr_gen",  gen_dr);
	auto rec_fit = signalFitting(sname+"_sig_dr_rec",  rec_dr);
	auto trk2 = (jtcTH1Player*) rec_fit->divide(*gen_fit, "B");
	auto c1 =new multi_pads<overlay_pad>("canvas_overlay_corr_"+sname, "", npt, ncent);
	c1->setXrange(0,2.49);
	c1->doHIarrange = 1;
	c1->addm2TH1(trk1);
	c1->addm2TH1(trk2);
	c1->setYrange(0.5, 1);
	c1->addLegend("upperright");
	c1->labelHist("hist",0); c1->labelHist("fit",1);
	c1->draw(); c1->SaveAs(fig_output+"/tracking_fit_"+sname+format);
	trk2->setName(sname+"_trkEff_p1_fit_*_*");
	trk2->setDirectory(_dir_);
	trk2->write();
}


void bjtc_step3_analyzer::jff_check(){
	auto sube = new jtcTH1Player("correlations_bjetMC_sube/trueB_sube0"+reco_tag(0,0)+"_sig_p0_*_*",npt, ncent);
	auto std  = new jtcTH1Player("correlations_bjetMC_sube/trueB_sube0"+reco_tag(0,0)+"_sig_p0_*_*",npt, ncent);
	sube->autoLoad(fstep2);
}

void bjtc_step3_analyzer::analyze(){
	cout<<"Opening file: "<<output+"/"+step2fname+".root"<<endl;
	fstep2= TFile::Open(output+"/"+step2fname+".root");
	funcert= TFile::Open(output+"/"+step2uncert+".root");
	_dir_ = f->mkdir(_name_);
	if(_dir_==0) _dir_=(TDirectory*) f->Get(_name_);
	f->cd(_name_);
	//bias_check();
	//sube_check();
	//	_dir_->cd();

//working sequence begin-----------------------
	auto jff_bjtc=get_jff_corr("correlations_bjetMC_sube/trueB_sube0", "trueB_sube0_JffCorr");
	get_spillOver_corr("correlations_bjetMC_sube/trueB_subeN0", "trueB_spillCorr");
	get_tagging_biasCorr();
	get_tracking_corr("tagged","correlations_bjetMC_std");

	get_tracking_corr("incl","correlations_djetMC_std");
	auto jff_djtc=get_jff_corr("correlations_djetMC_sube/incl_sube0", "incl_sube0_JffCorr");
	get_spillOver_corr("correlations_djetMC_sube/incl_subeN0", "incl_spillCorr");
/*
*/
//	get_tagging_biasCorr_uncert();
	//taggingBias_uncert();
//working sequence end-----------------------
	//mixing_ratio_check();
	//db_comparison();
	//get_tracking_corr2("incl","correlations_djetMC_std");
//	fitting_tracking("incl","correlations_djetMC_std");
//	fitting_tracking("tagged","correlations_djetMC_std");
//	get_tracking_corr_finebin("incl","correlations_djetMC_std");
//	get_tracking_corr_finebin("tagged","correlations_djetMC_std");
}

