
#include "myProcesses/liteFrame/plugin/plots/plotManager.h"
#include "myProcesses/jtc/plugin/workflow.h"
#include "myProcesses/jtc/plugin/bjtcSignalProducer.h"
#include "myProcesses/jtc/plugin/jtcUti.h"

struct bjtc_wf{
	jtcTH1Player *step_input, *step_decont, *step_trk, *step_bias, *step_jff, *step_bkg, *step_spill, *sum;
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
		void produce_data_syst();
		void systUncert_trigger(TString );
		void systUncert_tagBias();
		void systUncert_tagBias_c5shift();
		void pre_check();
		void systUncert_JEC();
		void systUncert_decont();
		void systUncert_JER();
		void bkgError();
		void correction_show();
		bjtc_wf produce_wf001(TString, jtcTH1Player* input, jtcTH1Player*);
		bjtc_wf produce_wf002(TString, jtcTH1Player* input, jtcTH1Player*, float *sfs=nullptr);
		incl_jtc_wf produce_incl_wf001(TString, jtcTH1Player* input);

		void validation_decontamination(); 
		jtcTH1Player* decontamination(jtcTH1Player* j2, jtcTH1Player* negative);
		jtcTH1Player* decontamination_noerror(jtcTH1Player* j2, jtcTH1Player* negative, bool isdata = 0);
		jtcTH1Player* decontamination_incl(jtcTH1Player* j2, jtcTH1Player* negative, float * sfs = nullptr);
		void load_correction(){
			//trkeff = new jtcTH1Player("corrections/tagged_trkEff_p1_fit_*_*", base->npt, base->ncent);
			contbias = new jtcTH1Player("corrections/contBias_p0_*_*", base->npt, base->ncent);
			contbias->autoLoad(fstep3);
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
		plotManager* closurePlot_pTsum(TString savename,jtcTH1Player*j1, jtcTH1Player*j2,TString lab1,TString lab2, float ymin, float ymax, int n= 6,int m= 2);
		void debug_plot_2D2Dr(TString savename,jtcTH1Player*j1, jtcTH1Player*j2,TString lab1="",TString lab2="");
		virtual void analyze() override;


		TFile *debugf, *fstep2, *fstep3;
		TFile *fstep2_uncert;
		jtcTH1Player *trkeff, *biaseff, *jffCorr, *spillOver, *trkeff_incl, *jffCorr_incl, *spillOver_incl, *contbias;
		TString format = ".png";
		TFile *fresult;
		TString step2fname, step3fname, step2Uncertfname;
};

void bjtc_step4_analyzer::debug_plot(TString savename,jtcTH1Player*j1, jtcTH1Player*j2,TString lab1,TString lab2, float xmin, float xmax, int n, int m){
	plot_overlay(savename, fig_output, j1, lab1, j2, lab2, xmin, xmax);
}


plotManager* bjtc_step4_analyzer::closurePlot_pTsum(TString savename,jtcTH1Player*j1, jtcTH1Player*j2,TString lab1,TString lab2, float ymin, float ymax, int n, int m){
	gStyle->SetTitleAlign(30);
	TLatex tex; 
	TString cent[] = {"Cent: 0-30%", "Cent: 30-90%"};
	auto c = new plotManager();
	c->initOverlayPad("canvas_"+savename, "", n,m);
	for(int i=0;i<j1->Nrow(); i++){
		for(int j=0;j<j1->Ncol(); j++){
			j1->at(i,j)->SetTitle(cent[j]);
			c->addHist(j1->at(i,j),i,j1->Ncol()-1-j, lab1,"pl");
			c->addHist(j2->at(i,j),i,j1->Ncol()-1-j, lab2,"pl");
			((overlayPad*)c->at(i,j1->Ncol()-1-j))->rymin = 0.;
			((overlayPad*)c->at(i,j1->Ncol()-1-j))->rymax = 2;
		}
	}
	c->doLogy = 1;
	c->setXrange(0, 0.99);
	c->draw();
	c->drawLegend();
	c->c->SaveAs(fig_output+"/"+savename+format);
	return c;
}

void bjtc_step4_analyzer::debug_plot_dr(TString savename,jtcTH1Player*j1, jtcTH1Player*j2,TString lab1,TString lab2, int n, int m){
	auto c = new plotManager();
	c->initOverlayPad("canvas_"+savename, "", n,m);
	for(int i=0;i<j1->Nrow(); i++){
		for(int j=0;j<j1->Ncol(); j++){
			c->addHist(j1->at(i,j),i,j1->Ncol()-1-j, lab1,"pl");
			c->addHist(j2->at(i,j),i,j1->Ncol()-1-j, lab2,"pl");
			//c->at(i,j1->Ncol()-1-j)->doLogy=1;
			((overlayPad*)c->at(i,j1->Ncol()-1-j))->rymin = 0.7;
			((overlayPad*)c->at(i,j1->Ncol()-1-j))->rymax = 1.3;
		}
	}
	c->setXrange(0, 0.99);
	c->draw();
	c->drawLegend();
	c->c->SaveAs(fig_output+"/"+savename+format);

	//	auto c =new multi_pads<overlay_pad>("c_"+savename, "", n,m);
	//	c->setXrange(0,.99);
	//	c->xtitle="#Delta r";
	//	c->doLogy= 0;
	//	c->ytitle="P(#Deltar)";
	//	c->doHIarrange = 1;
	//	c->addm2TH1(j2);
	//	c->addm2TH1(j1);
	//	c->ratio_title = "Ratio";
	//	//c->ratio_title = "target/ref.";
	//	c->addLegend("upperright");
	//	c->labelHist(lab1, 1);
	//	c->labelHist(lab2, 0);
	//	c->setRatioYrange(0., 2.);
	//	c->draw();
	//	c->SaveAs(fig_output+"/"+savename+format);
}



void bjtc_step4_analyzer::validation_decontamination(){
	auto rs  = new jtcTH1Player("correlations_djetMC_std/tagged"+reco_tag(1,0)+"_sig_p2_dr_*_*", base->npt, base->ncent);
	auto cont = new jtcTH1Player("correlations_djetMC_std/cont"+reco_tag(1,0)+"_sig_p2_dr_*_*", base->npt, base->ncent);
	auto incl = new jtcTH1Player("correlations_djetMC_std/incl"+reco_tag(1,0)+"_sig_p2_dr_*_*", base->npt, base->ncent);
	auto rsn = new jtcTH1Player("correlations_djetMC_std/negTag"+reco_tag(1,0)+"_sig_p2_dr_*_*", base->npt, base->ncent);
	auto ref = new jtcTH1Player("correlations_djetMC_std/tagTrue"+reco_tag(1,0)+"_sig_p2_dr_*_*", base->npt, base->ncent);
	cont->autoLoad(fstep2);
	incl->autoLoad(fstep2);
	rs->autoLoad(fstep2);
	ref->autoLoad(fstep2);
	rsn->autoLoad(fstep2);

	float mistagRate[2];
	auto purity = (TH1F*)fstep2->Get("correlations_djetMC_std/hp");
	mistagRate[0] = 1-purity->GetBinContent(1);
	mistagRate[1] = 1-purity->GetBinContent(2);

	//	auto contbias = (jtcTH1Player*) incl->divide(*cont);

	auto step1_cont = (jtcTH1Player*) cont;
	auto step1 = (jtcTH1Player* ) rs->clone("tag_trkStep");	
	auto step1_incl = (jtcTH1Player* ) rsn->clone("negTag");	
	for(int i=0; i< step1_incl->Nrow(); ++i){
		for(int j=0; j< step1_incl->Ncol(); ++j){
			step1_cont->at(i,j)->Scale(mistagRate[j]);
		}
	}
	auto step2 = (jtcTH1Player* ) step1->add2("tag_deconStep", *step1_cont, 1, -1);
	for(int i=0; i< step1_incl->Nrow(); ++i){
		for(int j=0; j< step1_incl->Ncol(); ++j){
			step2->at(i,j)->Scale(1.0/(1.-mistagRate[j]));
		}
	}
	debug_plot_dr("debug_decont_reference",step2, ref,"decont-tagged-p2","tagTrue-p2");
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

jtcTH1Player* bjtc_step4_analyzer::decontamination_incl(jtcTH1Player* j2, jtcTH1Player* incl, float* sfs){
	float sf1=1, sf2=1;
	if(sfs!=nullptr) {
		sf1=sfs[0]; sf2=sfs[1];
	}
	float mistagRate[2];
	auto purity = (TH1F*)fstep2->Get("correlations_djetMC_std/hp");
	mistagRate[0] = 1-sf1*purity->GetBinContent(1);
	mistagRate[1] = 1-sf2*purity->GetBinContent(2);
	auto adjustedIncl = (jtcTH1Player* ) incl->clone("biasAdjustedIncl");
	adjustedIncl->ring_corr(contbias, 1, "a");
	for(int i=0; i< incl->Nrow(); ++i){
		for(int j=0; j< incl->Ncol(); ++j){
			adjustedIncl->at(i,j)->Scale(mistagRate[j]);
		}
	}
	auto step2 = (jtcTH1Player* ) j2->add2("sig_decont", *adjustedIncl, 1, -1);
	for(int i=0; i< incl->Nrow(); ++i){
		for(int j=0; j< incl->Ncol(); ++j){
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

void bjtc_step4_analyzer::produce_data_syst(){

	auto fc5shift= TFile::Open("/eos/user/w/wangx/AN20-029/bjtc_c2bin_50mix_wf001_cwfix/bjtc_step2_output.root");
	auto rs = new jtcTH1Player("correlations_HIHardProbe_jet80or100/tagged"+reco_tag(1,1)+"_sig_p1_*_*", base->npt, base->ncent);
	auto ns = new jtcTH1Player("correlations_HIHardProbe_jet80or100/negTag"+reco_tag(1,1)+"_sig_p1_*_*", base->npt, base->ncent);
	rs->autoLoad(fc5shift);
	ns->autoLoad(fc5shift);
	//auto hist = produce_wf001("bjet_data_js_jet80or100_c5shift",rs, ns);
	auto hist = produce_wf002("bjet_data_js_jet80or100_c5shift",rs, ns);
	auto res = hist.step_spill->contractX("bjtc_c5shift_bkg");

	auto fref = TFile::Open("/eos/user/w/wangx/AN20-029/bjtc_c2bin_50mix_wf001_cwfix/bjtc_step4_output.root");
	auto sig = new jtcTH1Player("js_bjet/js_bjet_data_*_*", base->npt, base->ncent);
	sig->autoLoad(fref);
	auto nominal = sig->contractX("bjs_nominal");

	plot_overlay("systUncert_c5shift",fig_output,nominal,"nominal", res,"hiBin Unshift", 0, 0.99);
	auto wf = TFile::Open("bjtc_syst_c5shift.root", "recreate");
	res->write();
	wf->Close();
}

void bjtc_step4_analyzer::produce_data(){
	auto reff = TFile::Open("../data/HIN18020/AN17337Result_new.root");
	auto ref = new jtcTH1Player("dr_signal_bjtc_jetShape_step3_*_*", 6, 1);
	auto incl_pp = new jtcTH1Player("dr_signal_dijtc_jetShape_step3_*_*", 6, 1);
	ref->autoLoad(reff);
	incl_pp->autoLoad(reff);
	auto js_incl_pp = incl_pp->contractX("inclJetShapes");
	js_incl_pp->duplicateY("inclJetShapes_pp", 2);
	auto js_b_pp = ref->contractX("bjetShapes");
	js_b_pp->duplicateY("bjetShapes_pp", 2);

	auto rs = new jtcTH1Player("correlations_HIHardProbe_trigMerge80and100/tagged"+reco_tag(1,1)+"_sig_p1_*_*", base->npt, base->ncent);
	auto is = new jtcTH1Player("correlations_HIHardProbe_trigMerge80and100/incl"+reco_tag(1,1)+"_sig_p2_*_*", base->npt, base->ncent);
	//auto rs = new jtcTH1Player("correlations_HIHardProbe_jet80or100/tagged"+reco_tag(1,1)+"_sig_p1_*_*", base->npt, base->ncent);
	//auto is = new jtcTH1Player("correlations_HIHardProbe_jet80or100/incl"+reco_tag(1,1)+"_sig_p2_*_*", base->npt, base->ncent);
	rs->autoLoad(fstep2);
	is->autoLoad(fstep2);

	//	auto reff_incl = TFile::Open("../data/jetShap_inclusive_Jussi.root");
	//	auto ref_incl = new jtcTH1Player("jtc_inclusive_pTweighted_P*C*",1, 2);	
	//	ref_incl->autoLoad(reff_incl);


	auto rs_incl = new jtcTH1Player("correlations_HIHardProbe_trigMerge80and100/incl"+reco_tag(1,1)+"_sig_p2_*_*", base->npt, base->ncent);
	rs_incl->autoLoad(fstep2);
	float sfs[2] = {1.28, 1.1};
	auto hist = produce_wf002("bjet_data_js_nominal", rs, is, sfs);
	auto hist_incl = produce_incl_wf001("inclusive_data_js_nominal", rs_incl);
	auto incl_Pb = hist_incl.step_spill->contractX("inclusive_reference_*_*");

	auto final_dr    = hist.step_spill   ->contractX("probe_jff_sum_*_*");

	debug_plot_dr("JetShapeRatio",final_dr, js_b_pp,"PbPb","pp", 1,2);
	debug_plot_dr("JetShapeRatio_incl",final_dr, incl_Pb,"b jet","inclusive", 1,2);

	plot_overlay("JetShapeRatio_bInclusive_pTBin", fig_output, hist.step_spill, "b jet", hist_incl.step_spill, "inclusive", 0, 0.99);

	auto ratio_pb = final_dr->divide(*incl_Pb);
	auto ratio_pp = js_b_pp->divide(*js_incl_pp);
	auto ratio_incl = incl_Pb->divide(*js_incl_pp);

	ratio_pb->at(0,0)->SetTitle("Cent:0-30%, pT>1 GeV");
	ratio_pb->at(0,1)->SetTitle("Cent:30-90%, pT>1 GeV");
	plot_square("ratio_overlay_Pb_vs_pp", fig_output, ratio_pb, "PbPb", ratio_pp, "pp", 0, 0.99);

	ratio_incl->at(0,0)->SetTitle("Cent:0-30%, pT>1 GeV");
	ratio_incl->at(0,1)->SetTitle("Cent:30-90%, pT>1 GeV");
	plot_square("ratio_overlay_inclusive", fig_output, ratio_incl, "incl", 0, 0.99);


	f->cd();
	auto dir_inclusive = f->mkdir("js_inclusive");
	if(dir_inclusive==0) dir_inclusive=(TDirectory*) f->Get("js_inclusive");
	f->cd("js_inclusive");

	hist_incl.step_spill->setName("js_inclusive_data_*_*");
	hist_incl.bkg_error->setName("js_inclusive_data_bkgError_*_*");
	hist_incl.step_spill->setDirectory(dir_inclusive);
	hist_incl.bkg_error ->setDirectory(dir_inclusive);
	hist_incl.step_spill->write();
	hist_incl.bkg_error ->write();

	TString dirname = "js_bjet";
	auto dir_bjet = f->mkdir(dirname);
	if(dir_bjet==0) dir_bjet=(TDirectory*) f->Get(dirname);
	f->cd(dirname);
	hist.step_spill->setName("js_bjet_data_*_*");
	hist.bkg_error->setName("js_bjet_data_bkgError_*_*");
	hist.step_spill->setDirectory(dir_bjet);
	hist.bkg_error ->setDirectory(dir_bjet);
	hist.step_spill->write();
	hist.bkg_error ->write();

}

void bjtc_step4_analyzer::full_closure_test(){
	auto rs = new jtcTH1Player("correlations_djetMC_std/tagged"+reco_tag(1,1)+"_sig_p1_*_*", base->npt, base->ncent);
	rs->autoLoad(fstep2);
	auto ns = new jtcTH1Player("correlations_djetMC_std/incl"+reco_tag(1,1)+"_sig_p2_*_*", base->npt, base->ncent);
	auto is = new jtcTH1Player("correlations_djetMC_std/incl"+reco_tag(1,1)+"_sig_p2_*_*", base->npt, base->ncent);
	//auto ns = new jtcTH1Player("correlations_djetMC_std/negTag"+reco_tag(1,1)+"_sig_p1_*_*", base->npt, base->ncent);
	ns->autoLoad(fstep2);
	is->autoLoad(fstep2);
	auto ref_decont = new jtcTH1Player("correlations_djetMC_std/tagTrue"+reco_tag(1,1)+"_sig_p2_dr_*_*", base->npt, base->ncent);
	auto ref_trk = new jtcTH1Player("correlations_djetMC_std/tagTrue"+reco_tag(1,0)+"_sig_p2_dr_*_*", base->npt, base->ncent);
	auto ref_bias = new jtcTH1Player("correlations_bjetMC_std/trueB"+reco_tag(1,0)+"_sig_p2_dr_*_*", base->npt, base->ncent);
	auto ref_jff = new jtcTH1Player("correlations_bjetMC_std/trueB"+reco_tag(0,0)+"_sig_p2_dr_*_*", base->npt, base->ncent);
	auto ref_spill = new jtcTH1Player("correlations_bjetMC_sube/trueB_sube0"+reco_tag(0,0)+"_sig_p2_dr_*_*", base->npt, base->ncent);
	auto ref_bkg = new jtcTH1Player("correlations_djetMC_std/tagged"+reco_tag(1,1)+"_sig_p2_dr_*_*", base->npt, base->ncent);
	ref_decont->autoLoad(fstep2);
	ref_trk->autoLoad(fstep2);
	ref_bias->autoLoad(fstep2);
	ref_jff->autoLoad(fstep2);
	ref_spill->autoLoad(fstep2);
	ref_bkg->autoLoad(fstep2);


	auto hist = produce_wf002("bjetMC", rs, is);
	//auto hist = produce_wf001("bjetMC", rs, ns);
	auto probe_input =hist.step_input ->drIntegral("probe_input_*_*");
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
	auto probe_input_sum = probe_input->contractX("probe_input_sum_*_*");
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

	closurePlot_pTsum("closure_pTcombined_input_step",probe_input_sum, ref_decont_sum,"input step","tag&true(RR)", -100, 1000,1,2);
	closurePlot_pTsum("closure_pTcombined_decont_step",probe_decont_sum, ref_decont_sum,"decont. step","tag&true(RR)", -100, 1000,1,2);
	closurePlot_pTsum("closure_pTcombined_trk_step"  ,probe_trk_sum, ref_trk_sum,"trk step.","tag&true(RG)", -100, 1100 , 1,2);
	closurePlot_pTsum("closure_pTcombined_bias_step" ,probe_bias_sum, ref_bias_sum,"bias step.","true(RG)", -100, 1100, 1,2);
	//closurePlot_pTsum("closure_pTcombined_bkg_step",ref_bkg_sum, probe_bkg_sum,"bkg. step.","tag&true(RR)",-100,900,  1,2);
	closurePlot_pTsum("closure_pTcombined_bkg_step",probe_bkg_sum, ref_bkg_sum,"bkg. step.","tag&true(RR)",-100,900,  1,2);
	closurePlot_pTsum("closure_pTcombined_jff_step",probe_jff_sum, ref_jff_sum,"jff. step.","true(GG)"   ,-100, 1200, 1,2);
	closurePlot_pTsum("closure_pTcombined_spill_step",probe_spill_sum, ref_spill_sum,"Probe.","Gen-level",-100, 1200, 1,2);


	
	auto sig = new jtcTH1Player("correlations_djetMC_std/tagged"+reco_tag(1,1)+"_sig_p2_*_*", base->npt, base->ncent);
	auto sig_ref = new jtcTH1Player("correlations_bjetMC_sube/trueB_sube0"+reco_tag(0,0)+"_sig_p2_*_*", base->npt, base->ncent);
	sig->autoLoad(fstep2);
	sig_ref->autoLoad(fstep2);
	auto sum_ref = sig_ref->contractX("trueB_err_*_*");
	sum_ref->getBkgError();
	auto ref_err  = ref_spill_sum->clone("Ref_closure");
	jtcTH1Player* probe_err =(jtcTH1Player*) hist.bkg_error->contractX("probe_bkg_error");
	((jtcTH1Player*)ref_err)->setDrError(sum_ref);
	plot_overlay_uncert("fullClosure_pTsummed", fig_output,probe_spill_sum,"Probe.", ref_spill_sum,"Gen-level", probe_err, ref_err, 0,0.99, 1);
	/*
	*/
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
	hist.bkg_error = (jtcTH1Player* ) hist.step_spill->clone(name+"_bkgError");	
	hist.bkg_error->setDrError(hist.step_input->getBkgError());
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
	//hist.step_decont = input;
	hist.step_decont = decontamination_noerror(input, negative);
	//hist.step_decont = decontamination(input, negative);
	//hist.step_decont->write();
	cout<<"---------------------------"<<endl;
	cout<<"step: decontamination done "<<endl;
	cout<<"---------------------------"<<endl;
	hist.step_bkg = hist.step_decont->bkgSub(name+"_bkgStep", 1.5,2.5);
	cout<<"---------------------------"<<endl;
	cout<<"step: bkg subtraction done "<<endl;
	cout<<"---------------------------"<<endl;
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
	hist.sum = (jtcTH1Player*) hist.step_spill->contractX(name+"_pTintegrated");
	hist.bkg_error = (jtcTH1Player* ) hist.step_spill->clone(name+"_bkgError");	
	hist.bkg_error->setDrError(hist.step_bkg->getBkgError());
	cout<<"---------------------------"<<endl;
	cout<<"step: Spillover corr. done "<<endl;
	cout<<"---------------------------"<<endl;
	return hist;
}

bjtc_wf bjtc_step4_analyzer::produce_wf002(TString name, jtcTH1Player* input, jtcTH1Player *incl,float * sfs){
	// input signal: p1, decont: p2
	bjtc_wf hist;
	float mistagRate[2];
	auto purity = (TH1F*)fstep2->Get("correlations_djetMC_std/hp");
	mistagRate[0] = 1-purity->GetBinContent(1);
	mistagRate[1] = 1-purity->GetBinContent(2);
	//mistagRate[0] = 1-purity->GetBinContent(1);
	//mistagRate[1] = 1-purity->GetBinContent(2);
	//debugf = TFile::Open("step4_validation_"+name+".root", "recreate");
	//debugf->cd();
	hist.step_input = input;
	//	hist.step_input->write();
	cout<<"---------------------------"<<endl;
	cout<<"    production begin       "<<endl;
	cout<<"---------------------------"<<endl;
	hist.step_bkg = hist.step_input->bkgSub(name+"_bkgStep", 1.5,2.5);
	cout<<"---------------------------"<<endl;
	cout<<"step: bkg subtraction done "<<endl;
	cout<<"---------------------------"<<endl;
	//hist.step_decont = input;
	hist.step_decont = decontamination_incl(hist.step_bkg, incl,sfs);
	//hist.step_decont->write();
	cout<<"---------------------------"<<endl;
	cout<<"step: decontamination done "<<endl;
	cout<<"---------------------------"<<endl;
	hist.step_trk = (jtcTH1Player* ) hist.step_decont->clone(name+"_trkStep");	
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
	hist.sum = (jtcTH1Player*) hist.step_spill->contractX(name+"_pTintegrated");
	hist.bkg_error = (jtcTH1Player* ) hist.step_spill->clone(name+"_bkgError");	
	hist.bkg_error->setDrError(hist.step_bkg->getBkgError());
	cout<<"---------------------------"<<endl;
	cout<<"step: Spillover corr. done "<<endl;
	cout<<"---------------------------"<<endl;
	return hist;

}

void bjtc_step4_analyzer::pre_check(){
	auto trkeff_incl = new jtcTH1Player("corrections/incl_trkEff_p1_smth_*_*", base->npt, base->ncent);
	trkeff_incl->autoLoad(fstep3);

	plot_square("preCheck_trkcorr", fig_output, trkeff, "trkEff:b-tag", trkeff_incl, "trkEff:inclusive", 0, 0.99, 0.4, 1);

	auto rec = new jtcTH1Player("correlations_bjetMC_std/trueB"+reco_tag(1,0)+"_sig_p2_*_*",base->npt, base->ncent);
	auto gen = new jtcTH1Player("correlations_bjetMC_sube/trueB_sube0"+reco_tag(0,0)+"_sig_p2_*_*",base->npt, base->ncent);
	rec->autoLoad(fstep2);
	gen->autoLoad(fstep2);
	auto dr_rec = rec->drIntegral("trueb_dr_rec_*_*");	
	auto dr_gen = gen->drIntegral("trueb_dr_gen_*_*");	
	auto dr_diff = dr_rec->add2("diff_trueb_jff", *dr_gen, 1, -1);
	auto sum = jffCorr->add2("summed_check", *spillOver, 1, 1);
	plot_square("probeJFF_trueB", fig_output, dr_diff, "diff", sum, "jff", 0, 0.99);
}

void bjtc_step4_analyzer::systUncert_JEC(){

	auto rsup = new jtcTH1Player("correlations_HIHardProbe_jet80or100_JECU_up/tagged"+reco_tag(1,1)+"_sig_p0_dr_*_*", base->npt, base->ncent);
	rsup->autoLoad(fstep2_uncert);

	auto rsdown = new jtcTH1Player("correlations_HIHardProbe_jet80or100_JECU_down/tagged"+reco_tag(1,1)+"_sig_p0_dr_*_*", base->npt, base->ncent);
	rsdown->autoLoad(fstep2_uncert);

	auto rsjer = new jtcTH1Player("correlations_HIHardProbe_jet80or100_JER/tagged"+reco_tag(1,1)+"_sig_p0_dr_*_*", base->npt, base->ncent);
	rsjer->autoLoad(fstep2_uncert);

	auto jsb = new jtcTH1Player("correlations_HIHardProbe_jet80/tagged"+reco_tag(1,1)+"_sig_p0_dr_*_*", base->npt, base->ncent);
	jsb->autoLoad(fstep2);
	auto jsbsum = jsb->contractX("jsbSum");


	auto jecdown = rsup->contractX("bjet_data_systUn_JECDown");	
	auto jecup = rsdown->contractX("bjet_data_systUn_JECUp");	
	auto jer = rsjer->contractX("bjet_data_systUn_JER");	

	debug_plot("systUncert_JECDown",jsbsum, jecdown,"nominal","JEC LB", 0, 0.99, 1, 2);
	debug_plot("systUncert_JECUp",jsbsum, jecup,"nominal","JEC UB", 0, 0.99, 1, 2);
	debug_plot("systUncert_JER",jsbsum, jer,"nominal","JER smeared", 0, 0.99, 1, 2);

	auto f0 = TFile::Open(output+"/bjtc_systUncert.root", "recreate");
	f0->cd();
	jecdown->write();
	jecup->write();
	f0->Close();
}

void bjtc_step4_analyzer::systUncert_decont(){
	auto rs = new jtcTH1Player("correlations_HIHardProbe_trigMerge80and100/tagged"+reco_tag(1,1)+"_sig_p1_*_*", base->npt, base->ncent);
	auto is = new jtcTH1Player("correlations_HIHardProbe_trigMerge80and100/incl"+reco_tag(1,1)+"_sig_p2_*_*", base->npt, base->ncent);
	rs->autoLoad(fstep2);
	is->autoLoad(fstep2);
	float sfs1[2] = {1.28, 1.1};
	float sfs2[2] = {1., 1.};
	auto hist1 = produce_wf002("bjet_data_js_jet80or100_decont", rs, is, sfs2);
	auto hist0 = produce_wf002("bjet_data_js_jet80or100_nominal", rs, is, sfs1);

	auto dr0 = hist0.step_jff->contractX("bjet_data_js_pTsum_nominal");
	auto dr1 = hist1.step_jff->contractX("bjet_data_js_pTsum_probe");

	auto drate =  dr1->divide(*dr0);
	int cut = drate->at(0,0)->GetXaxis()->FindBin(0.4);
	//larger dr error is ignored.
	for(int j= 0; j<base->ncent; j++){
		for(int k=1; k<drate->at(0,0)->GetNbinsX()+1; k++){
			if(k<cut) continue;
			drate->at(0,j)->SetBinContent(k, 1);
		}
	} 

	TString dirname = "syst";
	auto dir = f->mkdir(dirname);
	if(dir==0) dir=(TDirectory*) f->Get(dirname);
	f->cd(dirname);
	drate->setName("syst_decont_*_*");
	drate->setDirectory(dir);
	drate->write();

	plot_overlay("systUncert_decont", fig_output, dr0, "Nominal", dr1, "Variated", 0,0.99);
//	TString dirname = "systUncert";
//	auto dir_bjet = fstep2_uncert->mkdir(dirname);
//	if(dir_bjet==0) dir_bjet=(TDirectory*) fstep2_uncert->Get(dirname);
//	fstep2_uncert->cd(dirname);
//	hist0.step_spill->write();
//	hist1.step_spill->write();
}

void bjtc_step4_analyzer::systUncert_JER(){
	auto js0 = new jtcTH1Player("correlations_HIHardProbe_jet80or100/tagged"+reco_tag(1,1)+"_sig_p2_dr_*_*", base->npt, base->ncent);
	auto js1 = new jtcTH1Player("correlations_HIHardProbe_jet80or100_JER/tagged"+reco_tag(1,1)+"_sig_p2_dr_*_*", base->npt, base->ncent);
	js0->autoLoad(fstep2);
	js1->autoLoad(fstep2_uncert);
	auto jss0 = js0->contractX("nominal");
	auto jss1 = js1->contractX("smeared");
	plot_overlay("systUncert_JER", fig_output, jss0, "Nominal", jss1, "JER smeared", 0,0.99);
}

void bjtc_step4_analyzer::systUncert_trigger(TString type){
	//auto rs1 = new jtcTH1Player("correlations_HIHardProbe_allTrig/"+type+reco_tag(1,1)+"_sig_p0_dr_*_*", base->npt, base->ncent);
	auto rs0 = new jtcTH1Player("correlations_HIHardProbe_jet80or100_85p/"+type+reco_tag(1,1)+"_sig_p2_dr_*_*", base->npt, base->ncent);
	auto rs1 = new jtcTH1Player("correlations_HIHardProbe_trigMerge80and100/"+type+reco_tag(1,1)+"_sig_p2_dr_*_*", base->npt, base->ncent);
	//auto rs0 = new jtcTH1Player("correlations_HIHardProbe_jet80or100/tagged"+reco_tag(1,1)+"_sig_p0_dr_*_*", base->npt, base->ncent);
	//auto rs1 = new jtcTH1Player("correlations_HIHardProbe_trigMerge80and100/tagged"+reco_tag(1,1)+"_sig_p0_dr_*_*", base->npt, base->ncent);
	rs0->autoLoad(fstep2);
	auto rs0sum = rs0->contractX("bjs_sum0");

	rs1->autoLoad(fstep2);
	auto rs1sum = rs1->contractX("bjs_sum1");
	rs1sum->at(0,0)->SetTitle("Signal pT weighted, Cent:0-30%, pT > 1 GeV");
	rs1sum->at(0,1)->SetTitle("Signal pT weighted, Cent:30-90%, pT > 1 GeV");

	auto c = new plotManager();
	c->initOverlayPad("canvas_uncert_trigger_"+type, "", 1,2);
	for(int j=0;j<rs1sum->Ncol(); j++){
			c->addHist(rs1sum->at(0,j),0,rs1sum->Ncol()-1-j, "jet80,100","pl");
			c->addHist(rs0sum->at(0,j),0,rs0sum->Ncol()-1-j, "jet100","pl");
			//c->at(i,j1->Ncol()-1-j)->doLogy=1;
			((overlayPad*)c->at(0,rs1sum->Ncol()-1-j))->rymin = 0.9;
			((overlayPad*)c->at(0,rs1sum->Ncol()-1-j))->rymax = 1.1;
	}
	c->setXrange(0, 0.99);
	c->draw();
	c->drawLegend();
	auto bx = new TBox();
        bx->SetFillColorAlpha(kGray+1, 0.5);
	for(int j=0;j<rs1sum->Ncol(); j++){
		((overlayPad*)c->at(0,j))->downpad->cd();
        	bx->DrawBox(0, 1-0.045, 1, 1+0.045);
	}
	c->c->SaveAs(fig_output+"/systUncert_trigger_"+type+".png");

	debug_plot("systUncert_trigger_ptbin_"+type, rs1, rs0,"Merge jet80and100","jet 100", 0, 0.99, 6, 2);
}

void bjtc_step4_analyzer::systUncert_tagBias(){
	auto biaseff2 = new jtcTH1Player("corrections/tagBias_GSP_smth_*_*", base->npt, base->ncent);
	biaseff2->autoLoad(fstep3);
	plot_overlay("systUncert_taggingBias", fig_output, biaseff, "PYTHIA", biaseff2, "GSP reweighted", 0,0.99);
}
void bjtc_step4_analyzer::systUncert_tagBias_c5shift(){
	biaseff = new jtcTH1Player("corrections/tagBias_smth_*_*", base->npt, base->ncent);
	biaseff->autoLoad(fstep3);
	auto biaseff2 = new jtcTH1Player("corrections/tagBiasC5shift_smth_*_*", base->npt, base->ncent);
	biaseff2->autoLoad(fstep3);
	plot_overlay("systUncert_taggingBias_c5shift", fig_output, biaseff, "Nominal", biaseff2, "Bkg shifted", 0,0.99);

}

void bjtc_step4_analyzer::bkgError(){
	auto sig = new jtcTH1Player("correlations_HIHardProbe_jet80or100_85p/tagged"+reco_tag(1,1)+"_sig_p2_*_*", base->npt, base->ncent);
	sig->autoLoad(fstep2);
	auto c = sig->drawBkgError("bkgError");
	c->save(fig_output+"/systUncert_bkg_ME.png");
}

void bjtc_step4_analyzer::correction_show(){
	auto data = new jtcTH1Player("js_bjet/js_bjet_data_*_*", base->npt, base->ncent);
	data->autoLoad(f);
	//auto rs = new jtcTH1Player("correlations_HIHardProbe_trigMerge80and100/tagged"+reco_tag(1,1)+"_sig_p2_dr_*_*", base->npt, base->ncent);

	auto sum_data = data->contractX("raw_data");
	auto sum_jff = jffCorr->contractX("jffSum");
	auto sum_jff_incl = jffCorr_incl->contractX("jffSum_incl");
	auto sum_spill = spillOver->contractX("spillSum");
//	auto ratio = sum_spill->divide(*sum_data);

	sum_data->at(0,0)->SetTitle("signal: pTweighted Cent: 0-30% pT > 1 GeV");
	sum_data->at(0,1)->SetTitle("signal: pTweighted Cent: 30-90% pT > 1 GeV");
	sum_data->at(0,0)->GetYaxis()->SetTitle("P(#Delta r)");
	sum_data->at(0,1)->GetYaxis()->SetTitle("P(#Delta r)");

	//plot_sigRef("corrections_JFFresidual", fig_output, sum_data, "Data", sum_jff, "Rec-Rec - Rec-Gen",0, 0.99);
	//plot_sigRef("corrections_JFFresidual_incl", fig_output, sum_data, "Data", sum_jff_incl, "Rec-Gen - Gen-Gen",0, 0.99);
	//plot_sigRef("corrections_SpillOver", fig_output, sum_data, "Data", sum_spill, "Gen-Gen - Gen-Gen (sube0)",0, 0.99);
	//plot_square("correction_tracking", fig_output, trkeff, "b jets",trkeff_incl , "incl.", 0, 0.99, 0, 1.2);
auto h0 = sum_jff_incl->at(0,0);
auto h1 = sum_jff_incl->at(0,1);
	for(int i=1; i< h0->GetNbinsX()+1; i++){
		h0->SetBinContent(i, -h0->GetBinContent(i));
		h1->SetBinContent(i, -h1->GetBinContent(i));
		if(h0->GetBinLowEdge(i) > 0.4){
			h0->SetBinContent(i, 0);
			h1->SetBinContent(i, 0);
		}
	}
	auto c0 = new plotManager();
        c0->initOverlayPad("canvas_JFFres_incl", "", 1, 2);
        c0->addm2TH1(sum_data, "data", "pfl");
        c0->addm2TH1(sum_jff_incl, "Gen-Gen - Rec-Gen", "pfl");
	((overlayPad*) c0->at(0,0))->rline = 0;
	((overlayPad*) c0->at(0,1))->rline = 0;
        c0->setXrange(0, .99);
        c0->setRatioYrange(-0.2, 1.1);
        c0->draw();
	c0->drawHLine(0, 2);
        c0->drawLegend("lin2right");
        c0->save(fig_output+"/corrections_JFFresidual_incl.png");

        auto c = new plotManager();
	//ratio->at(0,0)->SetTitle("signal: pTweighted Cent: 0-30% pT > 1 GeV");
	//ratio->at(0,1)->SetTitle("signal: pTweighted Cent: 30-90% pT > 1 GeV");
        c->initOverlayPad("canvas_SpillOver", "", 1, 2);
        c->addm2TH1(sum_data, "data", "pfl");
        c->addm2TH1(sum_spill, "Gen-Gen - Gen-Gen (sube0)", "pfl");
	((overlayPad*) c->at(0,0))->rline = 0;
	((overlayPad*) c->at(0,1))->rline = 0;
        c->setXrange(0, .99);
        c->setRatioYrange(-0.2, 1.1);
        c->draw();
	c->drawHLine(0, 2);
        c->drawLegend("lin2right");
        c->save(fig_output+"/corrections_SpillOver.png");

}

void bjtc_step4_analyzer::analyze(){
	fstep2 = TFile::Open(output+"/"+step2fname+".root");
	fstep3 = TFile::Open(output+"/"+step3fname+".root");
	fstep2_uncert= TFile::Open(output+"/"+step2Uncertfname+".root");
	load_correction();	
	//correction_show();// 
	//produce_data(); // only run after you have fully corrected data
	//full_closure_test();
	//bkgError();
	//systUncert_tagBias_c5shift();
	//systUncert_tagBias();
	//systUncert_JEC();
	//systUncert_JER();
	//systUncert_decont();
	//validation_decontamination();
	//produce_data_syst();
	//systUncert_trigger("incl");
	//systUncert_trigger("tagged");
}

