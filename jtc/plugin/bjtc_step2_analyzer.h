
#include "myProcesses/jtc/plugin/workflow.h"
#include "myProcesses/jtc/plugin/Utility.h"
#include "myProcesses/jtc/plugin/bjtcSignalProducer.h"
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
		void addSet(TString name, bool jet, bool trk, bool dosmooth = 1);// bool 1 for reco, 0 for gen
		void addSet(TString dir,TString name, TString mixname, bool jet, bool trk, bool dosmooth = 1, bool dosb = 1);// bool 1 for reco, 0 for gen
		void set_output(TString p1, TString p2);
		void jet_spectra();
		virtual void analyze() override;
		void write();
		//		TString reco_tag(bool jet, bool trk);
		TFile *fsig, *fmix;
		TString output, out_plot;
		bool isMC=0, doJSOnly = 1,  do_mix_debug=0;
		int npt, ncent;
		std::vector<bjtcSignalProducer*> producers;
		//std::unordered_map<TString, jtcSignalProducer*> dict;
		std::vector<TString> list;
		bool dorebin = 0, doSbCorrection = 0, doPurityCalculation=0, addNegTag=0, dosube=0;
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

//TString bjtc_step2_analyzer::reco_tag(bool jet, bool trk){
//	TString tag = jet ? "_RecoJet" : "_GenJet";
//	TString tag2 = trk ? "_RecoTrk":"_GenTrk";
//	return tag+tag2;
//}

void bjtc_step2_analyzer::addSet(TString name, bool jet, bool trk, bool dosmooth){
	addSet(name, name,name,jet,trk,dosmooth);
}
void bjtc_step2_analyzer::addSet(TString dir,TString name,TString mixname, bool jet, bool trk, bool dosmooth, bool dosb){
	TString name0= jet ? "_RecoLevel_pt_C%d":"_GenLevel_pt_C%d";
	TString jname = "jetQASets/"+dir+name0;
	TString dirname= dir+reco_tag(jet, trk);
	TString sname= name+reco_tag(jet, trk);
	TString mname= mixname+reco_tag(jet, trk);
	auto js = new bjtcSignalProducer(sname, base->npt, base->ncent);
	js->doSbCorrection = dosb;
	js->ptLabels = ps->getVectorAsArray<TString>("ptlabels");
	js->centLabels = ps->getVectorAsArray<TString>("centlabels");
	js->dorebin = dorebin;
	js->output = output; js->out_plot = fig_output; js->dosmooth = dosmooth;
	list.emplace_back(name);
	cout<<fsig->GetName()<<endl;
	js->loadSig(dirname+"/"+sname+"_pTweighted_P*_C*", fsig);
	js->loadMix(dirname+"/"+mname+"_mixing_P*_C*", fmix);
	js->scale_by_spectra(jname, fsig);
	producers.emplace_back(js);
	if( doJSOnly) return;
}	

void bjtc_step2_analyzer::addSet(TString name){
	if(dosube){
		addSet(name, name+"_sube0", name,1, 0, 0);
		addSet(name, name+"_subeN0",name,1, 0, 0, 0);
		addSet(name, name+"_sube0", name,0, 0, 0);
		addSet(name, name+"_subeN0",name,0, 0, 0, 0);
		return;
	}
	TString name1 = name+reco_tag(1,1);
	addSet(name, 1, 1, 0);
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

void bjtc_step2_analyzer::write(){
	base->wf->cd();
	//auto w = TFile::Open(path,"recreate");
	TDirectory* dir = base->wf->mkdir(_name_);
	base->wf->cd(_name_+"/");
	cout<<"folder: "<<dir<<endl;

	if(doPurityCalculation)purity->Write();
	//dir->cd();
	for(auto *it : producers){
		it->write();}
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

void bjtc_step2_analyzer::eventQA(){
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
		njet1+= ((TH1*)fsig->Get(Form(jname,1)))->Integral();
		njet2 = ((TH1*)fsig->Get(Form(jname,2)))->Integral();
		njet2+= ((TH1*)fsig->Get(Form(jname,3)))->Integral();

		jname = "jetQASets/tagTrue_RecoLevel_pt_C%d";
		float nbjet1, nbjet2;
		nbjet1 = ((TH1*)fsig->Get(Form(jname,0)))->Integral();
		nbjet1+= ((TH1*)fsig->Get(Form(jname,1)))->Integral();
		nbjet2 = ((TH1*)fsig->Get(Form(jname,2)))->Integral();
		nbjet2+= ((TH1*)fsig->Get(Form(jname,3)))->Integral();
		purity = new TH1F("hp", "purity", 2, 0, 2);
		purity->SetBinContent(1, nbjet1/njet1);
		purity->SetBinContent(2, nbjet2/njet2);
	}
}

