#ifndef JTCSIGNALPRODUCER_H
#define JTCSIGNALPRODUCER_H
#include "TH1.h"
#include "TF1.h"
#include "TFitResult.h"
#include "myProcesses/jtc/plugin/jtcTH1Player.h"
//#include "myProcesses/jtc/plugin/seagullFitter.h"
//#include "myProcesses/jtc/plugin/plotLib.h"
#include "myProcesses/liteFrame/plugin/Utility.h"
#include "myProcesses/liteFrame/plugin/plots/plotManager.h"

class jtcSignalProducer{
	public : jtcSignalProducer(TString name0, int npt, int ncent): _name(name0), n1(npt), n2(ncent){}
		 ~jtcSignalProducer(){}
		 virtual void scale_by_spectra(TString jname, TFile *f){
			 for(int i=0; i< n2;++i){
				 float x = ((TH1*)f->Get(Form(jname,i)))->Integral();
				 for(int j=0; j< n1;++j){
					 jrs->at(j,i)->Scale(1.0/x);
				 }}
		 }
		 virtual void loadSig(TString name, TFile *f){
			 fsig = f;
			 jrs = new jtcTH1Player(name, n1, n2); jrs->autoLoad(f);
		 }
		 virtual void loadMix(TString name, TFile *f){
			 fmix = f;
			 jmix = new jtcTH1Player(name, n1, n2); jmix->autoLoad(f);
		 }
		 virtual void produce();
		 void write(TDirectory *dir = 0);
		 void debug_mixing();
		 virtual void sb_correction(jtcTH1Player *j2);
		 void setDirectory(TDirectory* dir);
		 void debug(){
			 gStyle->SetOptStat(0);
			 deta_mix_p1 = jmix_p1->projX(_name+"_mix_deta_p1_*_*", -1.5, 4.5, "e", 0);
			 auto deta_sig_p2_rebin= jsig_p2->projX(_name+"_sig_deta_p2_*_*", -1, 1, "e", 1);
			 deta_sig_p2= jsig_p2->projX(_name+"_sig_deta_p2Unbine_*_*", -1, 1, "e", 0);
			 deta_sb_p2 = jsig_p2->projX(_name+"_sb_deta_p2_*_*", sb_ymin, sb_ymax, "e", 0);
			 deta_sig_p2->scale(0.5);
			 deta_sb_p2->scale(1.0/(sb_ymax-sb_ymin)); deta_mix_p1->rebinX(5); deta_sb_p2->rebinX(5);
			 deta_sig_p2->rebinX(5);
			 dphi_rs = jrs->projY(_name+"_rs_dphi", -1, 1, "e", 0);
			 auto c1 = new plotManager();
			 c1->initSquarePad(_name+"_c_deta_side","", n1, n2);
			 // c1->doHIarrange = 1;
			 for(int i=0; i< deta_sb_p2->Nrow();++i){
				 for(int j=0; j< deta_sb_p2->Ncol();++j){
					 set_errorbased_plot_range(deta_sb_p2->at(i,j),-3, 2.99);
				 }
			 }
			 c1->addm2TH1(deta_sb_p2, "", "pl", 1);
			 c1->addm2TH1(deta_sig_p2, "", "pl", 1);
			 c1->setXrange(-3, 2.99);
			 c1->draw();
			 c1->save(out_plot+"/canvas_sbCheck_"+_name+format);
			 auto c2 = new plotManager();
			 c2->initSquarePad(_name+"_c_deta_sig","", n1, n2);
			 c2->addm2TH1(deta_sig_p2_rebin, "", "", 1);
			 c2->setXrange(-3, 2.99);
			 float ymin[] = {-0.1, -0.2, -0.4, -1, -1, -5};
			 float ymax[] = {6, 9, 11, 50, 48, 280};
//			 for(int i=0; i<n1; ++i){
//				 for(int j=0; j<n2; ++j){
//					 c2->at(i,j)->doAutoYrange = 0;
//					 c2->at(i,j)->setYrange(ymin[i],ymax[i]);
//				 }
//			 }
//			 c2->xtitle = "#Delta#eta";
//			 c2->ytitle = "#frac{dN}{d#Delta#eta}";
			 c2->draw();
			 c2->save(out_plot+"/canvas_sigCheck_"+_name+format);
			 /*
			 */
			 auto c3 = new plotManager();
			 c3->initSquarePad(_name+"_c_deta_mix", "", n1, n2);
			 c3->addm2TH1(deta_mix_p1, "", "", 1);
			 c3->setXrange(-4, 3.99);
			 c3->draw();
			 c3->save(out_plot+"/canvas_smthMixing_"+_name+format);
			 //			 auto jdr_sig_integral = jdr_sig_p2->contractX("dr_"+_name);
			 auto c4 = new plotManager();
			 c4->initSquarePad(_name+"_c_dr_p1_sig", "", n1, n2);
			 c4->addm2TH1(jdr_sig_p2, "", "", 1);
			 c4->setXrange(0, .99);
			 c4->draw();
			 c4->save(out_plot+"/signal_dr_p1_"+_name+format);

			 auto c5 = new plotManager();
			 c5->initSquarePad(_name+"_c_dr_p0_sig", "", n1, n2);
			 c5->addm2TH1(jdr_sig_p0, "", "", 1);
			 c5->setXrange(0, .99);
			 c5->draw();
			 c5->save(out_plot+"/signal_dr_p0_"+_name+format);
			 /*
			 */
			cout<<"down"<<endl;
		 }

		 void debug2(){
			 auto tkmap = new matrixTH1Ptr(_name+"/"+_name+"_mix_tkMap_P*_C*", n1, n2);
			 tkmap->autoLoad(fmix);
			 auto c = new plotManager();
			 c->initSquarePad(_name+"_c_tkMap", "", n1, n2);
			 c->addm2TH1(tkmap, "", "", 1);
			 c->setXrange(-2.5, 2.49);
			 c->draw("colz");
			 c->save(out_plot+"/debug_mix_tkMap_"+_name+format);
		 }

		 // phase define:              signal   |   mixing
		 //      0                            raw input
		 //      1             mixing corrected |  smoothed
		 //      2               bkg subtracted |     -
		 jtcTH1Player *jrs, *jmix, *jmix_p1, *jsig_p0, *jsig_p1, *jsig_p2;
		 jtcTH1Player *jdr_sig_p0, *jdr_sig_p1, *jdr_sig_p2;

		 // for sideband check
		 jtcTH1Player *deta_sig_p1, *deta_sb_p1, *deta_sig_p2, *deta_sb_p2, *deta_mix_p1;
		 jtcTH1Player *dphi_rs, *dphi_sig_p1, *dphi_mix, *dphi_mix_p1;
		 float sb_ymin =1.7, sb_ymax=1.9;
		 bool dosmooth = 1, doSbCorrection = 0, usingSbMixing = 0;
		 //matrixPtrHolder<seagullFitter> *fitters;

		 TFile *fsig, *fmix;
		 // usually n1: npt,  n2: ncent
		 int n1, n2;
		 TString _name, output, out_plot, format=".jpg";
};

void jtcSignalProducer::write(TDirectory *dir){
	//	if(dir!=0){
	jsig_p0 = (jtcTH1Player*)jrs->clone(_name+"_sig_p0");
	jsig_p0->setDirectory(dir);
	jsig_p1->setDirectory(dir);
	jsig_p2->setDirectory(dir);
	jdr_sig_p0->setDirectory(dir);
	jdr_sig_p1->setDirectory(dir);
	jdr_sig_p2->setDirectory(dir);
	deta_mix_p1->setDirectory(dir);
	//	}
	jsig_p0->write();
	jsig_p1->write();
	jsig_p2->write();
	deta_mix_p1->write();
	jdr_sig_p0->write();
	jdr_sig_p1->write();
	jdr_sig_p2->write();
}

void jtcSignalProducer::sb_correction(jtcTH1Player *j2){
	deta_sig_p1 = jsig_p1->projX(_name+"_sig_deta_p1_*_*", -1, 1, "e", 0);
	deta_sig_p1->rebinX(5);
	deta_sb_p1 = jsig_p1->projX(_name+"_sb_deta_p1_*_*", sb_ymin, sb_ymax, "e", 0);
	auto c = new plotManager();
	c->initSquarePad(_name+"_c_deta_sig_p1", "", n1, n2);
	c->addm2TH1(deta_sig_p1, "", "", 1);
	c->setXrange(-3,2.99);
	c->draw();
	c->save(out_plot+"/sig_p1_"+_name+format);
	auto c1 = new plotManager();
	c1->initSquarePad(_name+"_c_deta_sbcorr", "", n1, n2);
	float xmin = -2.8, xmax = 2.8, centerleft = -0.15, centerright = 0.15;
	auto f1 = new TF1("f1", "pol2", xmin, xmax);
	auto fline = new TF1("fline", "pol0",  xmin, xmax);
	for(int i=0; i<n1;++i){
		for(int j=0; j<n2;++j){
			c1->cd(i, n2-1-j);
			deta_sb_p1->at(i,j)->Rebin(5);
			auto ptr0 = deta_sb_p1->at(i,j)->Fit(fline,"S0", "", xmin, xmax);
			auto ptr1 = deta_sb_p1->at(i,j)->Fit(f1,"S", "", xmin, xmax);
			deta_sb_p1->at(i,j)->SetAxisRange( xmin-0.2, xmax+0.2, "X");
			deta_sb_p1->at(i,j)->GetXaxis()->SetTitle("#Delta#eta");
			if(ptr0->Chi2()/ptr0->Ndf() > 1.1){
				float integ = f1->Integral(centerleft,centerright)/(xmax-xmin);
				jsig_p1->at(i,j)->Scale(1.0/integ);
				jtc::scale_Y_TF1((TH2*)jsig_p1->at(i,j), f1);
			}
		}
	}
	c1->save(out_plot+"/proc_sbcorr_"+_name+format);
}

void jtcSignalProducer::produce(){
	jrs->bin_size_normalization();
	jmix_p1 = jmix->prepareMixTable(_name+"_mixing_p1_*_*", dosmooth);
	jsig_p1 = (jtcTH1Player*)((matrixTH1Ptr*)jrs)->divide(*jmix_p1);
	jsig_p1 ->setName(_name+"_sig_p1_*_*");
	if(doSbCorrection) sb_correction(jsig_p1);
	jsig_p2 = jsig_p1->bkgSub(_name+"_sig_p2", 1.5, 2.5);
	jdr_sig_p0 = jsig_p1->drIntegral(_name+"_sig_p0_dr");
	jdr_sig_p1 = jsig_p1->drIntegral(_name+"_sig_p1_dr");
	jdr_sig_p2 = jsig_p2->drIntegral(_name+"_sig_p2_dr");
}

void jtcSignalProducer::debug_mixing(){
	auto c1 = new plotManager();
	c1->initSquarePad(_name+"_c_mixing", "", n1, n2);
	c1->setXrange(-1.5, 1.499);
	c1->addm2TH1(jmix, "", "", 1);
	c1->draw("colz");
	c1->save(out_plot+"/mixColz_"+_name+format);
}

#endif
