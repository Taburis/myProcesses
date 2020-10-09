
#include "myProcesses/jtc/plugin/workflow.h"
#include "myProcesses/jtc/plugin/Utility.h"
#include "myProcesses/jtc/plugin/bjtcFormat2SignalProducer.h"
#include "myProcesses/jtc/plugin/jtcUti.h"

class bjtc_format2_step2_analyzer : public analyzer{
	public :
		bjtc_format2_step2_analyzer(TString name, workflow &base0, ParaSet &ps ):analyzer(name,base0, ps){

		}
		~bjtc_format2_step2_analyzer(){}
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
		bool isMC=0, doJSOnly = 1,  do_mix_debug=0, isdjMC = 0, isSube = 0, usingSbMixing = 0;
		int npt, ncent;
		std::vector<bjtcFormat2SignalProducer*> producers;
		//std::unordered_map<TString, jtcSignalProducer*> dict;
		std::vector<TString> list;
		bool doSbCorrection = 0, doPurityCalculation=0, addNegTag=0, dosube=0;
		TString _name;
		TH1F* purity;
};

void bjtc_format2_step2_analyzer::loadFile(TString f0,bool ismc){
	fsig =TFile::Open(f0); isMC = ismc;
	fmix = fsig;
}

void bjtc_format2_step2_analyzer::loadFile(TString fsig0, TString fmix0, bool ismc){
	fsig = TFile::Open(fsig0); fmix = TFile::Open(fmix0); isMC = ismc;
}

void bjtc_format2_step2_analyzer::addSet(TString name, int extraTag, bool jet, bool trk, bool dosmooth, bool dosb){
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
	auto js = new bjtcFormat2SignalProducer(sname, base->npt, base->ncent);
	js->doSbCorrection = dosb;
	js->usingSbMixing = usingSbMixing;
	js->ptLabels = ps->getVectorAsArray<TString>("ptlabels");
	js->centLabels = ps->getVectorAsArray<TString>("centlabels");
	js->output = output; js->out_plot = fig_output; js->dosmooth = dosmooth;
	js->loadSig(dirname+"/"+sname+"_pTweighted_P*_C*", fsig);
	if(!usingSbMixing) js->loadMix(dirname+"/"+sname+"_mixing_P*_C*", fmix);
	list.emplace_back(name);
	cout<<fsig->GetName()<<endl;
	js->scale_by_spectra(jname, fsig);
	producers.emplace_back(js);
}


void bjtc_format2_step2_analyzer::addSet(TString name){
	//e.g. addSet("incl");
	TString name1 = name;
	if(isMC){
		if(isSube){
			//Reco-Gen
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

void bjtc_format2_step2_analyzer::set_output(TString p1, TString p2){
	output = p1; out_plot = p2+"/step2/"+_name;
}

void bjtc_format2_step2_analyzer::analyze(){
	eventQA();
	//jet_spectra();
	for(auto *it : producers){
		it->produce();
		//it->n2 = 4;
		it->debug();
		//		it->debug2();
		if(do_mix_debug) it->debug_mixing();
	}
	write();
}

void bjtc_format2_step2_analyzer::write(){
	cout<<"writing histogram to: "<<f->GetName()<<endl;
	f->cd();
	//auto w = TFile::Open(path,"recreate");
	TDirectory* dir = f->mkdir(_name_);
	f->cd(_name_+"/");
	//cout<<"folder: "<<dir<<endl;

	if(doPurityCalculation)purity->Write();
	//dir->cd();
	for(auto *it : producers){
		it->write();}
}

void bjtc_format2_step2_analyzer::jet_spectra(){
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
	auto c = new multi_pads<overlay_pad>("cjet","", 1, base->ncent);
	c->doHIarrange = 1;
	c->addm2TH1(htag);
	c->addm2TH1(htagb);
	c->addLegend("upperright");
	c->labelHist("tag.",0);
	c->labelHist("tag. & true", 1);
	c->setRatioYrange(0.2, 0.8);
	c->draw();
	c->SaveAs(fig_output+"/jet_spectra_distribution"+base->format);
}

void bjtc_format2_step2_analyzer::eventQA(){
	TH2D** hdvz = new TH2D*[base->ncent];
	auto c = new multi_pads<fast_pad>("hdvz","", 1, base->ncent);
	for(int i=0; i<base->ncent;++i){
		hdvz[i] = (TH2D*)fmix->Get(Form("mixing_dvz_C%d",i)); 
		hdvz[i]->GetYaxis()->SetTitle("dV_{z}=V_{z}^{sig}-V_{z}^{mix}");
		hdvz[i]->GetXaxis()->SetTitle("V_{z}^{sig}");
		c->add((TH1D*)hdvz[i], 0, base->ncent-1-i);
	}
	c->draw("colz");
	c->SaveAs(fig_output+"/dVz_vs_Vz_distribution"+base->format);
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

