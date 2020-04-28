
#include "myProcesses/jtc/plugin/workflow.h"
#include "myProcesses/jtc/plugin/Utility.h"
#include "myProcesses/jtc/plugin/bjtcSignalProducer.h"
#include "myProcesses/jtc/plugin/jtcUti.h"

class bjtc_step4_analyzer : public analyzer{
	public :
		bjtc_step4_analyzer(TString name, workflow &base0, ParaSet &ps ):analyzer(name,base0, ps){}
		void full_closure_test();
		jtcTH1Player* produce_workflow(TString, jtcTH1Player* input);
		void produce_wf001(TString, jtcTH1Player* input, jtcTH1Player*);
		void data_production();
		void validation_decontamination(); 
		void load_correction(){
			trkeff_incl = new jtcTH1Player("corrections/incl_trkEff_p1_smth_*_*", base->npt, base->ncent);
			trkeff_incl->autoLoad(base->wf);
			trkeff = new jtcTH1Player("corrections/tagged_trkEff_p1_smth_*_*", base->npt, base->ncent);
			trkeff->autoLoad(base->wf);
		//	biaseff = new jtcTH1Player("corrections/tagBias_smth_*_*", base->npt, base->ncent);
		//	biaseff->autoLoad(base->wf);
		}
		void validation(TString name);
		void link_files(TString f1, TString f2){
			TString datapath = ps->getPara<TString>("output");
			dmc = TFile::Open(datapath+"/"+f1+"_output.root");
			bmc = TFile::Open(datapath+"/"+f2+"_output.root");
		}
		void debug_plot_dr(TString savename,jtcTH1Player*j1, jtcTH1Player*j2,TString lab1="",TString lab2="");
		void debug_plot_dr_combined(TString savename,jtcTH1Player*j1, jtcTH1Player*j2,TString lab1="",TString lab2="");
		virtual void analyze() override;

		jtcTH1Player* inclusive_production(TString, jtcTH1Player*);

		TFile *bmc, *dmc, *debugf;
		jtcTH1Player *trkeff_incl, *trkeff, *biaseff;
		TString format = ".jpg";
};

void bjtc_step4_analyzer::debug_plot_dr(TString savename,jtcTH1Player*j1, jtcTH1Player*j2,TString lab1="",TString lab2=""){
	auto c =new multi_pads<overlay_pad>("c_"+savename, "", base->npt, base->ncent);
	c->setXrange(0,2.49);
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
void bjtc_step4_analyzer::debug_plot_dr_combined(TString savename,jtcTH1Player*j1, jtcTH1Player*j2,TString lab1="",TString lab2=""){
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
	auto rs = new jtcTH1Player("correlations_djetMC/tagged"+reco_tag(1,0)+"_sig_p0_dr_*_*", base->npt, base->ncent);
	auto rs0 = new jtcTH1Player("correlations_djetMC/incl"+reco_tag(1,0)+"_sig_p0_dr_*_*", base->npt, base->ncent);
	auto rsn = new jtcTH1Player("correlations_djetMC/negTag"+reco_tag(1,0)+"_sig_p0_dr_*_*", base->npt, base->ncent);
	auto ref = new jtcTH1Player("correlations_djetMC/tagTrue"+reco_tag(1,0)+"_sig_p0_dr_*_*", base->npt, base->ncent);
	rs->autoLoad(base->wf);
	rs0->autoLoad(base->wf);
	ref->autoLoad(base->wf);
	rsn->autoLoad(base->wf);


	float mistagRate[2];
	auto purity = (TH1F*)base->wf->Get("correlations_djetMC/hp");
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
	debug_plot_dr("debug_decont_incl_vs_tagTrue",rs0, ref,"incl-p0","tagTrue-p0");
}

jtcTH1Player* bjtc_step4_analyzer::inclusive_production(TString name, jtcTH1Player *rs){
	auto incl_step1 = (jtcTH1Player*) rs->clone(name+"_step1_tack");
	incl_step1->ring_corr(trkeff_incl, 2.5);
	incl_step1->bkgSub(name);
	auto c =new multi_pads<overlay_pad>("valid_incl_tracking", "", base->npt, base->ncent);
	return incl_step1;
}

void bjtc_step4_analyzer::full_closure_test(){
	auto ref = new jtcTH1Player("correlations_djetMC/trueB"+reco_tag(0,0)+"_sig_p2_*_*", base->npt, base->ncent);
	ref->autoLoad(base->wf);
	auto rs = new jtcTH1Player("correlations_djetMC/tagged"+reco_tag(1,1)+"_sig_p1_*_*", base->npt, base->ncent);
	rs->autoLoad(base->wf);
	auto pro = produce_workflow("test",rs);
	auto refdeta = ref->projX("ref_deta", -1, .99, "e");	
	auto refdr = ref->drIntegral("ref_dr");	
	auto prodeta = pro->projX("pro_deta", -1, .99, "e");
	auto prodr = pro->drIntegral("pro_dr");
	auto c =new multi_pads<overlay_pad>("clTest_dr", "", base->npt, base->ncent);
	c->setXrange(0,0.599);
	//	c->setYrange(0.5,3.);
	c->xtitle="#Delta r";
	c->ytitle="#frac{dN}{d#Deltar}";
	c->doHIarrange = 1;
	c->addm2TH1(refdr);
	c->addm2TH1(prodr);
	c->ratio_title = "corr./ref.";
	c->addLegend("upperright");
	c->labelHist("true b reference", 0);
	c->labelHist("corrected sample", 1);
	c->setRatioYrange(0, 2);
	c->draw();
	c->SaveAs(fig_output+"/fullClosure_dr_"+format);
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

void bjtc_step4_analyzer::produce_wf001(TString name, jtcTH1Player* input, jtcTH1Player *incl){	
	
	float mistagRate[2];
	auto purity = (TH1F*)base->wf->Get("correlations_djetMC/hp");
	mistagRate[0] = 1-purity->GetBinContent(1);
	mistagRate[1] = 1-purity->GetBinContent(2);
	debugf = TFile::Open("step4_debug.root", "recreate");
	debugf->cd();
	input->setName(name+"_bjetInput");
	input->write();
	incl->setName(name+"_inclInput");
	incl->write();
	auto step1 = (jtcTH1Player* ) input->clone(name+"_trkStep");	
	step1->ring_corr(trkeff , 2.5);
	auto step1_incl = (jtcTH1Player* ) incl->clone(name+"_incl_trkStep");	
	step1_incl->ring_corr(trkeff_incl , 2.5);
	step1_incl->write();
	for(int i=0; i< step1_incl->Nrow(); ++i){
		for(int j=0; j< step1_incl->Ncol(); ++j){
			step1_incl->at(i,j)->Scale(mistagRate[j]);
		}
	}
	auto step2 = (jtcTH1Player* ) step1->add2(name+"_deconStep", *step1_incl, 1, -1);
	for(int i=0; i< step1_incl->Nrow(); ++i){
		for(int j=0; j< step1_incl->Ncol(); ++j){
			step2->at(i,j)->Scale(1.0/(1.-mistagRate[j]));
		}
	}
	step2->write();
}

void bjtc_step4_analyzer::validation(TString name){
	auto step0 = new jtcTH1Player(name+"_bjetInput_*_*", base->npt, base->ncent );
	step0->autoLoad(debugf);
	auto step1 = new jtcTH1Player(name+"_trkStep_*_*", base->npt, base->ncent);
	step1->autoLoad(debugf);
	auto step2 = new jtcTH1Player(name+"_deconStep_*_*", base->npt, base->ncent);
	step2->autoLoad(debugf);

	auto ref1 = new jtcTH1Player("correlations_djetMC/tagged"+reco_tag(1,0)+"_sig_p1_*_*", base->npt, base->ncent);
	ref1->autoLoad(base->wf);
	auto ref2 = new jtcTH1Player("correlations_bjetMC/tagTrue"+reco_tag(1,0)+"_sig_p1_*_*", base->npt, base->ncent);
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

void bjtc_step4_analyzer::analyze(){
//	load_correction();	
//	full_closure_test();
//	data_production();
	//auto rs = new jtcTH1Player("correlations_djetMC/tagged"+reco_tag(1,1)+"_sig_p1_*_*", base->npt, base->ncent);
	//auto rs0 = new jtcTH1Player("correlations_djetMC/incl"+reco_tag(1,1)+"_sig_p1_*_*", base->npt, base->ncent);
	//rs->autoLoad(base->wf);
	//rs0->autoLoad(base->wf);
	validation_decontamination();
//	produce_wf001("bjetMC", rs, rs0);
//	validation("bjetMC");
//	if(debugf->IsOpen()) debugf->Close();
}

