
#include "myProcesses/jtc/config/config_bjtc2018aa_rebin.h"
#include "myProcesses/jtc/plugin/workflow.h"
#include "myProcesses/jtc/plugin/bjtc_format2_step2_analyzer.h"
#include "myProcesses/jtc/plugin/bjtc_step2_analyzer.h"
#include "myProcesses/jtc/plugin/bjtc_step3_analyzer.h"
//#include "myProcesses/jtc/plugin/bjtc_step4_analyzer.h"

bjtc_step2_analyzer* step2_setup(TString name, ParaSet &ps,workflow &wf, bool ismc, TString input){
	auto js = new bjtc_step2_analyzer(name, wf, ps);
	js->dorebin = 1;
	js->doSbCorrection = 1;
	js->loadFile(input, ismc);
	js->addSet("incl");
      	js->addSet("tagged");
	if(ismc){
		js->addSet("trueB");
		js->addSet("tagTrue");
	}
	return js;
};
bjtc_format2_step2_analyzer* step2_format2_setup(TString name, ParaSet &ps,workflow &wf, bool ismc, TString input){
	auto js = new bjtc_format2_step2_analyzer(name, wf, ps);
	js->doSbCorrection = 1;
	js->loadFile(input, ismc);
	js->addSet("incl");
//      	js->addSet("tagged");
//      	js->addSet("negTag");
//	if(ismc){
//		js->addSet("trueB");
//		js->addSet("tagTrue");
//	}
	return js;
}

bjtc_step2_analyzer* step2_neg_setup(TString name, ParaSet &ps,workflow &wf, TString input){
	auto js = new bjtc_step2_analyzer(name, wf, ps);
	js->dorebin = 1;
	js->doSbCorrection = 1;
	js->loadFile(input, 1);
	js->addSet("negTag");
	return js;
}

bjtc_step2_analyzer* step2_sube_setup(TString name, ParaSet &ps,workflow &wf, TString input, TString mix_input){
	auto js = new bjtc_step2_analyzer(name, wf, ps);
	js->dorebin = 1;
	js->dosube=1;
	js->doSbCorrection = 1;
	js->loadFile(input,mix_input, 1);
	//js->addSet("negTag");
	js->addSet("incl");
	js->addSet("tagged");
	js->addSet("trueB");
	js->addSet("tagTrue");
	return js;
}

bjtc_step2_analyzer* step2_sube_neg_setup(TString name, ParaSet &ps,workflow &wf, TString input, TString mix_input){
	auto js = new bjtc_step2_analyzer(name, wf, ps);
	js->dosube=1;
	js->dorebin = 1;
	js->doSbCorrection = 1;
	js->loadFile(input, mix_input, 1);
	js->addSet("negTag");
	return js;
}

void wf_bjtc_format2_dev(){

	TString name = "wf001_50mix_format2";
	TString step1_dsample_fm2_input= "../data/step1/bjtc_djetMC_format2_run1.root";
	TString step1_bsample_input= "../data/step1/bjtc_bjetMC_50mix_standard_run1.root";
	TString step1_dsample_input= "../data/step1/bjtc_djetMC_standard_run1.root";
	TString step1_dsample_sube_input= "../data/step1/bjtc_djetMC_sube_withNeg_run1.root";
	TString step1_dsample_neg_input= "../data/step1/bjtc_djetMC_neg_run1.root";
	TString step1_data_input= "../data/step1/bjtc_data_jussi_hiForest_run0.root";
	auto ps = config_bjtc2018aa::config_init();
	//TString eos_dir = "../data/step2";
	//TString eos_dir_fig = "./figures";
	TString eos_dir = "/eos/user/w/wangx/AN20-029";
	TString eos_dir_fig = "/eos/user/w/wangx/www/AN20-029";
	ps->setPara<TString>("output", eos_dir);
	ps->setPara<TString>("fig_output", eos_dir_fig);

	workflow wf001(name, *ps);
	wf001.doUpdate = 1;

	auto step2_dMC_fm2= step2_format2_setup("correlations_djetMC",*ps, wf001, 1, step1_dsample_fm2_input);
	step2_dMC_fm2->do_mix_debug=1;
	//auto step2_dMC_neg= step2_neg_setup("correlations_djetMC",*ps, wf001, step1_dsample_neg_input);
	//step2_dMC_neg->do_mix_debug=1;
	//auto step2_dMC_sube= step2_sube_setup("correlations_djetMC",*ps, wf001, step1_dsample_sube_input, step1_dsample_input);
	//step2_dMC_sube->do_mix_debug=1;
	//auto step2_dMC_sube_neg= step2_sube_neg_setup("correlations_djetMC",*ps, wf001, step1_dsample_sube_input, step1_dsample_neg_input);
	//step2_dMC_sube_neg->do_mix_debug=1;

	//auto step2_dMC = step2_setup("correlations_djetMC",*ps, wf001, 1, step1_dsample_input);
	//step2_dMC->do_mix_debug=1;
	//step2_dMC->doPurityCalculation=1;

	//auto step2_bMC = step2_setup("correlations_bjetMC",*ps, wf001, 1, step1_bsample_input);
	//step2_bMC->do_mix_debug=1;

	//auto step2_data = step2_setup("correlations_data_nominal",*ps, wf001, 0, step1_data_input);
	//step2_data->do_mix_debug=1;
	//
	//auto step3 = new bjtc_step3_analyzer("corrections", wf001, *ps);
	
	//auto step4 = new bjtc_step4_analyzer("production", wf001, *ps);
	//step4->link_files("wf001_dijetSample_50mix_skim1/wf001_dijetSample_50mix_run1", "wf001_bjetSample_50mix_run1/wf001_bjetSample_50mix_run1");
	
	wf001.run();
}

