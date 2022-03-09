
#define JTC_DEBUG
//#include "myProcesses/jtc/config/config_bjtc2018aa_rebin.h"
#include "config_bjtc2018aa_newbin.h"
#include "myProcesses/jtc/plugin/workflow.h"
//#include "myProcesses/jtc/plugin/bjtc_step0_analyzer.h"
//#include "myProcesses/jtc/plugin/bjtc_step2_analyzer.h"
//#include "myProcesses/jtc/plugin/bjtc_step3_analyzer.h"
//#include "bjtc_signal_analyzer.h"
#include "bjtc_step0_analyzer.h"
#include "bjtc_step2_analyzer.h"
#include "bjtc_step3_analyzer.h"
#include "bjtc_step4_analyzer.h"
#include "bjtc_step5_analyzer.h"
#include "bjtc_syst_analyzer.h"
#include "bjtc_xcheck.h"


enum dbtype {data, mcsube, mcstd};

bjtc_step2_analyzer* step2_format2_setup(TString name, ParaSet &ps, dbtype sim, workflow &wf, TString input){
	auto js = new bjtc_step2_analyzer(name, wf, ps);
	js->doSbCorrection = 1;
	if(sim == dbtype::mcsube) js->isSube = 1;
	else js->isSube = 0;
	if(sim != dbtype::data) js->loadFile(input, 1);
	else js->loadFile(input, 0);
	return js;
}

void escheme_probe(){

	//TString name = "wf001_50mix_format2";
	//TString name = "bjtc_c2bin_50mix_sb_1p4_1p8";
	//TString name = "bjtc_c2bin_50mix_sb_1p6_2p2_smth";
	//TString name = "bjtc_c2bin_50mix_sb_1p8_2p2_fineBin_seagullCorr";
	TString name = "bjtc_sb_1p6_2p2_fineBin_EScheme";
	//TString name = "bjtc_sb_1p6_2p2_fineBin";
	//TString name = "bjtc_sb_1p6_2p2";
	//TString name = "bjtc_c2bin_50mix_sb_1p6_2p2_nosbcorrection";
	//TString name = "bjtc_c2bin_50mix_sb_1p6_2p2_fineBin_nosbcorrection";
	//TString name = "bjtc_c2bin_50mix_sb_1p6_2p2_fineBin";
	//TString name = "bjtc_c2bin_50mix_sb_1p6_2p2_smth_mixingSmooth";
	TString pp_reference= "../data/HIN18020/AN17337Result_new.root";
	TString step1_dsample_std_c5shift_input = "../data/correlation/newbin/bjtc_djetMC_std_c5shift_c3bin_EScheme.root";
	TString step1_dsample_sube_c5shift_input= "../data/correlation/newbin/bjtc_djetMC_sube_c5shift_c3bin_EScheme.root";

	TString step1_data_fm2_jet80or100_nominal_input= "../data/correlation/newbin/bjtc_data_hardprobe_jet80or100_c3bin_EScheme.root";
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
	//TString step2fname = "bjtc_step2_output";
	TString step2fname = "bjtc_step2_output";
	TString step2uncer = "bjtc_step2_uncert";
	//TString step2fname = "bjtc_step2_output_newbin";
	//TString step2uncer = "bjtc_step2_uncert_newbin";
	TString systematicOutput = "bjtc_systematic";

	auto step0 = new bjtc_step0_analyzer("eventCheck", wf001, *ps);
	//step0->MCweight_check("djet-sube", "data", step1_dsample_sube_c5shift_input, step1_data_fm2_jet80or100_nominal_input, 1);
	//step0->MCweight_check("bjet-sube", "data", step1_bsample_sube_c5shift, step1_data_fm2_jet80or100_nominal_input, 1);
	//step0->MCweight_check("djet-sube", "bjet-sube", step1_dsample_sube_c5shift_input, step1_bsample_sube_c5shift);
	//step0->MCweight_check("djet-sube", "bjet-sube", step1_dsample_sube_c5shift_input, step1_bsample_pr_sube_c5shift_input);
	//step0->MCweight_check("djet-std", "djet-sube", step1_dsample_std_c5shift_input, step1_dsample_sube_c5shift_input);
	//step0->MCweight_check("djet-std", "data", step1_dsample_std_c5shift_input, step1_data_fm2_jet80or100_nominal_input, 1);
	//step0->jet_spectra_check("xcheck_jet_spectra", "nominal", "block", step1_data_fm2_jet80or100_nominal_input, step1_data_fm2_jet80or100_phi_block_input);
	//step0->jet_spectra_check("xcheck_jet_spectra2", "nominal", "80%", step1_data_fm2_jet80or100_nominal_input, step1_data_fm2_jet80or100_85p_input);
	
	auto step2_data_jet80or100= step2_format2_setup("correlations_HIHardProbe_nominal",*ps, dbtype::data, wf001, step1_data_fm2_jet80or100_nominal_input);
	step2_data_jet80or100->output_file_name = step2fname;
	step2_data_jet80or100->do_mix_debug=0;
	step2_data_jet80or100->addSet("incl");
//	step2_data_jet80or100->addSet("tagged");
	auto step2_dMC_std_c5shift= step2_format2_setup("correlations_djetMC_std_c5shift",*ps,dbtype::mcstd, wf001, step1_dsample_std_c5shift_input);
	step2_dMC_std_c5shift->output_file_name = step2fname;
	step2_dMC_std_c5shift->do_mix_debug=0;
	step2_dMC_std_c5shift->addSet("incl");

	auto step2_djetMC_sube_c5shift= step2_format2_setup("correlations_djetMC_sube_c5shift",*ps, dbtype::mcsube, wf001, step1_dsample_sube_c5shift_input);
	step2_djetMC_sube_c5shift->do_mix_debug=0;
	step2_djetMC_sube_c5shift->output_file_name = step2fname;
	step2_djetMC_sube_c5shift->addSet("incl");
/*	
*/
//step3 --------------------------------------------------------
	auto step3 = new bjtc_step3_analyzer("corrections", wf001, *ps);
	step3->step2fname = step2fname;
	step3->step2uncert = step2uncer;
	step3->systematic = systematicOutput;
	step3->format = ".png";
	step3->output_file_name = "bjtc_step3_output";
//step4 --------------------------------------------------------
	auto step4 = new bjtc_step4_analyzer("production", wf001, *ps);
	step4->step2fname=step2fname;
	step4->step2Uncertfname = step2uncer;
	step4->step3fname="bjtc_step3_output";
	step4->output_file_name = "bjtc_step4_output";

	auto step_check = new bjtc_xcheck("xcheck", wf001, *ps);
	step_check->step2fname=step2fname;
	step_check->step2Uncertfname = step2uncer;
	step_check->step3fname="bjtc_step3_output";
	step_check->step4fname="bjtc_step4_output";
/*
*/
/*
	auto step5 = new bjtc_step5_analyzer("Results", wf001, *ps);
	step5->pprefer_path=pp_reference;
	step5->systFilePath="/eos/user/w/wangx/AN20-029/bjtc_c2bin_50mix_newbin/bjtc_syst_output.root";
//	auto steps = new bjtc_syst_analyzer("systUncert", wf001, *ps);
//	steps->step2Uncertfname = step2uncer;
//	steps->step2fname=step2fname;
//	steps->output_file_name = "bjtc_syst_output";
*/
/*
*/
	//step5->fig_output += "/dev";
	wf001.run();
}
