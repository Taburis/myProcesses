
#define JTC_DEBUG
#include "myProcesses/jtc/config/config_bjtc2018aa_rebin.h"
#include "myProcesses/jtc/plugin/workflow.h"
#include "myProcesses/jtc/plugin/bjtc_format2_step2_analyzer.h"
#include "myProcesses/jtc/plugin/bjtc_step0_analyzer.h"
#include "myProcesses/jtc/plugin/bjtc_step2_analyzer.h"
#include "myProcesses/jtc/plugin/bjtc_step3_analyzer.h"
#include "myProcesses/jtc/plugin/bjtc_step4_analyzer.h"
#include "myProcesses/jtc/plugin/bjtc_step5_analyzer.h"


enum dbtype {data, mcsube, mcstd};

bjtc_format2_step2_analyzer* step2_format2_setup(TString name, ParaSet &ps, dbtype sim, workflow &wf, TString input){
	auto js = new bjtc_format2_step2_analyzer(name, wf, ps);
	js->doSbCorrection = 1;
	if(sim == dbtype::mcsube) js->isSube = 1;
	else js->isSube = 0;
	if(sim != dbtype::data) js->loadFile(input, 1);
	else js->loadFile(input, 0);
	return js;
}

void wf001_bjtc_c2bin_cwfixed(){

	//TString name = "wf001_50mix_format2";
	TString name = "bjtc_c2bin_50mix_wf001_cwfix";
	TString pp_reference= "../data/AN17337/AN17337Result_new.root";
	TString step1_dsample_fm2_sube_input= "../data/step1/bjtc_djetMC_format2_sube_c5shift_cwfix.root";
	TString step1_dsample_fm2_std_input= "../data/step1/bjtc_djetMC_format2_std_c5shift_cwfix.root";

	TString step1_bsample_fm2_sube_input= "../data/step1/bjtc_bjetMC_format2_sube_c5shift_cwfix.root";
	TString step1_bsample_fm2_std_input= "../data/step1/bjtc_bjetMC_format2_std_c5shift_cwfix.root";

	//TString step1_data_fm2_input= "../data/step1/bjtc_data_HIHardprobe_format2_v2.root";
	TString step1_data_fm2_input= "../data/step1/bjtc_data_HIHardprobe_format2_jet80ro100.root";
	TString step1_data_fm2_JECU_up_input  = "../data/step1/systUncert/bjtc_data_HIHardprobe_format2_jet80ro100_JEU_up.root";
	TString step1_data_fm2_JECU_down_input= "../data/step1/systUncert/bjtc_data_HIHardprobe_format2_jet80ro100_JEU_down.root";
	//TString step1_data_fm2_input= "../data/step1/bjtc_data_HIHardprobe_format2.root";
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
	TString step2fname = "bjtc_step2_output";
	TString step2uncer = "bjtc_step2_uncert";
//step0 -------------------------------------------------------
	auto step0 = new bjtc_step0_analyzer("step0_check", wf001, *ps);
	//step0 -> MCweight_check("djetMC","data", step1_dsample_fm2_std_input,step1_data_fm2_input);
	//step0 -> MCweight_check("bjetMC","data", step1_bsample_fm2_std_input,step1_data_fm2_input);
	//step0 -> pullCentWeight("djetMC", step1_dsample_fm2_std_input,step1_data_fm2_input);
	//step0->jet_spectra_check2("Cent_bSample_inclusive_recoJet", "incl_RecoLevel", step1_bsample_fm2_sube_input);
	//step0->jet_spectra_check2("Cent_inclusive_recoJet", "incl_RecoLevel", step1_data_fm2_input);
	//step0->jet_spectra_check2("Cent_inclusive_recoJet", "incl_RecoLevel", step1_dsample_fm2_sube_input);
	//step0->jet_spectra_check2("Cent_inclusive_genJet", "incl_GenLevel", step1_dsample_fm2_sube_input);
	//step0->jet_spectra_check("SubeVsAllEvt_inclusive_recoJet", "incl_RecoLevel", "incl_RecoLevel", step1_dsample_fm2_sube_input, step1_dsample_fm2_std_input);
	//step0->jet_spectra_check("SubeVsAllEvt_inclusive_genJet", "incl_GenLevel", "incl_GenLevel", step1_dsample_fm2_sube_input, step1_dsample_fm2_std_input);

//step2 --------------------------------------------------------
/*	
	auto step2_dMC_std_fm2= step2_format2_setup("correlations_djetMC_std",*ps,dbtype::mcstd, wf001, step1_dsample_fm2_std_input);
	step2_dMC_std_fm2->output_file_name = step2fname;
	step2_dMC_std_fm2->do_mix_debug=1;
	step2_dMC_std_fm2->doPurityCalculation=1;
	step2_dMC_std_fm2->addSet("incl");
	step2_dMC_std_fm2->addSet("tagged");
	step2_dMC_std_fm2->addSet("tagTrue");
	step2_dMC_std_fm2->addSet("negTag");
	step2_dMC_std_fm2->addSet("trueB");

	auto step2_dMC_sube_fm2= step2_format2_setup("correlations_djetMC_sube",*ps, dbtype::mcsube, wf001, step1_dsample_fm2_sube_input);
	step2_dMC_sube_fm2->do_mix_debug=1;
	step2_dMC_sube_fm2->output_file_name = step2fname;
	step2_dMC_sube_fm2->addSet("incl");

	auto step2_bMC_sube_fm2= step2_format2_setup("correlations_bjetMC_sube",*ps, dbtype::mcsube, wf001, step1_bsample_fm2_sube_input);
	step2_bMC_sube_fm2->output_file_name = step2fname;
	step2_bMC_sube_fm2->do_mix_debug=1;
	step2_bMC_sube_fm2->addSet("trueB");
	step2_bMC_sube_fm2->addSet("tagTrue");

	auto step2_bMC_std_fm2= step2_format2_setup("correlations_bjetMC_std",*ps, dbtype::mcstd, wf001, step1_bsample_fm2_std_input);
	step2_bMC_std_fm2->output_file_name = step2fname;
	step2_bMC_std_fm2->do_mix_debug=1;
	step2_bMC_std_fm2->addSet("trueB");
	step2_bMC_std_fm2->addSet("tagTrue");
	step2_bMC_std_fm2->addSet("tagged");

	auto step2_data= step2_format2_setup("correlations_HIHardProbe_jet80",*ps, dbtype::data, wf001, step1_data_fm2_input);
	step2_data->do_mix_debug=1;
	step2_data->output_file_name = step2fname;
	step2_data->addSet("tagged");
	step2_data->addSet("negTag");
	step2_data->addSet("incl");

	auto step2_data_JECU_up= step2_format2_setup("correlations_HIHardProbe_jet80or100_JECU_up",*ps, dbtype::data, wf001, step1_data_fm2_JECU_up_input);
	step2_data_JECU_up->do_mix_debug=1;
	step2_data_JECU_up->output_file_name = step2uncer;
	step2_data_JECU_up->addSet("tagged");
	step2_data_JECU_up->addSet("negTag");

	auto step2_data_JECU_down= step2_format2_setup("correlations_HIHardProbe_jet80or100_JECU_down",*ps, dbtype::data, wf001, step1_data_fm2_JECU_down_input);
	step2_data_JECU_down->do_mix_debug=1;
	step2_data_JECU_down->output_file_name = step2uncer;
	step2_data_JECU_down->addSet("tagged");
	step2_data_JECU_down->addSet("negTag");
*/
//step3 --------------------------------------------------------
//	auto step3 = new bjtc_step3_analyzer("corrections", wf001, *ps);
//	step3->step2fname = step2fname;
//	step3->format = ".png";
//	step3->output_file_name = "bjtc_step3_output";
//step4 --------------------------------------------------------
	auto step4 = new bjtc_step4_analyzer("production", wf001, *ps);
	step4->step2fname=step2fname;
	step4->step2Uncertfname = step2uncer;
	step4->step3fname="bjtc_step3_output";
	step4->output_file_name = "bjtc_step4_output";

//	auto step5 = new bjtc_step5_analyzer("Results", wf001, *ps);
//	step5->pprefer_path=pp_reference;
	wf001.run();
}
