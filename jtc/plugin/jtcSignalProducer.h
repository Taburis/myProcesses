#ifndef JTCSIGNALPRODUCER_H
#define JTCSIGNALPRODUCER_H
#include "myProcesses/jtc/plugin/jtcTH1Player.h"
//#include "myProcesses/jtc/plugin/seagullFitter.h"
#include "myProcesses/jtc/plugin/PLOTLIB/multi_pads.h"

class jtcSignalProducer{
		public : jtcSignalProducer(TString name0, int npt, int ncent): name(name0), n1(npt), n2(ncent){}
				 ~jtcSignalProducer(){}
				 void scale_by_spectra(TString name, TFile *f){
						 for(int i=0; i< n2;++i){
								 float x = ((TH1*)f->Get(name+Form("_%d",i)))->Integral();
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
				 void debug(){
						 deta_sig_p1 = jsig_p1->projX(name+"_sig_deta_p1", -1, 1, "e", 0);
						 deta_mix_p1 = jmix_p1->projX(name+"_mix_deta_p1", -1, 1, "e", 0);
						 deta_sig_p2 = jsig_p2->projX(name+"_sig_deta_p2", -1, 1, "e", 0);
						 deta_sb_p1 = jsig_p1->projX(name+"_sb_deta_p1", sb_ymin, sb_ymax, "e", 0);
						 deta_sb_p2 = jsig_p2->projX(name+"_sb_deta_p2", sb_ymin, sb_ymax, "e", 0);
						 deta_mix_p1->rebinX(5);
						 deta_sig_p1->rebinX(5);
						 deta_sig_p2->rebinX(5);
						 deta_sb_p1 ->rebinX(5);
						 deta_sb_p2 ->rebinX(5);
						 dphi_rs = jrs->projY(name+"_rs_dphi", -1, 1, "e", 0);
						 //deta_sig_p2 = jsig_p2->projX(name+"_sig_deta_p2", -1, 1);
						 //deta_sb_p2 = jsig_p2->projX(name+"_sb_deta_p2", sb_ymin, sb_ymax);
				 }
/*
				 multi_pads<base_pad>* residual_mixing_correction(int n, int m, TPoint (*arr)(int, int) = nullptr){
						 deta_sb_p1 = jsig_p2->projX(name+"_sb_deta_p1", sb_ymin, sb_ymax, "e", 0);
						 deta_sb_p1 ->rebinX(5);
						 fitters = new matrixPtrHolder<seagullFitter>(n1, n2);
						 auto c = new multi_pads<base_pad>(name+"_seagull_fitting", "", n,m);
						 for(int i=0; i<n1; i++){
								 for(int j=0; j<n2; j++){
										 auto fitter = new seagullFitter();
										 TPoint point;
                                         if(arr== nullptr){
                                                 point = c->flatten_arrange(i,j, deta_sb_p1->Nrow(), deta_sb_p1->Ncol());
                                         }else point = arr(i,j);
										 auto pad = c->fpads.at(int(point.GetX()), int(point.GetY()));
										 pad->pad->cd();
										 pad->xmin = -3; pad->xmax = 2.99;
										 pad->setup_frame((TH1D*)deta_sb_p1->at(i,j));

										 //fitter->addTarget((TH2D*)jrs->at(i,j));
										 fitter->addTarget((TH2D*)jsig_p1->at(i,j));
										 fitter->fit();
								 }}
						 return c;
				 }
*/

				 // phase define:              signal   |   mixing
				 //      0                            raw input
				 //      1             mixing corrected |  smoothed
				 //      2               bkg subtracted |     -
				 jtcTH1Player *jrs, *jmix, *jmix_p1, *jsig_p1, *jsig_p2;
				 jtcTH1Player *jdr_sig_p2;

				 // for sideband check
				 jtcTH1Player *deta_sig_p1, *deta_sb_p1, *deta_sig_p2, *deta_sb_p2, *deta_mix_p1;
				 jtcTH1Player *dphi_rs, *dphi_sig_p1, *dphi_mix, *dphi_mix_p1;
				 float sb_ymin =1.4, sb_ymax=2;
				 matrixPtrHolder<seagullFitter> *fitters;

				 // usually n1: npt,  n2: ncent
				 int n1, n2;
				 TString name;
};

#endif
