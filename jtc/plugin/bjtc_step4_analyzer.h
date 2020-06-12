
#include "myProcesses/jtc/plugin/workflow.h"
#include "myProcesses/jtc/plugin/Utility.h"
#include "myProcesses/jtc/plugin/bjtcSignalProducer.h"
#include "myProcesses/jtc/plugin/jtcUti.h"

struct bjtc_wf{
	jtcTH1Player *step_input, *step_decont, *step_trk, *step_bias, *step_jff, *step_bkg;
};

class bjtc_step4_analyzer : public analyzer{
	public :
		bjtc_step4_analyzer(TString name, workflow &base0, ParaSet &ps ):analyzer(name,base0, ps){}
		void full_closure_test();
		void produce_data();
		void pre_check();
		jtcTH1Player* produce_workflow(TString, jtcTH1Player* input);
		bjtc_wf produce_wf001(TString, jtcTH1Player* input, jtcTH1Player*);
		void data_production();
		void validation_decontamination(); 
		jtcTH1Player* decontamination(jtcTH1Player* j2, jtcTH1Player* negative);
		void load_correction(){
			//trkeff_incl = new jtcTH1Player("corrections/incl_trkEff_p1_smth_*_*", base->npt, base->ncent);
			//trkeff_incl->autoLoad(base->wf);
			trkeff = new jtcTH1Player("corrections/tagged_trkEff_p1_smth_*_*", base->npt, base->ncent);
			trkeff->autoLoad(base->wf);
			biaseff = new jtcTH1Player("corrections/tagBias_smth_*_*", base->npt, base->ncent);
			biaseff->autoLoad(base->wf);
			jffCorr = new jtcTH1Player("corrections/trueB_sube0_JffCorr_*_*", base->npt, base->ncent);
			jffCorr->autoLoad(base->wf);
		}
		void validation(TString name);
		void link_files(TString f1, TString f2){
			TString datapath = ps->getPara<TString>("output");
			dmc = TFile::Open(datapath+"/"+f1+"_output.root");
			bmc = TFile::Open(datapath+"/"+f2+"_output.root");
		}
		void debug_plot(TString savename,jtcTH1Player*j1, jtcTH1Player*j2,TString lab1="",TString lab2="",float xmin =0, float xmax=1, int n= 6,int m= 2);
		void debug_plot_dr(TString savename,jtcTH1Player*j1, jtcTH1Player*j2,TString lab1="",TString lab2="", int n= 6,int m= 2);
		void debug_plot_2D2Dr(TString savename,jtcTH1Player*j1, jtcTH1Player*j2,TString lab1="",TString lab2="");
		void debug_plot_dr_combined(TString savename,jtcTH1Player*j1, jtcTH1Player*j2,TString lab1="",TString lab2="");
		virtual void analyze() override;

		jtcTH1Player* inclusive_production(TString, jtcTH1Player*);

		TFile *bmc, *dmc, *debugf;
		jtcTH1Player *trkeff_incl, *trkeff, *biaseff, *jffCorr;
		TString format = ".jpg";
};

void bjtc_step4_analyzer::debug_plot(TString savename,jtcTH1Player*j1, jtcTH1Player*j2,TString lab1,TString lab2, float xmin, float xmax, int n, int m){
	auto c =new multi_pads<overlay_pad>("c_"+savename, "", n,m);
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
	c->setRatioYrange(0.5, 1.5);
	c->draw();
	c->SaveAs(fig_output+"/"+savename+format);
}


void bjtc_step4_analyzer::debug_plot_dr(TString savename,jtcTH1Player*j1, jtcTH1Player*j2,TString lab1,TString lab2, int n, int m){
	auto c =new multi_pads<overlay_pad>("c_"+savename, "", n,m);
	c->setXrange(0,1.49);
	c->xtitle="#Delta r";
	c->ytitle="#frac{dN}{d#Deltar}";
	c->doHIarrange = 1;
	c->addm2TH1(j2);
	c->addm2TH1(j1);
	c->ratio_title = "target/ref.";
	c->addLegend("upperright");
	c->labelHist(lab1, 1);
	c->labelHist(lab2, 0);
	c->setRatioYrange(0.5, 1.5);
	c->draw();
	c->SaveAs(fig_output+"/"+savename+format);
}


void bjtc_step4_analyzer::debug_plot_dr_combined(TString savename,jtcTH1Player*j1, jtcTH1Player*j2,TString lab1,TString lab2){
	auto js2 = j2->contractY(savename+"_s2");
	auto js1 = j1->contractY(savename+"_s1");
	auto c =new multi_pads<overlay_pad>("c_"+savename, "", base->npt, 1);
	c->setXrange(0,2.49);
	c->xtitle="#Delta r";
	c->ytitle="#frac{dN}{d#Deltar}";
	c->doHIarrange = 1;
	c->addm2TH1(js2);
	c->addm2TH1(js1);
	c->ratio_title = "target/ref.";
	c->addLegend("upperright");
	c->labelHist(lab1, 1);
	c->labelHist(lab2, 0);
	c->setRatioYrange(0.5, 1.5);
	c->draw();
	c->SaveAs(fig_output+"/"+savename+format);
}

void bjtc_step4_analyzer::validation_decontamination(){
	//auto rs = new jtcTH1Player("correlations_djetMC/tagged_sube0"+reco_tag(1,0)+"_sig_p0_dr_*_*", base->npt, base->ncent);
	//auto rs0 = new jtcTH1Player("correlations_djetMC/incl_sube0"+reco_tag(1,0)+"_sig_p0_dr_*_*", base->npt, base->ncent);
	//auto rsn = new jtcTH1Player("correlations_djetMC/negTag_sube0"+reco_tag(1,0)+"_sig_p0_dr_*_*", base->npt, base->ncent);
	//auto ref = new jtcTH1Player("correlations_djetMC/tagTrue_sube0"+reco_tag(1,0)+"_sig_p0_dr_*_*", base->npt, base->ncent);
	auto rs  = new jtcTH1Player("correlations_djetMC_std/tagged"+reco_tag(1,0)+"_sig_p0_dr_*_*", base->npt, base->ncent);
	//	auto rs0 = new jtcTH1Player("correlations_djetMC_std/incl"+reco_tag(1,0)+"_sig_p0_dr_*_*", base->npt, base->ncent);
	auto rsn = new jtcTH1Player("correlations_djetMC_std/negTag"+reco_tag(1,0)+"_sig_p0_dr_*_*", base->npt, base->ncent);
	auto ref = new jtcTH1Player("correlations_djetMC_std/tagTrue"+reco_tag(1,0)+"_sig_p0_dr_*_*", base->npt, base->ncent);
	rs->autoLoad(base->wf);
	//	rs0->autoLoad(base->wf);
	ref->autoLoad(base->wf);
	rsn->autoLoad(base->wf);


	float mistagRate[2];
	auto purity = (TH1F*)base->wf->Get("correlations_djetMC_std/hp");
	mistagRate[0] = 1-purity->GetBinContent(1);
	mistagRate[1] = 1-purity->GetBinContent(2);

	debug_plot_dr("debug_decont_raw_input",rs, rsn,"tagged p0","neg p0");
	//debug_plot_dr("debug_decont_raw_input",rs, rs0,"tagged p0","incl p0");

	auto step1 = (jtcTH1Player* ) rs->clone("tag_trkStep");	
	auto step1_incl = (jtcTH1Player* ) rsn->clone("negTag");	
	//auto step1_incl = (jtcTH1Player* ) rs0->clone("incl_trkStep");	
	for(int i=0; i< step1_incl->Nrow(); ++i){
		for(int j=0; j< step1_incl->Ncol(); ++j){
			step1_incl->at(i,j)->Scale(mistagRate[j]);
		}
	}
	debug_plot_dr("debug_decont_scaled", step1_incl, rsn,"scaled-p0","neg-p0");
	//debug_plot_dr("debug_decont_scaled", step1_incl, rs0,"scaled-p0","incl-p0");
	auto step2 = (jtcTH1Player* ) step1->add2("tag_deconStep", *step1_incl, 1, -1);
	debug_plot_dr("debug_decont_subtracted",step2, step1,"sub-tagged-p0","tagged-p0");
	for(int i=0; i< step1_incl->Nrow(); ++i){
		for(int j=0; j< step1_incl->Ncol(); ++j){
			step2->at(i,j)->Scale(1.0/(1.-mistagRate[j]));
		}
	}
	debug_plot_dr("debug_decont_scaledback",step2, step1,"decont-tagged-p0","tagged-p0");
	debug_plot_dr("debug_decont_reference",step2, ref,"decont-tagged-p0","tagTrue-p0");
	debug_plot_dr_combined("debug_decont_reference_combined",step2, ref,"decont-tagged-p0","tagTrue-p0");
	debug_plot_dr("debug_decont_decoBias",rs, ref,"tagged-p0","tagTrue-p0");
	debug_plot_dr("debug_decont_neg_vs_tagTrue",rsn, ref,"negTag-p0","tagTrue-p0");
	//	debug_plot_dr("debug_decont_incl_vs_tagTrue",rs0, ref,"incl-p0","tagTrue-p0");
}

jtcTH1Player* bjtc_step4_analyzer::decontamination(jtcTH1Player* j2, jtcTH1Player* negative){
	float mistagRate[2];
	auto purity = (TH1F*)base->wf->Get("correlations_djetMC_std/hp");
	mistagRate[0] = 1-purity->GetBinContent(1);
	mistagRate[1] = 1-purity->GetBinContent(2);
	auto scaled_negative = (jtcTH1Player* ) negative->clone("negTag");	
	for(int i=0; i< negative->Nrow(); ++i){
		for(int j=0; j< negative->Ncol(); ++j){
			scaled_negative->at(i,j)->Scale(mistagRate[j]);
		}
	}
	auto step2 = (jtcTH1Player* ) j2->add2("sig_decont", *scaled_negative, 1, -1);
	for(int i=0; i< negative->Nrow(); ++i){
		for(int j=0; j< negative->Ncol(); ++j){
			step2->at(i,j)->Scale(1.0/(1.-mistagRate[j]));
		}
	}
	return step2;
}

jtcTH1Player* bjtc_step4_analyzer::inclusive_production(TString name, jtcTH1Player *rs){
	auto incl_step1 = (jtcTH1Player*) rs->clone(name+"_step1_tack");
	incl_step1->ring_corr(trkeff_incl, 2.5);
	incl_step1->bkgSub(name);
	auto c =new multi_pads<overlay_pad>("valid_incl_tracking", "", base->npt, base->ncent);
	return incl_step1;
}

void bjtc_step4_analyzer::produce_data(){
	auto rs = new jtcTH1Player("correlations_HIHardProbe_jet80/tagged"+reco_tag(1,1)+"_sig_p1_*_*", base->npt, base->ncent);
	auto ns = new jtcTH1Player("correlations_HIHardProbe_jet80/negTag"+reco_tag(1,1)+"_sig_p1_*_*", base->npt, base->ncent);
	rs->autoLoad(base->wf);
	ns->autoLoad(base->wf);
	auto hist = produce_wf001("bjet_data_jet80", rs, ns);
}

void bjtc_step4_analyzer::full_closure_test(){
	auto rs = new jtcTH1Player("correlations_djetMC_std/tagged"+reco_tag(1,1)+"_sig_p1_*_*", base->npt, base->ncent);
	rs->autoLoad(base->wf);
	auto ns = new jtcTH1Player("correlations_djetMC_std/negTag"+reco_tag(1,1)+"_sig_p1_*_*", base->npt, base->ncent);
	ns->autoLoad(base->wf);
	auto ref_decont = new jtcTH1Player("correlations_djetMC_std/tagTrue"+reco_tag(1,1)+"_sig_p1_dr_*_*", base->npt, base->ncent);
	auto ref_trk = new jtcTH1Player("correlations_djetMC_std/tagTrue"+reco_tag(1,0)+"_sig_p2_dr_*_*", base->npt, base->ncent);
	auto ref_bias = new jtcTH1Player("correlations_djetMC_std/trueB"+reco_tag(1,0)+"_sig_p2_dr_*_*", base->npt, base->ncent);
	auto ref_jff = new jtcTH1Player("correlations_djetMC_std/trueB"+reco_tag(0,0)+"_sig_p2_dr_*_*", base->npt, base->ncent);
	auto ref_bkg = new jtcTH1Player("correlations_djetMC_std/tagTrue"+reco_tag(1,1)+"_sig_p2_dr_*_*", base->npt, base->ncent);
	ref_decont->autoLoad(base->wf);
	ref_trk->autoLoad(base->wf);
	ref_bias->autoLoad(base->wf);
	ref_jff->autoLoad(base->wf);
	ref_bkg->autoLoad(base->wf);

	auto hist = produce_wf001("bjetMC", rs, ns);
	auto probe_decont=hist.step_decont->drIntegral("probe_decont_*_*");
	auto probe_trk   =hist.step_trk   ->drIntegral("probe_trk_*_*");
	auto probe_bias  =hist.step_bias  ->drIntegral("probe_bias_*_*");
	auto probe_jff   =hist.step_jff;
	//auto probe_jff   =hist.step_jff   ->drIntegral("probe_jff_*_*");
	auto probe_bkg   =hist.step_bkg   ->drIntegral("probe_bkg_*_*");
	auto ref_decont_sum = ref_decont->contractX("ref_decont_sum_*_*");
	auto ref_trk_sum    = ref_trk   ->contractX("ref_trk_sum_*_*");
	auto ref_bias_sum   = ref_bias  ->contractX("ref_bias_sum_*_*");
	auto ref_jff_sum    = ref_jff   ->contractX("ref_jff_sum_*_*");
	auto ref_bkg_sum    = ref_bkg   ->contractX("ref_bkg_sum_*_*");
	auto probe_decont_sum = probe_decont->contractX("probe_decont_sum_*_*");
	auto probe_trk_sum    = probe_trk   ->contractX("probe_trk_sum_*_*");
	auto probe_bias_sum   = probe_bias  ->contractX("probe_bias_sum_*_*");
	auto probe_jff_sum    = probe_jff   ->contractX("probe_jff_sum_*_*");
	auto probe_bkg_sum    = probe_bkg   ->contractX("probe_bkg_sum_*_*");
	debug_plot_dr("closure_decont_step",probe_decont, ref_decont,"decont. step","tag&true(RR)");
	debug_plot_dr("closure_trk_step",probe_trk, ref_trk,"trk step.","tag&true(RG)");
	debug_plot_dr("closure_bias_step",probe_bias, ref_bias,"bias step.","true(RG)");
	debug_plot_dr("closure_jff_step",probe_jff, ref_jff,"jff. step.","true(GG)");

	auto deta_sig_bkg = hist.step_bkg->projX("valid_bkg_sig_deta_p1_*_*", -1, 1, "e", 0);
	auto deta_sb_bkg = hist.step_bkg->projX("valid_bkg_sb_deta_p1_*_*", 1.8, 2.2, "e", 0);
	deta_sig_bkg->scale(0.5);
	deta_sb_bkg->scale(1.0/0.4);
	deta_sig_bkg->rebin(5);
	deta_sb_bkg->rebin(5);
	debug_plot("closure_sbCheck_bkg",deta_sig_bkg, deta_sb_bkg,"signal","sideband", -2.5, 2.5);

	auto deta_sig_decont = hist.step_decont->projX("valid_decont_sig_deta_p1_*_*", -1, 1, "e", 0);
	auto deta_sb_decont = hist.step_decont->projX("valid_decont_sb_deta_p1_*_*", 1.8, 2.2, "e", 0);
	deta_sig_decont->scale(0.5);
	deta_sb_decont->scale(1.0/0.4);
	deta_sig_decont->rebin(5);
	deta_sb_decont->rebin(5);
	debug_plot("closure_sbCheck_decont",deta_sig_decont, deta_sb_decont,"signal","sideband", -2.5, 2.5);

	auto deta_sig_trk = hist.step_input->projX("valid_trk_sig_deta_p1_*_*", -1, 1, "e", 0);
	auto deta_sb_trk = hist.step_input->projX("valid_trk_sb_deta_p1_*_*", 1.8, 2.2, "e", 0);
	deta_sig_trk->scale(0.5);
	deta_sb_trk->scale(1.0/0.4);
	deta_sig_trk->rebin(5);
	deta_sb_trk->rebin(5);
	debug_plot("closure_sbCheck_trk",deta_sig_trk, deta_sb_trk,"signal","sideband", -2.5, 2.5);

	debug_plot_dr("closure_pTcombined_decont_step",probe_decont_sum, ref_decont_sum,"decont. step","tag&true(RR)",1,2);
	debug_plot_dr("closure_pTcombined_trk_step"  ,probe_trk_sum, ref_trk_sum,"trk step.","tag&true(RG)",1,2);
	debug_plot_dr("closure_pTcombined_bias_step",probe_bias_sum, ref_bias_sum,"bias step.","true(RG)",1,2);
	debug_plot_dr("closure_pTcombined_jff_step",probe_jff_sum, ref_jff_sum,"jff. step.","true(GG)", 1,2);
	debug_plot_dr("closure_pTcombined_bkg_step",probe_bkg_sum, ref_bkg_sum,"bkg. step.","tag&true(RR)", 1,2);
}

bjtc_wf bjtc_step4_analyzer::produce_wf001(TString name, jtcTH1Player* input, jtcTH1Player *negative){	
	bjtc_wf hist;
	float mistagRate[2];
	auto purity = (TH1F*)base->wf->Get("correlations_djetMC_std/hp");
	mistagRate[0] = 1-purity->GetBinContent(1);
	mistagRate[1] = 1-purity->GetBinContent(2);
	debugf = TFile::Open("step4_validation_"+name+".root", "recreate");
	debugf->cd();
	hist.step_input = input;
	hist.step_input->write();
	cout<<"---------------------------"<<endl;
	cout<<"    production begin       "<<endl;
	cout<<"---------------------------"<<endl;
	hist.step_decont = decontamination(input, negative);
	hist.step_decont->write();
	cout<<"---------------------------"<<endl;
	cout<<"step: decontamination done "<<endl;
	cout<<"---------------------------"<<endl;
	hist.step_bkg = hist.step_decont->bkgSub(name+"_bkgStep", 1.5,2.5);
	cout<<"---------------------------"<<endl;
	cout<<"step: bkg subtraction done "<<endl;
	cout<<"---------------------------"<<endl;
	hist.step_trk = (jtcTH1Player* ) hist.step_bkg->clone(name+"_trkStep");	
	hist.step_trk ->ring_corr(trkeff , 2.5, 1, 1);
	hist.step_trk ->write();
	cout<<"---------------------------"<<endl;
	cout<<"step: tracking corr. done "<<endl;
	cout<<"---------------------------"<<endl;
	hist.step_bias= (jtcTH1Player* ) hist.step_trk->clone(name+"_biasStep");
	hist.step_bias->ring_corr(biaseff,2.5, 1, 1);
	hist.step_bias->write();
	cout<<"---------------------------"<<endl;
	cout<<"step: tagging corr. done "<<endl;
	cout<<"---------------------------"<<endl;
//	hist.step_jff = (jtcTH1Player* ) hist.step_bias->clone(name+"_jffStep");
//	hist.step_jff ->ring_corr(jffCorr,0.5, 1, 0);
//	hist.step_jff ->write();	
	hist.step_jff = hist.step_bias->drIntegral(name+"_jffStep0");
	hist.step_jff->add2(name+"_jffStep", *jffCorr,1,1);
	hist.step_jff ->write();	
	cout<<"---------------------------"<<endl;
	cout<<"step: JFF residual corr. done "<<endl;
	cout<<"---------------------------"<<endl;
	return hist;
}

jtcTH1Player* bjtc_step4_analyzer::produce_workflow(TString name, jtcTH1Player* rs){
	auto res0 =(jtcTH1Player*) rs->clone(name+"_pre");
	res0->ring_corr(trkeff , 2.5);
	res0->ring_corr(biaseff, 2.5);
	auto res = res0->bkgSub(name);
	return res;
}

void bjtc_step4_analyzer::data_production(){
	auto reff = TFile::Open("../data/AN17337Result_new.root");
	auto ref = new jtcTH1Player("dr_signal_bjtc_jetShape_step3_*_*", 6, 1);
	ref->autoLoad(reff);
	auto ref_dr = ref->contractX("bjetShapes");
	ref_dr->duplicateX("bjetShapes_pp", 2);
	auto rs = new jtcTH1Player("correlations_data_nominal/tagged"+reco_tag(1,1)+"_sig_p1_*_*", base->npt, base->ncent);
	rs->autoLoad(base->wf);
	auto data = produce_workflow("data_nominal", rs);
	auto c =new multi_pads<base_pad>("clData_dr", "", base->npt, base->ncent);
	auto data_dr = data->drIntegral("data_dr");
	auto data_tot = data_dr->contractX("data_sum_dr");
	c->setXrange(0,0.599);
	c->xtitle="#Delta r";
	c->ytitle="#frac{dN}{d#Deltar}";
	c->doHIarrange = 1;
	c->addm2TH1(data_dr);
	//	c->ratio_title = "corr./ref.";
	//	c->addLegend("upperright");
	//	c->labelHist("data", 0);
	//	c->setRatioYrange(0, 2);
	c->draw();
	c->SaveAs(fig_output+"/data_nominal_dr_"+format);
	auto c2 =new multi_pads<base_pad>("clData2_dr", "", 1, base->ncent);
	c2->setXrange(0,0.599);
	c2->xtitle="#Delta r";
	c2->ytitle="#frac{dN}{d#Deltar}";
	c2->doHIarrange = 1;
	c2->addm2TH1(data_tot);
	c2->addm2TH1(ref_dr);
	c2->draw();
	c2->SaveAs(fig_output+"/data_nominal_summeddr_"+format);
}


void bjtc_step4_analyzer::validation(TString name){
	auto step0 = new jtcTH1Player(name+"_bjetInput_*_*", base->npt, base->ncent );
	step0->autoLoad(debugf);
	auto step1 = new jtcTH1Player(name+"_trkStep_*_*", base->npt, base->ncent);
	step1->autoLoad(debugf);
	auto step2 = new jtcTH1Player(name+"_deconStep_*_*", base->npt, base->ncent);
	step2->autoLoad(debugf);

	auto ref1 = new jtcTH1Player("correlations_djetMC_std/tagged"+reco_tag(1,0)+"_sig_p1_*_*", base->npt, base->ncent);
	ref1->autoLoad(base->wf);
	auto ref2 = new jtcTH1Player("correlations_bjetMC_std/tagTrue"+reco_tag(1,0)+"_sig_p1_*_*", base->npt, base->ncent);
	ref2->autoLoad(base->wf);

	auto step1dr = step1->drIntegral("step1dr");	
	auto ref1dr = ref1->drIntegral("ref1dr");	
	auto step2dr = step2->drIntegral("step2dr");	
	auto ref2dr = ref2->drIntegral("ref2dr");	
	auto c =new multi_pads<overlay_pad>("debug_step1_tracking", "", base->npt, base->ncent);
	c->setXrange(0,0.9);
	c->xtitle="#Delta r";
	c->ytitle="#frac{dN}{d#Deltar}";
	c->doHIarrange = 1;
	c->addm2TH1(ref1dr);
	c->addm2TH1(step1dr);
	c->ratio_title = "corr./ref.";
	c->addLegend("upperright");
	c->labelHist("Reco-Gen", 0);
	c->labelHist("track corrected.", 1);
	c->setRatioYrange(0, 2);
	c->draw();
	c->SaveAs(fig_output+"/valid_step1_"+format);

	c =new multi_pads<overlay_pad>("debug_step2_tracking", "", base->npt, base->ncent);
	c->addLegend("upperright");
	c->setXrange(0,2.49);
	c->xtitle="#Delta r";
	c->ytitle="#frac{dN}{d#Deltar}";
	c->doHIarrange = 1;
	c->addm2TH1(ref2dr);
	c->addm2TH1(step2dr);
	c->ratio_title = "corr./ref.";
	c->labelHist("tag-true", 0);
	c->labelHist("decont.", 1);
	c->setRatioYrange(.5, 1.5);
	c->draw();
	c->SaveAs(fig_output+"/valid_step2_"+format);
}

void bjtc_step4_analyzer::pre_check(){
	auto trkeff_incl = new jtcTH1Player("corrections/incl_trkEff_p1_smth_*_*", base->npt, base->ncent);
	trkeff_incl->autoLoad(base->wf);
	
	auto c2 =new multi_pads<base_pad>("cl_trkCheck_dr", "", base->npt, base->ncent);
	c2->setXrange(0,2.49);
	c2->setYrange(0.4,1);
	c2->xtitle="#Delta r";
//	c2->ytitle="#frac{dN}{d#Deltar}";
	c2->doHIarrange = 1;
	c2->addm2TH1(trkeff);
	c2->addm2TH1(trkeff_incl);
	c2->addLegend("upperright");
	c2->labelHist("trkEff:b-tag",0);
	c2->labelHist("trkEff:inclusive",1);
	c2->draw();
	c2->SaveAs(fig_output+"/preCheck_trkcorr_"+format);
}

void bjtc_step4_analyzer::analyze(){
	load_correction();	
	pre_check();
	produce_data();
//	full_closure_test();
	//	data_production();
	//auto rs0 = new jtcTH1Player("correlations_djetMC/incl"+reco_tag(1,1)+"_sig_p1_*_*", base->npt, base->ncent);
	//rs0->autoLoad(base->wf);
	//	validation_decontamination();
	//	validation("bjetMC");
	//	if(debugf->IsOpen()) debugf->Close();
}

