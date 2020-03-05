
#ifndef WORKFLOW_H
#define WORKFLOW_H
#include "TFile.h"
#include "TDirectory.h"
#include "TF1.h"
#include "TH1.h"
#include "TString.h"
#include "myProcesses/jtc/plugin/ParaSet.h"

class analyzer;

class workflow{
	public : 
		workflow(TString name, ParaSet &ps0);
		~workflow(){}
		void link(analyzer *an);
		//	virtual void analyzer_setup(analyzer &an);
		void run();

		TFile *wf;
		TDirectory *baseDir;
		std::vector<analyzer*> analyzers;
		ParaSet *ps;
		bool doUpdate = 0;
		int ncent, npt;
		float *centbin, *ptbin;
		TString *centLabels, *ptLabels;
		TString _name_;
		TString output, fig_output, format=".jpg";
};
workflow::workflow(TString name, ParaSet &ps0){
	_name_ = name; ps=&ps0;
	output = ps0.getPara<TString>("output");
	output = output+"/"+_name_;
	fig_output = ps0.getPara<TString>("fig_output");
	fig_output =fig_output+"/"+_name_;
	npt = ps0.getPara<int>("npt");
	ncent = ps0.getPara<int>("ncent");
	ptbin = ps0.getVectorAsArray<float>("ptbin");
	centbin = ps0.getVectorAsArray<float>("centbin");
	ptLabels = ps0.getVectorAsArray<TString>("ptlabels");
	centLabels = ps0.getVectorAsArray<TString>("centlabels");
}
class analyzer{
	public : 
		analyzer(TString name, workflow &base0 , ParaSet &ps0 )
		{
			_name_=name; ps = &ps0;
			base = &base0;	 base->link(this);
			output=base->output; fig_output = base->fig_output+"/"+name;
			const int dir_fig = system("mkdir -p "+fig_output);
		};
		virtual void analyze()=0;
		template <typename T>
			void keep(T* obj){
				TDirectory *dir = (TDirectory*) gDirectory;
				base->wf->cd(); obj->Write();
				dir->cd();
			}
		template <typename T>
			T* get(TString name){base->wf->Get(name);}
		TString _name_;
		ParaSet *ps;
		workflow *base;
		TString output, fig_output;
};

void  workflow::link(analyzer *an){
	an->base = this;
	analyzers.emplace_back(an);
}

void workflow::run(){
	cout<<"output folder: "<<output<<endl;
	cout<<"figure ouput: "<<fig_output<<endl;
	const int dir_root = system("mkdir -p "+output);
	const int dir_fig = system("mkdir -p "+fig_output);
	if(doUpdate)wf = TFile::Open(output+"/"+_name_+"_output.root", "update");
	else wf = TFile::Open(output+"/"+_name_+"_output.root", "recreate");
	for(auto & it : analyzers){
		it->analyze();
	}
	wf->Close();
}

#endif
