
#define event_content_skim
#include "myProcesses/jtc/bjet2018/config/cfg.h"
#include "myProcesses/jtc/plugin/jtcUti.h"
#include "myProcesses/jtc/plugin/producerBJTC.h"

using namespace config_AN20029;

void jtcConfig_MC_subeProcess(bool doCrab = 0, int jobID=0){


	using pset = config_AN20029::pset_nominalHI_skim;
	using src  = config_AN20029::selections;
	using weight  = config_AN20029::weight_Hydjet_nominal;
	
	using config = configBase<pset, src, weight>;
	config cfg;
	cfg.ps->isMC = 1;
	cfg.ps->isHI = 1;
	bool domixing = 1;

	int nhibin_mix= 180, nvz_mix = 60;
	float hibin_max_mix=180, hibin_min_mix=0;

        TString infname = "root://eoscms.cern.ch//store/group/phys_heavyions/wangx/HI2018_HiForestSkim/Bjet_pThat-15_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/bjetSkim_run2_FixedTagger/201009_152602/0000/skim_105.root";
	TString mixing_buffer = "/eos/cms/store/group/phys_heavyions/wangx/mixingBuffer/mixing_buffer_MC_ordered_sube_vz60_hi180.root";

	std::vector<std::string> file_name;	
	if(doCrab){
		ReadFileList(file_name, Form("job_input_file_list_%d.txt",jobID), true);
		infname = file_name.at(0);
		mixing_buffer ="mixing_buffer.root";
	}

	auto f = TFile::Open(infname);
	auto lf = new liteFrame<eventMap, config>("test", cfg, f);
//	lf->nevt = 100;
	lf->output = "correlation.root";
	auto jp = new producerBJTC<eventMap, config>("jtc");
	jp->domixing=domixing;
	lf->addProducer(jp);
	jp->dosube = 1;
	jp->vzmin_mix = -15;
	jp->vzmax_mix = 15;
	jp->nvz_mix = nvz_mix;
	jp->ncent_mix = nhibin_mix;
	jp->nsize = 40;
	jp->nPerTrig = 50;
	jp->hibinmin_mix = hibin_min_mix;
	jp->hibinmax_mix = hibin_max_mix;
	jp->setup_mixingTable();
	if(domixing) jp->load_mixing_buffTree(mixing_buffer);
	lf->run();
}
