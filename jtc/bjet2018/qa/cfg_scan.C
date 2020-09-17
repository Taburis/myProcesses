
#include "myProcesses/jtc/plugin/scanPlugin_evtInfo.h"
#include "myProcesses/jtc/plugin/scanPlugin_jetReco.h"
#include "myProcesses/jtc/plugin/scanPlugin_jetQA.h"
#include "myProcesses/hiforest/plugin/eventMap_hiForest.h"
//#include "myProcesses/hiforest/plugin/eventMap_skim.h"
#include "myProcesses/jtc/config/cfg_AN_20_029_Base.h"

namespace {

	using pset_load  = configuration::pset_nominalpp;
	using cuts_load  = configuration::cuts_nominal<eventMap>;
	using weight_load= configuration::weight_pythia_c5shift<eventMap>;

	using config = configuration::configBase<pset_load, cuts_load, weight_load>;
	config c0;

}
