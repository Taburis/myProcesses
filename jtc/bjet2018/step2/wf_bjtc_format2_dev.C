
#define JTC_DEBUG
#include "myProcesses/jtc/config/config_bjtc2018aa_rebin.h"
#include "myProcesses/jtc/plugin/workflow.h"
#include "myProcesses/jtc/plugin/bjtc_format2_step2_analyzer.h"
#include "myProcesses/jtc/plugin/bjtc_step0_analyzer.h"
#include "myProcesses/jtc/plugin/bjtc_step2_analyzer.h"
#include "myProcesses/jtc/plugin/bjtc_step3_analyzer.h"
#include "myProcesses/jtc/plugin/bjtc_step4_analyzer.h"


enum dbtype {data, mcsube, mcstd};

bjtc_format2_step2_analyzer* step2_format2_setup(TString name, ParaSet &ps, dbtype sim, workflow &wf, TString input){
	auto js = new bjtc_format2_step2_analyzer(name, wf, ps);
	js->doSbCorrection = 1;
	if(sim == dbtype::mcsube) js->isSube = 1;
	else js->isSube = 0;
	if(sim != dbtype::data) js->loadFile(input, 1);
	else js->loadFile(input, 0);
	//js->addSet("incl");
      	//js->addSet("tagged");
      	//js->addSet("negTag");
	//if(sim == dbtype::data) return js;
	//js->addSet("trueB");
	//js->addSet("tagTrue");
	return js;
}

void wf_bjtc_format2_dev(){

	TString name = "wf001_50mix_format2";
	TString step1_dsample_fm2_sube_input= "../data/step1/bjtc_djetMC_format2_sube_run1_JEC_fix.root";
	TString step1_dsample_fm2_std_input= "../data/step1/bjtc_djetMC_format2_std_v2_pthat50.root";

	TString step1_bsample_fm2_sube_input= "../data/step1/bjtc_bjetMC_format2_sube_v2.root";
	TString step1_bsample_fm2_std_input= "../data/step1/bjtc_bjetMC_format2_std_v2.root";

	TString step1_data_fm2_input= "../data/step1/bjtc_data_HIHardprobe_format2_v2.root";
	auto ps = config_bjtc2018aa::config_init();
	//TString eos_dir = "../data/step2";
	//TString eos_dir_fig = "./figures";
	TString eos_dir = "/eos/user/w/wangx/AN20-029";
	TString eos_dir_fig = "/eos/user/w/wangx/www/AN20-029";
	ps->setPara<TString>("output", eos_dir);
	ps->setPara<TString>("fig_output", eos_dir_fig);

	workflow wf001(name, *ps);
	wf001.doUpdate = 1;
	if(!wf001.doUpdate) system("rm -r "+eos_dir_fig+"/"+name);
//step0 --------------------------------------------------------
	//auto step0 = new bjtc_step0_analyzer("step0_check", wf001, *ps);
	//step0->jet_spectra_check2("Cent_bSample_inclusive_recoJet", "incl_RecoLevel", step1_bsample_fm2_sube_input);
	//step0->jet_spectra_check2("Cent_inclusive_recoJet", "incl_RecoLevel", step1_data_fm2_input);
	//step0->jet_spectra_check2("Cent_inclusive_recoJet", "incl_RecoLevel", step1_dsample_fm2_sube_input);
	//step0->jet_spectra_check2("Cent_inclusive_genJet", "incl_GenLevel", step1_dsample_fm2_sube_input);
	//step0->jet_spectra_check("SubeVsAllEvt_inclusive_recoJet", "incl_RecoLevel", "incl_RecoLevel", step1_dsample_fm2_sube_input, step1_dsample_fm2_std_input);
	//step0->jet_spectra_check("SubeVsAllEvt_inclusive_genJet", "incl_GenLevel", "incl_GenLevel", step1_dsample_fm2_sube_input, step1_dsample_fm2_std_input);

//step2 --------------------------------------------------------
	//auto step2_dMC_std_fm2= step2_format2_setup("correlations_djetMC_std",*ps,dbtype::mcstd, wf001, step1_dsample_fm2_std_input);
	//step2_dMC_std_fm2->do_mix_debug=1;
	//step2_dMC_std_fm2->doPurityCalculation=1;
	//step2_dMC_std_fm2->addSet("incl");
	//step2_dMC_std_fm2->addSet("tagged");
	//step2_dMC_std_fm2->addSet("tagTrue");
	//step2_dMC_std_fm2->addSet("negTag");
	//step2_dMC_std_fm2->addSet("trueB");

	//auto step2_dMC_sube_fm2= step2_format2_setup("correlations_djetMC_sube",*ps, dbtype::mcsube, wf001, step1_dsample_fm2_sube_input);
	//step2_dMC_sube_fm2->do_mix_debug=1;

	//auto step2_bMC_sube_fm2= step2_format2_setup("correlations_bjetMC_sube",*ps, dbtype::mcsube, wf001, step1_bsample_fm2_sube_input);
	//step2_bMC_sube_fm2->do_mix_debug=1;
	//step2_bMC_sube_fm2->addSet("trueB");
	//step2_bMC_sube_fm2->addSet("tagTrue");

	//auto step2_bMC_std_fm2= step2_format2_setup("correlations_bjetMC_std",*ps, dbtype::mcstd, wf001, step1_bsample_fm2_std_input);
	//step2_bMC_std_fm2->do_mix_debug=1;
	//step2_bMC_std_fm2->addSet("trueB");
	//step2_bMC_std_fm2->addSet("tagTrue");
	//step2_bMC_std_fm2->addSet("tagged");

	//auto step2_data= step2_format2_setup("correlations_HIHardProbe_jet80",*ps, dbtype::data, wf001, step1_data_fm2_input);
	//step2_data->do_mix_debug=1;
	//step2_data->addSet("tagged");
	//step2_data->addSet("negTag");
	//step2_data->addSet("incl");

	
//step3 --------------------------------------------------------
	//auto step3 = new bjtc_step3_analyzer("corrections", wf001, *ps);
	//
//step4 --------------------------------------------------------
	auto step4 = new bjtc_step4_analyzer("production", wf001, *ps);
	wf001.run();
}

