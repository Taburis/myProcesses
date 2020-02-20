
#include "myProcesses/jtc/plugin/Utility.h"
#include "myProcesses/jtc/plugin/jtcSignalProducer.h"

class jtcStep2Producer{
	public :
		jtcStep2Producer(TString name){_name = name;}
		~jtcStep2Producer(){}
		void loadFile(TString f,bool isMC = 0);
		void addSet(TString name);
		void addSet(TString name, bool jet, bool trk);// bool 1 for reco, 0 for gen
		void set_output(TString p1, TString p2);
		void produce();
		void write();
		TString reco_tag(bool jet, bool trk);
		TFile *f;
		TString output, out_plot;
		bool isMC=0, doJSOnly = 1;	
		int npt, ncent;
		std::vector<jtcSignalProducer*> producers;
		//std::unordered_map<TString, jtcSignalProducer*> dict;
		std::vector<TString> list;
		centralityHelper *cent;
		TString _name;
};

void jtcStep2Producer::loadFile(TString f0,bool ismc){
	f =TFile::Open(f0); isMC = ismc;
}

TString jtcStep2Producer::reco_tag(bool jet, bool trk){
	TString tag = jet ? "_RecoJet" : "_GenJet";
	TString tag2 = trk ? "_RecoTrk":"_GenTrk";
	return tag+tag2;
}

void jtcStep2Producer::addSet(TString name, bool jet, bool trk){
	TString name0= jet ? "_RecoLevel_pt_C%d":"_GenLevel_pt_C%d";
	TString jname = "jetQASets/"+name+name0;
	TString sname= name+reco_tag(jet, trk);
	auto js = new jtcSignalProducer(sname, npt, ncent);
	js->output = output; js->out_plot = out_plot;
	list.emplace_back(name);
	js->loadSig(sname+"/"+sname+"_pTweighted_P*_C*", f);
	js->loadMix(sname+"/"+sname+"_mixing_P*_C*", f);
	js->scale_by_spectra(jname, f);
	producers.emplace_back(js);
	if( doJSOnly) return;
}

void jtcStep2Producer::addSet(TString name){
	TString name1 = name+reco_tag(1,1);
	addSet(name, 1, 1);
	std::cout<<"added the set: "<<name1<<" to the process list.."<<std::endl;
	if(!isMC) return ;
	addSet(name, 1, 0);
	name1= name+reco_tag(1, 0);
	std::cout<<"added the set: "<<name1<<" to the process list.."<<std::endl;
	addSet(name, 0, 0);
	name1= name+reco_tag(0, 0);
	std::cout<<"added the set: "<<name1<<" to the process list.."<<std::endl;
}

void jtcStep2Producer::set_output(TString p1, TString p2){
	output = p1; out_plot = p2+"/step2/"+_name;
}

void jtcStep2Producer::produce(){
        const int dir_err = system("mkdir -p "+out_plot);	
	for(auto *it : producers){
		it->produce();
		it->debug();
		it->debug2();
	}
	write();
}

void jtcStep2Producer::write(){
	TString path = output+"/step2/"+_name+"_step2.root";
	auto w = TFile::Open(path,"recreate");
	w->cd();
	for(auto *it : producers){ it->write();}
	w->Close();
}

