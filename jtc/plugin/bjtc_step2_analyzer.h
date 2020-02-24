
#include "myProcesses/jtc/plugin/workflow.h"
#include "myProcesses/jtc/plugin/Utility.h"
#include "myProcesses/jtc/plugin/jtcSignalProducer.h"
#include "myProcesses/jtc/plugin/jtcUti.h"

class bjtc_step2_analyzer : public analyzer{
	public :
		bjtc_step2_analyzer(TString name, workflow &base0, ParaSet &ps ):analyzer(name,base0, ps){}
		~bjtc_step2_analyzer(){}
		void loadFile(TString f,bool isMC = 0);
		void addSet(TString name);
		void addSet(TString name, bool jet, bool trk);// bool 1 for reco, 0 for gen
		void set_output(TString p1, TString p2);
		virtual void analyze() override;
		void write();
//		TString reco_tag(bool jet, bool trk);
		TFile *f;
		TString output, out_plot;
		bool isMC=0, doJSOnly = 1;	
		int npt, ncent;
		std::vector<jtcSignalProducer*> producers;
		//std::unordered_map<TString, jtcSignalProducer*> dict;
		std::vector<TString> list;
		TString _name;
};

void bjtc_step2_analyzer::loadFile(TString f0,bool ismc){
	f =TFile::Open(f0); isMC = ismc;
}

//TString bjtc_step2_analyzer::reco_tag(bool jet, bool trk){
//	TString tag = jet ? "_RecoJet" : "_GenJet";
//	TString tag2 = trk ? "_RecoTrk":"_GenTrk";
//	return tag+tag2;
//}

void bjtc_step2_analyzer::addSet(TString name, bool jet, bool trk){
	TString name0= jet ? "_RecoLevel_pt_C%d":"_GenLevel_pt_C%d";
	TString jname = "jetQASets/"+name+name0;
	TString sname= name+reco_tag(jet, trk);
	auto js = new jtcSignalProducer(sname, base->npt, base->ncent);
	js->output = output; js->out_plot = fig_output;
	list.emplace_back(name);
	cout<<f->GetName()<<endl;
	js->loadSig(sname+"/"+sname+"_P*_C*", f);
	//js->loadSig(sname+"/"+sname+"_pTweighted_P*_C*", f);
	js->loadMix(sname+"/"+sname+"_mixing_P*_C*", f);
	js->scale_by_spectra(jname, f);
	producers.emplace_back(js);
	if( doJSOnly) return;
}

void bjtc_step2_analyzer::addSet(TString name){
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

void bjtc_step2_analyzer::set_output(TString p1, TString p2){
	output = p1; out_plot = p2+"/step2/"+_name;
}

void bjtc_step2_analyzer::analyze(){
	for(auto *it : producers){
		it->produce();
		it->debug();
		//it->debug2();
	}
	write();
}

void bjtc_step2_analyzer::write(){
	base->wf->cd();
	//auto w = TFile::Open(path,"recreate");
	for(auto *it : producers){ it->write();}
}

