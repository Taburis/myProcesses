
#define JTC_DEBUG
//#include "myProcesses/jtc/config/config_bjtc2018aa_rebin.h"
#include "config_bjtc2018aa_newbin.h"
#include "myProcesses/jtc/plugin/workflow.h"
//#include "myProcesses/jtc/plugin/bjtc_step2_analyzer.h"
//#include "myProcesses/jtc/plugin/bjtc_step0_analyzer.h"
//#include "myProcesses/jtc/plugin/bjtc_step2_analyzer.h"
//#include "myProcesses/jtc/plugin/bjtc_step3_analyzer.h"
//#include "bjtc_signal_analyzer.h"
#include "bjtc_step2_analyzer.h"
#include "bjtc_step0_analyzer.h"
#include "bjtc_step3_analyzer.h"
#include "bjtc_step4_analyzer.h"
#include "bjtc_step5_analyzer.h"
#include "bjtc_syst_analyzer.h"
#include "bjtc_xcheck.h"


enum dbtype {data, mcsube, mcstd};

bjtc_step2_analyzer* step2_format2_setup(TString name, ParaSet &ps, dbtype sim, workflow &wf, TString input){
	auto js = new bjtc_step2_analyzer(name, wf, ps);
	js->doSbCorrection = 0;
	js->doSbSM = 1;
	if(sim == dbtype::mcsube) js->isSube = 1;
	else js->isSube = 0;
	if(sim != dbtype::data) js->loadFile(input, 1);
	else js->loadFile(input, 0);
	return js;
}

void syst(){

	TString name = "bjtc_syst";
	TString pp_reference= "../data/HIN18020/AN17337Result_new.root";
	TString step1_dsample_sube_jer_smeared="../data/correlation/syst/bjtc_MC_sube_jer_probe.root";
	TString step1_dsample_sube_jer_ref="../data/correlation/syst/bjtc_MC_sube_jer_ref.root";
	TString step1_dsample_sube_input= "../data/correlation/newbin/bjtc_djetMC_sube_c3bin.root";
	TString step1_dsample_std_input = "../data/correlation/newbin/bjtc_djetMC_std_c5sft_c3b_allSets.root";
	TString step1_dsample_std_c5shift_input = "../data/correlation/newbin/bjtc_djetMC_std_c5shift.root";
	TString step1_bsample_of_sube_input= "../data/correlation/newbin/bjtc_bjet_officialSample_c3bin.root";
	TString step1_bsample_pr_sube_gsp_c5sft = "../data/correlation/newbin/bjtc_bjetMC_PR_sube_c5sft_GSPweighted.root";
	TString step1_data_hpl_jet60 = "../data/correlation/syst/bjtc_data_hardprobelow_jet60.root";
	TString step1_data_hpl_jet80n100 = "../data/correlation/syst/bjtc_data_hardprobelow_jet80n100.root";
	TString step1_bsample_pr_sube_c5sft = "../data/correlation/newbin/bjtc_bjetMC_private_sube_c5shifted.root";
	TString step1_bsample_pr_sube_input= "../data/correlation/newbin/bjtc_bjetMC_private_sube.root";
	TString step1_bsample_fm2_std_input = "../data/correlation/bjtc_bjetMC_format2_std_nominal.root";
	TString step1_dsample_fm2_cjetWeighted= "../data/correlation/bjtc_djetMC_format2_std_cJetReweighted.root";
	TString step1_dsample_sube_c5shift= "../data/correlation/newbin/bjtc_djetMC_sube_c5shift_c3bin.root";

	TString step1_data_fm2_jet80or100_85p_input= "../data/correlation/newbin/bjtc_data_hardprobe_jet80or100_c3bin.root";
	TString step1_data_fm2_jet80or100_nominal_input= "../data/correlation/newbin/bjtc_data_hardprobe_jet80or100_c3bin_fullset.root";
	TString step1_data_fm2_jet80or100_test_input= "../data/correlation/newbin/bjtc_data_hardprobe_jet80or100_vz13.root";
	TString step1_data_fm2_jet80or100_phi_block_input= "../data/correlation/newbin/bjtc_data_hardprobe_jet80or100_block_phi0p4_1.root";
	TString step1_data_fm2_jet80or100= "../data/correlation/newbin/bjtc_data_hardprobe_jet80or100_c3bin.root";
	TString step1_data_jer_input= "../data/correlation/newbin/bjtc_data_hardprobe_jet80or100_jer_c3bin.root";
	TString step1_data_jeu_up_input= "../data/correlation/newbin/bjtc_data_hardprobe_jet80or100_jec_up_c3bin.root";
	TString step1_data_jeu_down_input= "../data/correlation/newbin/bjtc_data_hardprobe_jet80or100_jec_down_c3bin.root";
	TString step1_data_jeu_up_app= "../data/app/bjtc_data_jec_up.root";
	TString step1_data_jeu_up_77p_app= "../data/app/bjtc_data_jec_up_77p.root";
	// uncertainty part
	auto ps = config_bjtc2018aa::config_init();
	TString eos_dir = "/eos/user/w/wangx/AN20-029";
	TString eos_dir_fig = "/eos/user/w/wangx/www/AN20-029";
	ps->setPara<TString>("output", eos_dir);
	ps->setPara<TString>("fig_output", eos_dir_fig);

	workflow wf001(name, *ps);
	wf001.doUpdate = 1;
	if(!wf001.doUpdate) system("rm -r "+eos_dir_fig+"/"+name);
	TString step2fname = "bjtc_step2_output";
	TString step2uncer = "bjtc_step2_uncert";
	TString systematicOutput = "bjtc_systematic";

//nominal step2 --------------------------------------------------------
/*
	auto step2_data_jec_up= step2_format2_setup("correlations_HIHardProbe_jec_up",*ps, dbtype::data, wf001, step1_data_jeu_up_input);
	step2_data_jec_up->output_file_name = step2fname;
	step2_data_jec_up->addSet("tagged");
	step2_data_jec_up->addSet("incl");

	auto step2_data_nominal= step2_format2_setup("correlations_HIHardProbe_jec_ref",*ps, dbtype::data, wf001, step1_data_fm2_jet80or100_85p_input);
	step2_data_nominal->output_file_name = step2fname;
	step2_data_nominal->addSet("tagged");
	step2_data_nominal->addSet("incl");

	auto step2_data_jec_tag_app= step2_format2_setup("correlations_HIHardProbe_jec_up_77p",*ps, dbtype::data, wf001, step1_data_jeu_up_77p_app);
	step2_data_jec_tag_app->output_file_name = step2fname;
	step2_data_jec_tag_app->addSet("tagged");

	auto step2_data_jet80or100= step2_format2_setup("correlations_HIHardProbe_sm",*ps, dbtype::data, wf001, step1_data_fm2_jet80or100);
	step2_data_jet80or100->output_file_name = step2fname;
	step2_data_jet80or100->addSet("incl");
	step2_data_jet80or100->addSet("tagged");

	auto step2_data_jer= step2_format2_setup("correlations_HIHardProbe_jersmeared",*ps, dbtype::data, wf001, step1_data_jer_input);
	step2_data_jer->output_file_name = step2fname;
	step2_data_jer->addSet("incl");
	step2_data_jer->addSet("tagged");

	auto step2_dMC_sube_jer_ref= step2_format2_setup("correlations_djetMC_sube_jer_ref",*ps, dbtype::mcsube, wf001, step1_dsample_sube_jer_ref);
    step2_dMC_sube_jer_ref->output_file_name = step2fname;
    step2_dMC_sube_jer_ref->addSet("trueB");
    step2_dMC_sube_jer_ref->addSet("incl");

	auto step2_dMC_sube_jer_smeared= step2_format2_setup("correlations_djetMC_sube_jersmeared",*ps, dbtype::mcsube, wf001, step1_dsample_sube_jer_smeared);
    step2_dMC_sube_jer_smeared->output_file_name = step2fname;
    step2_dMC_sube_jer_smeared->addSet("trueB");
    step2_dMC_sube_jer_smeared->addSet("incl");
	auto step2_data_jet60= step2_format2_setup("correlations_data_syst_jet60",*ps, dbtype::data, wf001, step1_data_hpl_jet60);
    step2_data_jet60->output_file_name = step2fname;
    step2_data_jet60->addSet("tagged");
    step2_data_jet60->addSet("incl");

	auto step2_data_jet80n100= step2_format2_setup("correlations_data_syst_jet80n100",*ps, dbtype::data, wf001, step1_data_hpl_jet80n100);
    step2_data_jet80n100->output_file_name = step2fname;
    step2_data_jet80n100->addSet("tagged");
    step2_data_jet80n100->addSet("incl");

	auto step2_dMC_std= step2_format2_setup("correlations_djetMC_std_c5sft",*ps,dbtype::mcstd, wf001, step1_dsample_std_input);
    step2_dMC_std->output_file_name = step2fname;
    step2_dMC_std->doPurityCalculation=1;
    step2_dMC_std->addSet("incl");
    step2_dMC_std->addSet("tagged");
    step2_dMC_std->addSet("tagTrue");
    step2_dMC_std->addSet("trueB");


	auto step2_bMC_pr_sube= step2_format2_setup("correlations_bjetMC_pr_sube_c5sft",*ps, dbtype::mcsube, wf001, step1_bsample_pr_sube_c5sft);
    step2_bMC_pr_sube->output_file_name = step2fname;
    step2_bMC_pr_sube->addSet("tagTrue");
    step2_bMC_pr_sube->addSet("trueB");
	auto step2_bMC_pr_sube_gsp= step2_format2_setup("correlations_bjetMC_pr_sube_gsp_c5sft",*ps, dbtype::mcsube, wf001, step1_bsample_pr_sube_gsp_c5sft);
    step2_bMC_pr_sube_gsp->output_file_name = step2fname;
    step2_bMC_pr_sube_gsp->addSet("tagTrue");
    step2_bMC_pr_sube_gsp->addSet("trueB");
*/

//    auto step2_dMC_sube_fm2= step2_format2_setup("correlations_djetMC_sube",*ps, dbtype::mcsube, wf001, step1_dsample_fm2_sube_input);
//    step2_dMC_sube_fm2->output_file_name = step2fname;
//    step2_dMC_sube_fm2->addSet("incl");
//    step2_dMC_sube_fm2->addSet("trueB");
//    step2_dMC_sube_fm2->addSet("tagged");
//
//	auto step2_bMC_sube_fm2= step2_format2_setup("correlations_bjetMC_sube",*ps, dbtype::mcsube, wf001, step1_bsample_of_sube_input);
//    step2_bMC_sube_fm2->output_file_name = step2fname;
//    step2_bMC_sube_fm2->addSet("trueB");
//    step2_bMC_sube_fm2->addSet("tagTrue");
//    auto step2_bMC_std_fm2= step2_format2_setup("correlations_bjetMC_std",*ps, dbtype::mcstd, wf001, step1_bsample_fm2_std_input);
//    step2_bMC_std_fm2->output_file_name = step2fname;
//    step2_bMC_std_fm2->addSet("trueB");
//    step2_bMC_std_fm2->addSet("tagTrue");
//    step2_bMC_std_fm2->addSet("tagged");

/*
*/
//step3 --------------------------------------------------------
/*
	auto step3 = new bjtc_step3_analyzer("corrections", wf001, *ps);
	step3->step2fname = step2fname;
	step3->step2uncert = step2uncer;
	step3->systematic = systematicOutput;
	step3->format = ".png";
	step3->output_file_name = "bjtc_step3_output";
*/
	auto steps = new bjtc_syst_analyzer("systUncert", wf001, *ps);
	steps->step2Uncertfname = step2uncer;
	steps->step2fname=step2fname;
	steps->output_file_name = "bjtc_syst_output";

//step4 --------------------------------------------------------
/*
	auto step4 = new bjtc_step4_analyzer("production", wf001, *ps);
	step4->step2fname=eos_dir+"/"+name+"/"+step2fname;
	step4->step2Uncertfname = step2uncer;
	step4->step3fname="bjtc_step3_output";
	step4->output = ".";
	step4->output_file_name = "step4_output_sm";

	auto step_check = new bjtc_xcheck("xcheck", wf001, *ps);
	step_check->step2fname=step2fname;
	step_check->step2Uncertfname = step2uncer;
	//step_check->step3fname="bjtc_step3_output";
	step_check->step4fname="step4_output_sm";

	auto step5 = new bjtc_step5_analyzer("Results", wf001, *ps);
	step5->output = ".";
	step5->pprefer_path=pp_reference;
*/
	//step5->systFilePath="/eos/user/w/wangx/AN20-029/bjtc_c2bin_50mix_newbin/bjtc_syst_output.root";
	//step5->fig_output += "/dev";
/*
*/
	wf001.run();
}
