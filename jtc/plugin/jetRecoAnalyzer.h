
#ifndef jetRecoAnalyzer_H
#define jetRecoAnalyzer_H
#include <cstdlib>
#include "myProcesses/hiforest/plugin/simpleReader.h"
#include "myProcesses/jtc/plugin/Utility.h"
#include "myProcesses/jtc/plugin/plotLib.h"

class jetRecoAnalyzer{
	public :
	jetRecoAnalyzer(){}
	~jetRecoAnalyzer(){}
	void analyze();
	void addData(TString fdata);
	void addMC(TString fdata);
	void normalization(TH1* h);
	multi_pads<overlay_pad> * drawCompare(TString name);
	TFile *_dataf, *_mcf;
	simpleReader smc, sda;
	int ncent;
	centralityHelper *cent;
	TString output, format = ".jpg";
};

void jetRecoAnalyzer::addData(TString fdata){
	_dataf= TFile::Open(fdata);
	sda.load(_dataf);
}
void jetRecoAnalyzer::addMC(TString fdata){
	_mcf= TFile::Open(fdata);
	smc.load(_mcf);
}
void jetRecoAnalyzer::normalization(TH1* h){
	h->Scale(1.0/h->Integral());
	divide_bin_size(h);
}

multi_pads<overlay_pad> *  jetRecoAnalyzer::drawCompare(TString hist){
	auto c = new multi_pads<overlay_pad>("c_"+hist, "", 1, ncent);
	for(int i=0; i<ncent; i++){
		TString histn = hist+"_C%d";
		TH1* hmc = smc[Form(histn,i)];
		TH1* hda = sda[Form(histn,i)];
		hmc->Rebin(2);
		hda->Rebin(2);
		normalization(hmc);
		normalization(hda);
		c->add(hmc, 0, ncent-1-i);
		c->add(hda, 0, ncent-1-i);
		c->at(0, ncent-i-1)->ratio_title = "MC/Data";
	}
	return c;
}
void jetRecoAnalyzer::analyze(){
	ncent = cent->nbins;
	gStyle->SetOptStat(0);
	TString folder = output+"/jetRecoStudy";
	const int dir_err = system("mkdir -p "+folder);	
	auto c = drawCompare("hrdmCone");
	//c->doLogy = 1;
	c->setRatioYrange(0,2.5);
	c->setXrange(0,30);
	c->xtitle="p_{T}^{trk}";
	c->draw();
	c->SaveAs(folder+"/randomCone"+format);
	c = drawCompare("hjtCone");
	c->setRatioYrange(0,2.5);
	c->setXrange(0,30);
	c->xtitle="p_{T}^{trk}";
	//c->doLogy = 1;
	c->draw();
	c->SaveAs(folder+"/jetCone"+format);
}

#endif

