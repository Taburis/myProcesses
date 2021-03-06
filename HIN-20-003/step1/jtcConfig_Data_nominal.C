
#define event_content_skim
#include "myProcesses/HIN-20-003/config/cfg_newbin.h"
//#include "myProcesses/HIN-20-003/config/cfg.h"
#include "myProcesses/jtc/plugin/jtcUti.h"
#include "producerBJTC.h"

using namespace config_AN20029;

void jtcConfig_Data_nominal(bool doCrab = 0, int jobID=0){


	using pset = pset_nominalHI_skim;
	using src  = selections;
	using weight  = weight_data_nominal;

	
	using config = configBase<pset, src, weight>;
	config cfg;
	cfg.ps->isMC = 0;
	cfg.ps->isHI = 1;
	cfg.weight->mergeTrig = 1;
	bool doMixing = 1;

	int nhibin_mix= 180, nvz_mix = 60;
	float hibin_max_mix=180, hibin_min_mix=0;

        TString infname = "/eos/cms/store/group/phys_heavyions/wangx/HI2018Data/HIHardProbes_HIRun2018A-04Apr2019-v1_CSVTagVars/HIHardProbes/HIHardProbes_HIRun2018A-04Apr2019-v1-CSVTagVars_jet80or100_goldenJSON/201122_193133/0000/skim_103.root";
	TString mixing_buffer = "root://eoscms.cern.ch//store/group/phys_heavyions/wangx/mixingBuffer/mixing_buffer_data_minbias_ordered.root";

	std::vector<std::string> file_name;	
	if(doCrab){
		ReadFileList(file_name, Form("job_input_file_list_%d.txt",jobID), true);
		infname = file_name.at(0);
		mixing_buffer ="mixing_buffer.root";
	}

	auto f = TFile::Open(infname);
	auto lf = new liteFrame<eventMap, config>("test", cfg, f);
//	lf->nevt = 10;
	lf->nevt = -1;
	lf->output = "correlation.root";
	auto jp = new producerBJTC<eventMap, config>("jtc");
	jp->domixing=doMixing;
	lf->addProducer(jp);
	jp->vzmin_mix = -15;
	jp->vzmax_mix = 15;
	jp->nvz_mix = nvz_mix;
	jp->ncent_mix = nhibin_mix;
	jp->nsize = 40;
	jp->nPerTrig = 50;
	jp->hibinmin_mix = hibin_min_mix;
	jp->hibinmax_mix = hibin_max_mix;
	jp->setup_mixingTable();
	if(doMixing) jp->load_mixing_buffTree(mixing_buffer);
	lf->run();
}
