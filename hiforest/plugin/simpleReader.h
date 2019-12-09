
#ifndef SIMPLE_READER_H
#define SIMPLE_READER_H

#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include <unordered_map>

class simpleReader {
		public : simpleReader(){}
				 ~simpleReader(){}
				 void load(TFile *f);
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
				if (!cl->InheritsFrom("TH1")) continue;
				auto h = (TH1*)key->ReadObj();
				hist[h->GetName()] = h;
		}
}

#endif
