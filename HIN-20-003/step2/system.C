
#define JTC_DEBUG
#include "myProcesses/jtc/config/config_bjtc2018aa_rebin.h"
#include "myProcesses/jtc/plugin/workflow.h"
#include "bjtc_systematic.h"
#include "myProcesses/jtc/plugin/bjtc_format2_step2_analyzer.h"
//#include "myProcesses/jtc/plugin/bjtc_step0_analyzer.h"
//#include "myProcesses/jtc/plugin/bjtc_step2_analyzer.h"
//#include "myProcesses/jtc/plugin/bjtc_step3_analyzer.h"
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

void system(){

	//TString name = "wf001_50mix_format2";
	TString name = "bjtc_c2bin_50mix_systematic";
	TString pp_reference= "../data/HIN18020/AN17337Result_new.root";
	TString step1_dsample_fm2_sube_input= "../data/correlation/bjtc_djetMC_format2_sube_nominal.root";
	TString step1_dsample_fm2_std_input = "../data/correlation/bjtc_djetMC_format2_std_nominal.root";

	TString step1_bsample_fm2_sube_input= "../data/correlation/bjtc_bjetMC_format2_sube_nominal.root";
	TString step1_bsample_fm2_std_input = "../data/correlation/bjtc_bjetMC_format2_std_nominal.root";
	TString step1_bsample_fm2_gsp= "../data/correlation/bjtc_bjetMC_format2_sube_gspWeighted.root";
	TString step1_dsample_fm2_cjetWeighted= "../data/correlation/bjtc_djetMC_format2_std_cJetReweighted.root";

	TString step1_data_fm2_jet80_input= "../data/step1/bjtc_data_HIHardprobe_format2_v2.root";
	TString step1_data_fm2_input= "../data/correlation/bjtc_data_hardprobe_jet80or100.root";
	TString step1_data_fm2_trigger80and100_input= "../data/correlation/bjtc_data_hardprobe_merge80and100_p85.root";
	TString step1_data_fm2_hardprobeLower_jet60= "../data/correlation/syst/jtc_data_hardprobelow_jet60.root";
	TString step1_data_fm2_hardprobeLower_jet80= "../data/correlation/syst/jtc_data_hardprobelow_jet80above.root";
	// uncertainty part
	TString step1_data_fm2_JECU_up_input = "../data/correlation/syst/bjtc_data_hardprobe_merge80and100_JEUUp.root";
	TString step1_data_fm2_JECU_down_input= "../data/correlation/syst/bjtc_data_hardprobe_merge80and100_JEUDown.root";
	TString step1_data_fm2_JECU_ref_input= "../data/correlation/syst/bjtc_data_hardprobe_merge80and100_JEUref.root";
	auto ps = config_bjtc2018aa::config_init();
	//TString eos_dir = "../data/step2";
	//TString eos_dir_fig = "./figures";
	TString step2nominal ="/eos/user/w/wangx/AN20-029/bjtc_c2bin_50mix_nomianl/bjtc_step2_output.root";
	TString eos_dir = "/eos/user/w/wangx/AN20-029";
	TString eos_dir_fig = "/eos/user/w/wangx/www/AN20-029";
	ps->setPara<TString>("output", eos_dir);
	ps->setPara<TString>("fig_output", eos_dir_fig);

	workflow wf001(name, *ps);
	wf001.doUpdate = 1;
	if(!wf001.doUpdate) system("rm -r "+eos_dir_fig+"/"+name);
	TString step2fname = "bjtc_step2_output";
	TString systematicOutput = "bjtc_systematic";
//step0 -------------------------------------------------------
//	auto step0 = new bjtc_step0_analyzer("step0_check", wf001, *ps);
//	step0 -> MCweight_check("djetMC","data", step1_dsample_fm2_std_input,step1_data_fm2_input);
	//step0 -> MCweight_check("bjetMC","data", step1_bsample_fm2_std_input,step1_data_fm2_input);
	//step0 -> pullCentWeight("djetMC", step1_dsample_fm2_std_input,step1_data_fm2_input);
	//step0->jet_spectra_check2("Cent_bSample_inclusive_recoJet", "incl_RecoLevel", step1_bsample_fm2_sube_input);
	//step0->jet_spectra_check2("Cent_inclusive_recoJet", "incl_RecoLevel", step1_data_fm2_input);
	//step0->jet_spectra_check2("Cent_inclusive_recoJet", "incl_RecoLevel", step1_bsample_fm2_sube_input);
	//step0->jet_spectra_check2("Cent_inclusive_recoJet", "incl_RecoLevel", step1_dsample_fm2_sube_input);
	//step0->jet_spectra_check2("Cent_inclusive_genJet", "incl_GenLevel", step1_dsample_fm2_sube_input);
	//step0->jet_spectra_check("SubeVsAllEvt_inclusive_recoJet", "incl_RecoLevel", "incl_RecoLevel", step1_dsample_fm2_sube_input, step1_dsample_fm2_std_input);
	//step0->jet_spectra_check("SubeVsAllEvt_inclusive_genJet", "incl_GenLevel", "incl_GenLevel", step1_dsample_fm2_sube_input, step1_dsample_fm2_std_input);

//nominal step2 --------------------------------------------------------
/*
	auto step2_dMC_std_fm2= step2_format2_setup("correlations_djetMC_std",*ps,dbtype::mcstd, wf001, step1_dsample_fm2_std_input);
	step2_dMC_std_fm2->output_file_name = step2fname;
	step2_dMC_std_fm2->doPurityCalculation=1;
	step2_dMC_std_fm2->addSet("incl");
	step2_dMC_std_fm2->addSet("tagged");
	step2_dMC_std_fm2->addSet("tagTrue");
	step2_dMC_std_fm2->addSet("negTag");
	step2_dMC_std_fm2->addSet("trueB");
	step2_dMC_std_fm2->addSet("cont");
	auto step2_dMC_sube_fm2= step2_format2_setup("correlations_djetMC_sube",*ps, dbtype::mcsube, wf001, step1_dsample_fm2_sube_input);
	step2_dMC_sube_fm2->output_file_name = step2fname;
	step2_dMC_sube_fm2->addSet("incl");
	step2_dMC_sube_fm2->addSet("trueB");
	step2_dMC_sube_fm2->addSet("tagged");
//	step2_dMC_sube_fm2->addSet("cont");

	auto step2_bMC_sube_fm2= step2_format2_setup("correlations_bjetMC_sube",*ps, dbtype::mcsube, wf001, step1_bsample_fm2_sube_input);
	step2_bMC_sube_fm2->output_file_name = step2fname;
	step2_bMC_sube_fm2->addSet("trueB");
	step2_bMC_sube_fm2->addSet("tagTrue");
	auto step2_data_jet60= step2_format2_setup("correlations_HIHardProbeLower_jet60",*ps, dbtype::data, wf001, step1_data_fm2_hardprobeLower_jet60);
	step2_data_jet60->output_file_name = step2fname;
	step2_data_jet60->addSet("tagged");
	step2_data_jet60->addSet("incl");

	auto step2_data_jet80= step2_format2_setup("correlations_HIHardProbeLower_jet80and100",*ps, dbtype::data, wf001, step1_data_fm2_hardprobeLower_jet80);
	step2_data_jet80->output_file_name = step2fname;
	step2_data_jet80->addSet("tagged");
	step2_data_jet80->addSet("incl");
//JEC uncertainties
	auto step2_data_jecUp= step2_format2_setup("correlations_HIHardProbe_jecUp",*ps, dbtype::data, wf001, step1_data_fm2_JECU_up_input);
	step2_data_jecUp->output_file_name = step2fname;
	step2_data_jecUp->addSet("tagged");
	step2_data_jecUp->addSet("incl");
	auto step2_data_jecDown= step2_format2_setup("correlations_HIHardProbe_jecDown",*ps, dbtype::data, wf001, step1_data_fm2_JECU_down_input);
	step2_data_jecDown->output_file_name = step2fname;
	step2_data_jecDown->addSet("tagged");
	step2_data_jecDown->addSet("incl");
	auto step2_data_jecRef= step2_format2_setup("correlations_HIHardProbe_jecRef",*ps, dbtype::data, wf001, step1_data_fm2_JECU_ref_input);
	step2_data_jecRef->output_file_name = step2fname;
	step2_data_jecRef->addSet("tagged");
	step2_data_jecRef->addSet("incl");
*/

//systematic--------------------------------------------------------------------------
	auto step_sys = new bjtc_systematic("systematic", wf001, *ps);
	step_sys->step2fname = step2fname;
	step_sys->systematic = systematicOutput;
	step_sys->format = ".png";
	step_sys->output_file_name = "bjtc_systematic";
	step_sys->step2nominal = step2nominal;

	wf001.run();
}
