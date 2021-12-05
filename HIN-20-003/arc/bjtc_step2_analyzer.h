
#include "myProcesses/jtc/plugin/workflow.h"
//#include "myProcesses/jtc/plugin/Utility.h"
#include "bjtcSignalProducer.h"
#include "myProcesses/jtc/plugin/jtcUti.h"

class bjtc_step2_analyzer : public analyzer{
	public :
		bjtc_step2_analyzer(TString name, workflow &base0, ParaSet &ps ):analyzer(name,base0, ps){

		}
		~bjtc_step2_analyzer(){}
		void loadFile(TString f,bool isMC = 0);
		void loadFile(TString fsig,TString fmix, bool isMC = 0);
		void addSet(TString name);
		void eventQA();
		void addSet(TString name, int extraTag, bool jet, bool trk, bool dosmooth, bool dosb=0);
		void set_output(TString p1, TString p2);
		void jet_spectra();
		virtual void analyze() override;
		void write();
		//TString reco_tag(bool jet, bool trk);
		TFile *fsig, *fmix;
		TString output, out_plot;
		bool isMC=0, doJSOnly = 1,  do_mix_debug=0, isdjMC = 0, isSube = 0, usingSbMixing = 0, doSbSM=0;
		bool doIntegral = 0;
		int npt, ncent;
		std::vector<bjtcSignalProducer*> producers;
		//std::unordered_map<TString, jtcSignalProducer*> dict;
		std::vector<TString> list;
		bool doSbCorrection = 0, doPurityCalculation=0, addNegTag=0, dosube=0;
		TString _name;
		TH1F* purity;
};

void bjtc_step2_analyzer::loadFile(TString f0,bool ismc){
	fsig =TFile::Open(f0); isMC = ismc;
	fmix = fsig;
}

void bjtc_step2_analyzer::loadFile(TString fsig0, TString fmix0, bool ismc){
	fsig = TFile::Open(fsig0); fmix = TFile::Open(fmix0); isMC = ismc;
}

void bjtc_step2_analyzer::addSet(TString name, int extraTag, bool jet, bool trk, bool dosmooth, bool dosb){
	//extraTag used to select 0 for "sube0", 1 for "subeN0", or 2 for combing allsube in this format, doesn't work for the reco trk
	TString dirname= name+reco_tag(jet, trk);
	TString jname= jet ? "jetQASets/"+name+"_RecoLevel_pt_C%d":"jetQASets/"+name+"_GenLevel_pt_C%d";
	TString subeTag = "";
	if(trk == 0 && extraTag != 2){
		if(extraTag == 0 ){
			subeTag = "_sube0";
		}else { subeTag = "_subeN0";}
	}
	TString sname= name+subeTag+reco_tag(jet, trk);
	auto js = new bjtcSignalProducer(sname, base->npt, base->ncent);
	js->doIntegral =  doIntegral;
	js->doSbCorrection = dosb;
	js->doSbSM = doSbSM;
	js->usingSbMixing = usingSbMixing;
	js->ptLabels = ps->getVectorAsArray<TString>("ptlabels");
	js->centLabels = ps->getVectorAsArray<TString>("centlabels");
	js->output = output; js->out_plot = fig_output; js->dosmooth = dosmooth;
	js->loadSig(dirname+"/"+sname+"_pTweighted_P*_C*", fsig);
	if(!usingSbMixing) js->loadMix(dirname+"/"+sname+"_mixing_P*_C*", fmix);
	if(doIntegral) js->n1 = 1;
	list.emplace_back(name);
	cout<<fsig->GetName()<<endl;
	js->scale_by_spectra(jname, fsig);
	producers.emplace_back(js);
}


void bjtc_step2_analyzer::addSet(TString name){
	//e.g. addSet("incl");
	TString name1 = name;
	if(isMC){
		if(isSube){
			//Reco-Gen
			//addSet(name, 0, 1, 0, 1, 0); //sube0
			//addSet(name, 1, 1, 0, 1, 0); //subeN0
			addSet(name, 0, 1, 0, 1, doSbCorrection); //sube0
			addSet(name, 1, 1, 0, 1, doSbCorrection); //subeN0
			//addSet(name, 2, 1, 0, 1); //all event
			std::cout<<"added the set: "<<name1<<" to the process list.."<<std::endl;
			//Gen-Gen
			addSet(name, 0, 0, 0, 1, doSbCorrection); //sube0
			addSet(name, 1, 0, 0, 1, doSbCorrection); //subeN0
			//addSet(name, 2, 0, 0, 1); //all event
			std::cout<<"added the set: "<<name1<<" to the process list.."<<std::endl;
		}else {
			//addSet(name, 2, 1, 0, 1, 0); //all event
			//addSet(name, 2, 0, 0, 1, 0); //all event
			addSet(name, 2, 1, 0, 1, doSbCorrection); //all event
			addSet(name, 2, 0, 0, 1, doSbCorrection); //all event
		}
		if(isSube == 0) addSet(name, 0, 1, 1, 1, doSbCorrection); //all event
	}else {
		addSet(name, 2, 1, 1, 0, doSbCorrection);  //for data, only reco-reco
	}
	std::cout<<"added the set: "<<name1<<" to the process list.."<<std::endl;
	return;
}

void bjtc_step2_analyzer::set_output(TString p1, TString p2){
	output = p1; out_plot = p2+"/step2/"+_name;
}

void bjtc_step2_analyzer::analyze(){
	eventQA();
	//jet_spectra();
	for(auto *it : producers){
		it->produce();
		//it->n2 = 4;
		it->debug();
		if(do_mix_debug) it->debug_mixing();
	}
	write();
}

void bjtc_step2_analyzer::write(){
	cout<<"writing histogram to: "<<f->GetName()<<endl;
	cout<<output_file_name<<endl;
//	f = TFile::Open(output_file_name,"recreate");
	f->cd();
	//auto w = TFile::Open(path,"recreate");
	TDirectory* dir = f->mkdir(_name_);
	f->cd(_name_+"/");
	//cout<<"folder: "<<dir<<endl;

	if(doPurityCalculation)purity->Write();
	//dir->cd();
	
	for(auto *it : producers){
		cout<<"writing collection: "<<it->_name<<endl;
		it->write();}
	cout<<"Signal production: "<<_name<<" is done!"<<endl;
}

void bjtc_step2_analyzer::jet_spectra(){
	jtcTH1Player *hincl = new jtcTH1Player("inclJetPt", 1, base->ncent);
	hincl->add((TH1*)fsig->Get("jetQASets/incl_RecoLevel_pt_C0"),0,0);
	cout<<hincl->at(0,0)->GetName()<<endl;
	hincl->at(0,0)->Add((TH1*)fsig->Get("jetQASets/incl_RecoLevel_pt_C1"));
	hincl->add((TH1*)fsig->Get("jetQASets/incl_RecoLevel_pt_C2"),0,1);
	hincl->at(0,1)->Add((TH1*)fsig->Get("jetQASets/incl_RecoLevel_pt_C3"));
	jtcTH1Player *htag = new jtcTH1Player("tagJetPt", 1, base->ncent);
	htag->add((TH1D*)fsig->Get("jetQASets/tagged_RecoLevel_pt_C0"),0,0);
	htag->at(0,0)->Add((TH1*)fsig->Get("jetQASets/tagged_RecoLevel_pt_C1"));
	htag->add((TH1D*)fsig->Get("jetQASets/tagged_RecoLevel_pt_C2"),0,1);
	htag->at(0,1)->Add((TH1*)fsig->Get("jetQASets/tagged_RecoLevel_pt_C3"));
	jtcTH1Player *htagb = new jtcTH1Player("tagTrueJetPt", 1, base->ncent);
	htagb->add((TH1D*)fsig->Get("jetQASets/tagTrue_RecoLevel_pt_C0"),0,0);
	htagb->at(0,0)->Add((TH1*)fsig->Get("jetQASets/tagTrue_RecoLevel_pt_C1"));
	htagb->add((TH1D*)fsig->Get("jetQASets/tagTrue_RecoLevel_pt_C2"),0,1);
	htagb->at(0,1)->Add((TH1*)fsig->Get("jetQASets/tagTrue_RecoLevel_pt_C3"));
}

void bjtc_step2_analyzer::eventQA(){
	if(doPurityCalculation){
		TString jname = "jetQASets/tagged_RecoLevel_pt_C%d";
		float njet1, njet2;
		njet1 = ((TH1*)fsig->Get(Form(jname,0)))->Integral();
		njet2 = ((TH1*)fsig->Get(Form(jname,1)))->Integral();

		jname = "jetQASets/tagTrue_RecoLevel_pt_C%d";
		float nbjet1, nbjet2;
		nbjet1 = ((TH1*)fsig->Get(Form(jname,0)))->Integral();
		nbjet2 = ((TH1*)fsig->Get(Form(jname,1)))->Integral();
		purity = new TH1F("hp", "purity", 2, 0, 2);
		purity->SetBinContent(1, nbjet1/njet1);
		purity->SetBinContent(2, nbjet2/njet2);
	}
}

