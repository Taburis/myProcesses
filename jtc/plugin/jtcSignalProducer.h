#ifndef JTCSIGNALPRODUCER_H
#define JTCSIGNALPRODUCER_H
#include "myProcesses/jtc/plugin/jtcTH1Player.h"
//#include "myProcesses/jtc/plugin/seagullFitter.h"
#include "myProcesses/jtc/plugin/PLOTLIB/multi_pads.h"

class jtcSignalProducer{
	public : jtcSignalProducer(TString name0, int npt, int ncent): name(name0), n1(npt), n2(ncent){}
		 ~jtcSignalProducer(){}
		 void scale_by_spectra(TString jname, TFile *f){
			 for(int i=0; i< n2;++i){
				 float x = ((TH1*)f->Get(Form(jname,i)))->Integral();
				 for(int j=0; j< n1;++j){
					 jrs->at(j,i)->Scale(1.0/x);
				 }}
		 }
		 void loadSig(TString name, TFile *f){
			 jrs = new jtcTH1Player(name, n1, n2); jrs->autoLoad(f);
		 }
		 void loadMix(TString name, TFile *f){
			 jmix = new jtcTH1Player(name, n1, n2); jmix->autoLoad(f);
		 }
		 void produce(){
			 jrs->invariant();
			 jmix_p1 = jmix->smoothMixing(name+"_mixing_p1");
			 jsig_p1 = (jtcTH1Player*) (*((matrixTH1Ptr*)jrs)/(*((matrixTH1Ptr*)jmix_p1)));
			 jsig_p1 ->setName(name+"_sig_p1");
			 jsig_p2 = jsig_p1->bkgSub(name+"_sig_p2", 1.5, 2.5);
			 jdr_sig_p2 = jsig_p2->drIntegral(name+"_sig_p2_dr");
		 }
		 void write();
		 void debug(){
			 gStyle->SetOptStat(0);
			 deta_sig_p1 = jsig_p1->projX(name+"_sig_deta_p1", -1, 1, "e", 0);
			 deta_mix_p1 = jmix_p1->projX(name+"_mix_deta_p1", -1.5, 4.5, "e", 0);
			 deta_sig_p2 = jsig_p2->projX(name+"_sig_deta_p2", -1, 1, "e", 0);
			 deta_sb_p1 = jsig_p1->projX(name+"_sb_deta_p1", sb_ymin, sb_ymax, "e", 0);
			 deta_sb_p2 = jsig_p2->projX(name+"_sb_deta_p2", sb_ymin, sb_ymax, "e", 0);
			 deta_mix_p1->rebinX(5);
			 deta_sig_p1->rebinX(5);
			 deta_sig_p2->rebinX(5);
			 deta_sb_p1 ->rebinX(5);
			 deta_sb_p2 ->rebinX(5);
			 dphi_rs = jrs->projY(name+"_rs_dphi", -1, 1, "e", 0);
			 auto c1 = new multi_pads<base_pad>(name+"_c_deta_side", "", n1, n2);
			 c1->doHIarrange = 1;
			 c1->addm2TH1(deta_sb_p2);
			 c1->addm2TH1(deta_sig_p2);
			 c1->setXrange(-3, 2.99);
			 c1->xtitle = "#Delta#eta";
			 c1->draw();
			 c1->SaveAs(out_plot+"/canvas_sbCheck_"+name+format);
			 auto c2 = new multi_pads<base_pad>(name+"_c_deta_sig", "", n1, n2);
			 c2->doHIarrange = 1;
			 c2->addm2TH1(deta_sig_p2);
			 c2->setXrange(-3, 2.99);
			 c1->xtitle = "#Delta#eta";
			 c2->draw();
			 c2->SaveAs(out_plot+"/canvas_sigCheck_"+name+format);
			 auto c3 = new multi_pads<base_pad>(name+"_c_deta_mix", "", n1, n2);
			 c3->doHIarrange = 1;
			 c3->addm2TH1(deta_mix_p1);
			 c3->setXrange(-4, 3.99);
			 c3->xtitle = "#Delta#eta";
			 c3->draw();
			 c3->SaveAs(out_plot+"/canvas_smthMixing_"+name+format);

		 }

		 // phase define:              signal   |   mixing
		 //      0                            raw input
		 //      1             mixing corrected |  smoothed
		 //      2               bkg subtracted |     -
		 jtcTH1Player *jrs, *jmix, *jmix_p1, *jsig_p1, *jsig_p2;
		 jtcTH1Player *jdr_sig_p2;

		 // for sideband check
		 jtcTH1Player *deta_sig_p1, *deta_sb_p1, *deta_sig_p2, *deta_sb_p2, *deta_mix_p1;
		 jtcTH1Player *dphi_rs, *dphi_sig_p1, *dphi_mix, *dphi_mix_p1;
		 float sb_ymin =1.7, sb_ymax=1.9;
		 //matrixPtrHolder<seagullFitter> *fitters;

		 // usually n1: npt,  n2: ncent
		 int n1, n2;
		 TString name, output, out_plot, format=".jpg";
};

void jtcSignalProducer::write(){
	jsig_p1->write();
	jsig_p2->write();
}

#endif
