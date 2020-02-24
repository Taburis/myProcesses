

#include "myProcesses/jtc/config/config_bjtc2018aa.h"
#include "myProcesses/jtc/plugin/workflow.h"
#include "myProcesses/jtc/plugin/bjtc_step2_analyzer.h"
#include "myProcesses/jtc/plugin/bjtc_step3_analyzer.h"

bjtc_step2_analyzer* step2_setup(TString name, ParaSet &ps,workflow &wf, bool ismc, TString input){
	auto js = new bjtc_step2_analyzer(name, wf, ps);
	js->loadFile(input, ismc);
	js->addSet("incl");
	//js->addSet("trueB");
	//js->addSet("tagged");
	//js->addSet("tagTrue");
	return js;
};

void wf_bjtc_dev(){
	TString step1_dijetmc= "../data/step1/bjtc_dijetMC_20per.root";
	auto ps = config_bjtc2018aa::config_init();
	TString eos_dir = "/eos/user/w/wangx/AN20-029";
	TString eos_dir_fig = "/eos/user/w/wangx/www/AN20-029";
	ps->setPara<TString>("output", eos_dir);
	ps->setPara<TString>("fig_output", eos_dir_fig);
	workflow wf001("wf001_dijetSample", *ps);
	auto dijet_step2 = step2_setup("correlations",*ps, wf001, 1, step1_dijetmc);
	auto dijet_step3 = new bjtc_step3_analyzer("corrections", wf001, *ps);
	wf001.run();
}
