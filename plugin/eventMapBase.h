
#ifndef EVENTMAPBASE_H
#define EVENTMAPBASE_H
// this is a standlone event template for any other analysis
#include "TTreeReader.h"
#include "TTreeReaderArray.h"
#include <map>

class eventMapBase {
	public : 
		eventMapBase(TFile *f){_f = f;};
		~eventMapBase(){};
		template<typename T>
			TTreeReaderValue<T>* linkValue(std::string, TTreeReader & reader);
		template<typename T>
			TTreeReaderArray<T>* linkArray(std::string, TTreeReader & reader);
		void readTree(std::string treepath, std::string readername);
		void nextEvent();

//		virtual bool eventFilter() = 0;
//		virtual double eventWeight() = 0;

		TFile *_f = nullptr;	
		std::map<std::string,TTreeReader*> reader;

};

template<typename T>
TTreeReaderValue<T>* eventMapBase::linkValue(std::string name, TTreeReader & reader){
	auto  arr = new TTreeReaderValue<T>(reader, name.c_str());
	return arr;
}

template<typename T>
TTreeReaderArray<T>* eventMapBase::linkArray(std::string name, TTreeReader & reader){
	auto arr = new TTreeReaderArray<T>(reader, name.c_str());
	return arr;
}

void eventMapBase::readTree(std::string treepath, std::string readername){
	//reader.insert(std::pair<std::string, TTreeReader>(readername, TTreeReader(treepath.c_str(), _f)));
	TTreeReader* rd = new TTreeReader(treepath.c_str(), _f);
	reader.insert(std::pair<std::string, TTreeReader*>(readername, rd));
	return;
}

void eventMapBase::nextEvent(){
	for( auto &it : reader ){
		it.second->Next();
	}
}

#endif
