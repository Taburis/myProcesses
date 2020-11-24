
#ifndef SIMPLE_READER_H
#define SIMPLE_READER_H

#include "TFile.h"
#include "TROOT.h"
#include "TKey.h"
#include "TClass.h"
#include "TH1.h"
#include "TH2.h"
#include <unordered_map>
#include <iostream>

class simpleReader {
	public : simpleReader(){}
		 ~simpleReader(){}
		 void load(TFile *f);
		 void load_TH1_from_dir(TDirectory*dir);
		 TH1* operator [](std::string hm) {
			 return hist[hm];
		 }

		 std::unordered_map<std::string, TH1*> hist;
		 bool kLoaded = 0;
};

void simpleReader::load(TFile *f){
	TKey *key; kLoaded = 1;
	TIter next(f->GetListOfKeys());
	while ((key = (TKey*)next())) {
		TClass *cl = gROOT->GetClass(key->GetClassName());
		if (cl->InheritsFrom("TDirectory") ) load_TH1_from_dir((TDirectory*)key->ReadObj());
		if (!cl->InheritsFrom("TH1")) continue;
		auto h = (TH1*)key->ReadObj();
		hist[h->GetName()] = h;
	}
}

void simpleReader::load_TH1_from_dir(TDirectory*dir){
	TString dirname = dir->GetName();
	std::cout<<"loading TH1 from dir: "<<dirname<<std::endl;
	TKey *key;
	TIter next(dir->GetListOfKeys());
	while ((key = (TKey*)next())) {
		TClass *cl = gROOT->GetClass(key->GetClassName());
		if (!cl->InheritsFrom("TH1")) continue;
		auto h = (TH1*)key->ReadObj();
		TString keyname = dirname+"/"+h->GetName();
		hist[keyname.Data()] = h;
	}
}

#endif
