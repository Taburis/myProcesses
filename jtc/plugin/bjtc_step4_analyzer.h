
#include "myProcesses/jtc/plugin/workflow.h"
#include "myProcesses/jtc/plugin/Utility.h"
#include "myProcesses/jtc/plugin/bjtcSignalProducer.h"
#include "myProcesses/jtc/plugin/jtcUti.h"

struct bjtc_wf{
	jtcTH1Player *step_input, *step_decont, *step_trk, *step_bias, *step_jff, *step_bkg, *step_spill;
	jtcTH1Player *bkg_error;
};
struct incl_jtc_wf{
	jtcTH1Player *step_input, *step_trk, *step_jff, *step_bkg, *step_spill;
	jtcTH1Player *bkg_error;
};

class bjtc_step4_analyzer : public analyzer{
	public :
		bjtc_step4_analyzer(TString name, workflow &base0, ParaSet &ps ):analyzer(name,base0, ps){}
		void full_closure_test();
		void produce_data();
		void pre_check();
		bjtc_wf produce_wf001(TString, jtcTH1Player* input, jtcTH1Player*);
		incl_jtc_wf produce_incl_wf001(TString, jtcTH1Player* input);

		void validation_decontamination(); 
		jtcTH1Player* decontamination(jtcTH1Player* j2, jtcTH1Player* negative);
		jtcTH1Player* decontamination_noerror(jtcTH1Player* j2, jtcTH1Player* negative, bool isdata = 0);
		void load_correction(){
			//trkeff = new jtcTH1Player("corrections/tagged_trkEff_p1_fit_*_*", base->npt, base->ncent);
			trkeff = new jtcTH1Player("corrections/tagged_trkEff_p1_smth_*_*", base->npt, base->ncent);
			trkeff->autoLoad(fstep3);
			biaseff = new jtcTH1Player("corrections/tagBias_smth_*_*", base->npt, base->ncent);
			biaseff->autoLoad(fstep3);
			jffCorr = new jtcTH1Player("corrections/trueB_sube0_JffCorr_*_*", base->npt, base->ncent);
			jffCorr ->autoLoad(fstep3);
			spillOver = new jtcTH1Player("corrections/trueB_spillCorr_*_*",base->npt, base->ncent);
			spillOver->autoLoad(fstep3);

			//for inclusive jet
			trkeff_incl = new jtcTH1Player("corrections/incl_trkEff_p1_smth_*_*", base->npt, base->ncent);
			trkeff_incl->autoLoad(fstep3);
			jffCorr_incl = new jtcTH1Player("corrections/incl_sube0_JffCorr_*_*", base->npt, base->ncent);
			jffCorr_incl ->autoLoad(fstep3);
			spillOver_incl = new jtcTH1Player("corrections/incl_spillCorr_*_*", base->npt, base->ncent);
			spillOver_incl ->autoLoad(fstep3);
		}
		void debug_plot(TString savename,jtcTH1Player*j1, jtcTH1Player*j2,TString lab1="",TString lab2="",float xmin =0, float xmax=1, int n= 6,int m= 2);
		void debug_plot_dr(TString savename,jtcTH1Player*j1, jtcTH1Player*j2,TString lab1="",TString lab2="", int n= 6,int m= 2);
		void debug_plot_2D2Dr(TString savename,jtcTH1Player*j1, jtcTH1Player*j2,TString lab1="",TString lab2="");
		void debug_plot_dr_combined(TString savename,jtcTH1Player*j1, jtcTH1Player*j2,TString lab1="",TString lab2="");
		virtual void analyze() override;


		TFile *debugf, *fstep2, *fstep3;
		jtcTH1Player *trkeff, *biaseff, *jffCorr, *spillOver, *trkeff_incl, *jffCorr_incl, *spillOver_incl;
		TString format = ".jpg";
		TFile *fresult;
		TString step2fname, step3fname;
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
	c->setXrange(0,.99);
	c->xtitle="#Delta r";
	c->doLogy= 0;
	c->ytitle="P(#Deltar)";
	c->doHIarrange = 1;
	c->addm2TH1(j2);
	c->addm2TH1(j1);
	c->ratio_title = "Ratio";
	//c->ratio_title = "target/ref.";
	c->addLegend("upperright");
	c->labelHist(lab1, 1);
	c->labelHist(lab2, 0);
	c->setRatioYrange(0., 2.);
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
	auto rs  = new jtcTH1Player("correlations_djetMC_std/tagged"+reco_tag(1,0)+"_sig_p0_dr_*_*", base->npt, base->ncent);
	//	auto rs0 = new jtcTH1Player("correlations_djetMC_std/incl"+reco_tag(1,0)+"_sig_p0_dr_*_*", base->npt, base->ncent);
	auto rsn = new jtcTH1Player("correlations_djetMC_std/negTag"+reco_tag(1,0)+"_sig_p0_dr_*_*", base->npt, base->ncent);
	auto ref = new jtcTH1Player("correlations_djetMC_std/tagTrue"+reco_tag(1,0)+"_sig_p0_dr_*_*", base->npt, base->ncent);
	rs->autoLoad(fstep2);
	//	rs0->autoLoad(fstep2);
	ref->autoLoad(fstep2);
	rsn->autoLoad(fstep2);


	float mistagRate[2];
	auto purity = (TH1F*)fstep2->Get("correlations_djetMC_std/hp");
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
	auto purity = (TH1F*)fstep2->Get("correlations_djetMC_std/hp");
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

jtcTH1Player* bjtc_step4_analyzer::decontamination_noerror(jtcTH1Player* j2, jtcTH1Player* negative, bool isdata){
	float mistagRate[2];
	auto purity = (TH1F*)fstep2->Get("correlations_djetMC_std/hp");
	mistagRate[0] = 1-purity->GetBinContent(1);
	mistagRate[1] = 1-purity->GetBinContent(2);
	float scale = 1.0;
	if( isdata){	
	}
	auto scaled_negative = (jtcTH1Player* ) negative->clone("negTag");	
	for(int i=0; i< negative->Nrow(); ++i){
		for(int j=0; j< negative->Ncol(); ++j){
			scaled_negative->at(i,j)->Scale(mistagRate[j]);
		}
	}
	auto step2 = (jtcTH1Player* ) j2->clone("sig_decont");
	for(int i=0; i< negative->Nrow(); ++i){
		for(int j=0; j< negative->Ncol(); ++j){
			TH1* h = step2->at(i,j);
			TH1* h2= scaled_negative->at(i,j);
			for(int k=1; k< h->GetNbinsX()+1; ++k){
				for(int l=1; l< h->GetNbinsY()+1; ++l){
					double c = h->GetBinContent(k,l)-h2->GetBinContent(k,l);
					c=c/(1.0-mistagRate[j]);
					h->SetBinContent(k,l, c);	
				}
			}
	
		}
	}
	return step2;
}


void bjtc_step4_analyzer::produce_data(){
	auto reff = TFile::Open("../data/AN17337/AN17337Result_new.root");
	auto ref = new jtcTH1Player("dr_signal_bjtc_jetShape_step3_*_*", 6, 1);
	auto incl_pp = new jtcTH1Player("dr_signal_dijtc_jetShape_step3_*_*", 6, 1);
	ref->autoLoad(reff);
	incl_pp->autoLoad(reff);
	auto js_incl_pp = incl_pp->contractX("inclJetShapes");
	js_incl_pp->duplicateY("inclJetShapes_pp", 2);
	auto js_b_pp = ref->contractX("bjetShapes");
	js_b_pp->duplicateY("bjetShapes_pp", 2);

	auto rs = new jtcTH1Player("correlations_HIHardProbe_jet80/tagged"+reco_tag(1,1)+"_sig_p1_*_*", base->npt, base->ncent);
	auto ns = new jtcTH1Player("correlations_HIHardProbe_jet80/negTag"+reco_tag(1,1)+"_sig_p1_*_*", base->npt, base->ncent);
	rs->autoLoad(fstep2);
	ns->autoLoad(fstep2);

	//	auto reff_incl = TFile::Open("../data/jetShap_inclusive_Jussi.root");
	//	auto ref_incl = new jtcTH1Player("jtc_inclusive_pTweighted_P*C*",1, 2);	
	//	ref_incl->autoLoad(reff_incl);


	auto rs_incl = new jtcTH1Player("correlations_HIHardProbe_jet80/incl"+reco_tag(1,1)+"_sig_p2_*_*", base->npt, base->ncent);
	rs_incl->autoLoad(fstep2);
	auto hist = produce_wf001("bjet_data_js_jet80", rs, ns);
	auto hist_incl = produce_incl_wf001("inclusive_data_js_jet80", rs_incl);
	auto incl_Pb = hist_incl.step_spill->contractX("inclusive_reference_*_*");

	auto final_dr    = hist.step_spill   ->contractX("probe_jff_sum_*_*");

	debug_plot_dr("JetShapeRatio",final_dr, js_b_pp,"PbPb","pp", 1,2);
	debug_plot_dr("JetShapeRatio_incl",final_dr, incl_Pb,"b jet","inclusive", 1,2);

	auto c = prepare_canvas<overlay_pad>("c", 6,2);
	c->addm2TH1(hist.step_spill);
	c->addm2TH1(hist_incl.step_spill);
	c->labelHist("b jet", 0);
	c->labelHist("iinclusive", 1);
	c->draw();
	c->SaveAs(fig_output+"/JetShapeRatio_bToInclusive_pTBin"+format);

	auto ratio_pb = final_dr->divide(*incl_Pb);
	auto ratio_pp = js_b_pp->divide(*js_incl_pp);
	auto ratio_incl = incl_Pb->divide(*js_incl_pp);

	ratio_pb->at(0,0)->SetTitle("Cent:0-30%, pT>1 GeV");
	ratio_pb->at(0,1)->SetTitle("Cent:30-90%, pT>1 GeV");
	auto cm = new multi_pads<base_pad>("cm","", 1, 2);
	cm->doHIarrange =1;
	cm->setXrange(0,.99);
	cm->setYrange(0.5,2);
	cm->addm2TH1(ratio_pb);
	cm->addm2TH1(ratio_pp);
	cm->addLegend("upperright");
	cm->labelHist("PbPb", 0);
	cm->labelHist("pp", 1);
	cm->xtitle = "#Deltar";
	cm->draw();
	cm->SaveAs(fig_output+"/ratio_overlay_Pb_vs_pp"+format);

	ratio_incl->at(0,0)->SetTitle("Cent:0-30%, pT>1 GeV");
	ratio_incl->at(0,1)->SetTitle("Cent:30-90%, pT>1 GeV");
	auto cm2 = new multi_pads<base_pad>("cm2","", 1, 2);
	cm2->doHIarrange =1;
	cm2->setXrange(0,.99);
	cm2->setYrange(0.5,2);
	cm2->addm2TH1(ratio_incl);
	cm2->xtitle = "#Deltar";
	cm2->draw();
	cm2->SaveAs(fig_output+"/ratio_overlay_inclusive"+format);
	

	auto f = TFile::Open("debug.root", "recreate");
	hist_incl.step_spill->write();
	f->Close();
}

void bjtc_step4_analyzer::full_closure_test(){
	auto rs = new jtcTH1Player("correlations_djetMC_std/tagged"+reco_tag(1,1)+"_sig_p1_*_*", base->npt, base->ncent);
	rs->autoLoad(fstep2);
	auto ns = new jtcTH1Player("correlations_djetMC_std/negTag"+reco_tag(1,1)+"_sig_p1_*_*", base->npt, base->ncent);
	ns->autoLoad(fstep2);
	auto ref_decont = new jtcTH1Player("correlations_djetMC_std/tagTrue"+reco_tag(1,1)+"_sig_p1_dr_*_*", base->npt, base->ncent);
	auto ref_trk = new jtcTH1Player("correlations_djetMC_std/tagTrue"+reco_tag(1,0)+"_sig_p2_dr_*_*", base->npt, base->ncent);
	auto ref_bias = new jtcTH1Player("correlations_bjetMC_std/trueB"+reco_tag(1,0)+"_sig_p2_dr_*_*", base->npt, base->ncent);
	auto ref_jff = new jtcTH1Player("correlations_bjetMC_std/trueB"+reco_tag(0,0)+"_sig_p2_dr_*_*", base->npt, base->ncent);
	auto ref_spill = new jtcTH1Player("correlations_bjetMC_sube/trueB_sube0"+reco_tag(0,0)+"_sig_p2_dr_*_*", base->npt, base->ncent);
	auto ref_bkg = new jtcTH1Player("correlations_djetMC_std/tagTrue"+reco_tag(1,1)+"_sig_p2_dr_*_*", base->npt, base->ncent);
	ref_decont->autoLoad(fstep2);
	ref_trk->autoLoad(fstep2);
	ref_bias->autoLoad(fstep2);
	ref_jff->autoLoad(fstep2);
	ref_spill->autoLoad(fstep2);
	ref_bkg->autoLoad(fstep2);

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
	auto ref_spill_sum    = ref_spill   ->contractX("ref_spill_sum_*_*");
	auto ref_bkg_sum    = ref_bkg   ->contractX("ref_bkg_sum_*_*");
	auto probe_decont_sum = probe_decont->contractX("probe_decont_sum_*_*");
	auto probe_trk_sum    = probe_trk   ->contractX("probe_trk_sum_*_*");
	auto probe_bias_sum   = probe_bias  ->contractX("probe_bias_sum_*_*");
	auto probe_jff_sum    = probe_jff   ->contractX("probe_jff_sum_*_*");
	auto probe_spill_sum   =hist.step_spill->contractX("probe_spill_sum_*_*");
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
	debug_plot_dr("closure_pTcombined_bkg_step",probe_bkg_sum, ref_bkg_sum,"bkg. step.","tag&true(RR)", 1,2);
	debug_plot_dr("closure_pTcombined_jff_step",probe_jff_sum, ref_jff_sum,"jff. step.","true(GG)", 1,2);
	debug_plot_dr("closure_pTcombined_spill_step",probe_spill_sum, ref_spill_sum,"Probe.","Gen-level", 1,2);
}

incl_jtc_wf bjtc_step4_analyzer::produce_incl_wf001(TString name, jtcTH1Player* input){
	incl_jtc_wf hist;
	hist.step_input = input;
	hist.step_trk = (jtcTH1Player* ) hist.step_input->clone(name+"_trkStep");	
	hist.step_trk ->ring_corr(trkeff_incl , 2.5, "ra");
	hist.step_jff = hist.step_trk->drIntegral(name+"_jffStep");
	hist.step_jff->addContent( *jffCorr_incl,1,-1);
	//hist.step_jff->add2(name+"_jffStep", *jffCorr_incl,1,1);
	hist.step_spill= (jtcTH1Player*) hist.step_jff->clone(name+"_spillStep");
	hist.step_spill->addContent(*spillOver_incl,1,-1);
	//hist.step_spill->add2(name+"_spillStep", *spillOver_incl,1,-1);
	return hist;
}

bjtc_wf bjtc_step4_analyzer::produce_wf001(TString name, jtcTH1Player* input, jtcTH1Player *negative){
	bjtc_wf hist;
	float mistagRate[2];
	auto purity = (TH1F*)fstep2->Get("correlations_djetMC_std/hp");
	mistagRate[0] = 1-purity->GetBinContent(1);
	mistagRate[1] = 1-purity->GetBinContent(2);
	//debugf = TFile::Open("step4_validation_"+name+".root", "recreate");
	//debugf->cd();
	hist.step_input = input;
//	hist.step_input->write();
	cout<<"---------------------------"<<endl;
	cout<<"    production begin       "<<endl;
	cout<<"---------------------------"<<endl;
	//hist.step_decont = decontamination_noerror(input, negative);
	hist.step_decont = decontamination(input, negative);
	//hist.step_decont->write();
	cout<<"---------------------------"<<endl;
	cout<<"step: decontamination done "<<endl;
	cout<<"---------------------------"<<endl;
	hist.step_bkg = hist.step_decont->bkgSub(name+"_bkgStep", 1.5,2.5);
	cout<<"---------------------------"<<endl;
	cout<<"step: bkg subtraction done "<<endl;
	cout<<"---------------------------"<<endl;
	hist.bkg_error = (jtcTH1Player* ) hist.step_bkg->clone(name+"_bkgError");	
	hist.bkg_error->setDrError(hist.bkg_error->getBkgError());
	hist.step_trk = (jtcTH1Player* ) hist.step_bkg->clone(name+"_trkStep");	
	hist.step_trk ->ring_corr(trkeff , 2.5, "ra");
	//hist.step_trk ->write();
	cout<<"---------------------------"<<endl;
	cout<<"step: tracking corr. done "<<endl;
	cout<<"---------------------------"<<endl;
	hist.step_bias= (jtcTH1Player* ) hist.step_trk->clone(name+"_biasStep");
	hist.step_bias->ring_corr(biaseff,2.5, "era");
	//hist.step_bias->write();
	cout<<"---------------------------"<<endl;
	cout<<"step: tagging corr. done "<<endl;
	cout<<"---------------------------"<<endl;
	//	hist.step_jff = (jtcTH1Player* ) hist.step_bias->clone(name+"_jffStep");
	//	hist.step_jff ->ring_corr(jffCorr,0.5, "ra");
	//	hist.step_jff ->write();	
	//auto jff2d = hist.step_bias->drIntegral(name+"_jffStep0");
	hist.step_jff = hist.step_bias->drIntegral(name+"_jffStep0");
	hist.step_jff->addContent(*jffCorr, 1,-1);
	//hist.step_jff =(jtcTH1Player*) jff2d->add2(name+"_jffStep", *jffCorr,1,1);
	//hist.step_jff ->write();	
	cout<<"---------------------------"<<endl;
	cout<<"step: JFF residual corr. done "<<endl;
	cout<<"---------------------------"<<endl;

	hist.step_spill= (jtcTH1Player*) hist.step_jff->clone(name+"_spillStep");
	hist.step_spill->addContent( *spillOver,1,-1);
	//hist.step_spill->add2(name+"_spillStep", *spillOver,1,-1);
	auto res = hist.step_spill->clone(name);
	cout<<"---------------------------"<<endl;
	cout<<"step: Spillover corr. done "<<endl;
	cout<<"---------------------------"<<endl;
	return hist;
}


void bjtc_step4_analyzer::pre_check(){
	auto trkeff_incl = new jtcTH1Player("corrections/incl_trkEff_p1_smth_*_*", base->npt, base->ncent);
	trkeff_incl->autoLoad(fstep3);

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

	auto rec = new jtcTH1Player("correlations_bjetMC_std/trueB"+reco_tag(1,0)+"_sig_p2_*_*",base->npt, base->ncent);
        auto gen = new jtcTH1Player("correlations_bjetMC_sube/trueB_sube0"+reco_tag(0,0)+"_sig_p2_*_*",base->npt, base->ncent);
        rec->autoLoad(fstep2);
        gen->autoLoad(fstep2);
	auto dr_rec = rec->drIntegral("trueb_dr_rec_*_*");	
	auto dr_gen = gen->drIntegral("trueb_dr_gen_*_*");	
	auto dr_diff = dr_rec->add2("diff_trueb_jff", *dr_gen, 1, -1);
	auto cjff = prepare_canvas<base_pad>("cjff", base->npt,base->ncent);
	auto sum = jffCorr->add2("summed_check", *spillOver, 1, 1);
	cjff->addm2TH1(dr_diff);
	cjff->addm2TH1(sum);
	cjff->labelHist("diff", 0);
	cjff->labelHist("jff", 1);
	cjff->draw();
	cjff->SaveAs(fig_output+"/probeJFF_trueB"+format);
}

void bjtc_step4_analyzer::analyze(){
	fstep2 = TFile::Open(output+"/"+step2fname+".root");
	fstep3 = TFile::Open(output+"/"+step3fname+".root");
	load_correction();	
	//pre_check();
	produce_data();
	full_closure_test();
	//	validation_decontamination();
}

