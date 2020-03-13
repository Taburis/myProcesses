
#include "TString.h"
#include "myProcesses/jtc/plugin/ParaSet.h"
#include <sstream>
#include <string>
#include <iostream>

namespace config_bjtc2018aa {
	const int npt = 6;
	const int ncent = 2;
	float ptbins[] = {1, 2, 3,4,8, 12, 300};
	TString* ptLabels, *centLabels;
	//drop the tails
	float centbins[] = {0, 60, 180};

	ParaSet * config_init(){
		auto ps = new ParaSet("config_pset");
		std::stringstream s1, s2;
		ptLabels= new TString[npt+1];
		for(int i=0; i<npt; i++){
			s1.str(std::string());
			s2.str(std::string());
			s1<<ptbins[i];
			s2<<ptbins[i+1];
			ptLabels[i] =TString("Pt:"+s1.str()+"-"+s2.str());
		}
		centLabels = new TString[ncent];
		std::stringstream sstream1, sstream2;
		for(int i=0; i<ncent; ++i){
			sstream1.str(std::string());
			sstream2.str(std::string());
			sstream1<<float(centbins[i])/2;
			sstream2<<float(centbins[i+1])/2;
			centLabels[i]="Cent: "+sstream1.str()+"-"+sstream2.str()+"%";
		}

		ps->setPara<int>("npt", npt);
		ps->setParaVector<float>("ptbin",npt+1, ptbins);
		ps->setParaVector<TString>("ptlabels",npt, ptLabels);
		ps->setPara<int>("ncent", ncent);
		ps->setParaVector<float>("centbin",ncent+1, centbins);
		ps->setParaVector<TString>("centlabels",ncent, centLabels);
		return ps;
	}
};

