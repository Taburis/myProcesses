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
		 void makeMixTable();
		 virtual void sb_correction(jtcTH1Player *j2);
		 void setDirectory(TDirectory* dir);
		 void plot_slice_sideband(){
			 float phimin = 1.2;
			 float phimax = 2.4;
			 if(!deta_sig_p2) {
				 deta_sig_p2=(jtcTH1Player*) jsig_p2->profileX(_name+"_sig_deta_p2_*_*", -1, 1, "e");
				 //deta_sig_p1->scale(1.0/2); 
				 deta_sig_p2->rebinX(10);
			 }
			 float step = (phimax-phimin)/3;
			 auto slice1 = (jtcTH1Player*) jsig_p2->profileX(_name+"_sb_slice1_deta_p2_*_*", phimin, phimin+step, "e");
			 auto slice2 = (jtcTH1Player*) jsig_p2->profileX(_name+"_sb_slice2_deta_p2_*_*", phimin+step, phimin+2*step, "e");
			 auto slice3 = (jtcTH1Player*) jsig_p2->profileX(_name+"_sb_slice3_deta_p2_*_*", phimin+2*step, phimax, "e");
			 //slice1->scale(1.0/step);  
			 //slice2->scale(1.0/step);  
			 //slice3->scale(1.0/step);  
			 slice1->rebinX(10);
			 slice2->rebinX(10);
			 slice3->rebinX(10);
			 auto c0 = new plotManager();
			 c0->initSquarePad(_name+"_c_deta_slice","", n1, n2);
			 for(int i=0; i< slice1->Nrow();++i){
				 for(int j=0; j< slice1->Ncol();++j){
					 c0->cd(i,j);
					 slice1->at(i,j)->SetAxisRange(-3.5,3.49, "X");
					 slice1->at(i,j)->SetAxisRange(-3.5,3.49, "X");
					 set_errorbased_plot_range(slice1->at(i,j),-4, 3.99, 5);
					 c0->at(i,j)->kSetYRange=0;
				 }
			 }
			 slice1->setXtitle("#Delta#eta");
			 c0->addm2TH1(slice1, "slice1", "pl", 1);
			 c0->addm2TH1(slice2, "slice2", "pl", 1);
			 c0->addm2TH1(slice3, "slice3", "pl", 1);
			 c0->addm2TH1(deta_sig_p2, "signal", "pl", 1);
			 c0->setXrange(-3.8, 3.79);
			 c0->draw();
			 c0->drawLegend("lowermid");
			 c0->save(out_plot+"/check_sb_slice_deta_"+_name+format);

		 }
		 void debug(){
			 gStyle->SetOptStat(0);
			 //deta_mix_p1 = jmix_p1->projX(_name+"_mix_deta_p1_*_*", -1.5, 4.5, "e", 0);
			 deta_mix_p1 = jmix_p1->projX(_name+"_mix_deta_p1_*_*", -1.5, 4.5, "e", 1);
			 auto deta_sig_p2_rebin= jsig_p2->projX(_name+"_sig_deta_unbined_p2_*_*", -1, 1, "e", 1);
			 auto deta_sig_p1_rebin= jsig_p1->projX(_name+"_sig_deta_unbined_p1_*_*", -1, 1, "e", 1);
			 if(!deta_sig_p2){ 
				 deta_sig_p2=(jtcTH1Player*) jsig_p2->profileX(_name+"_sig_deta_p2_*_*", -1, 1, "e");
				 //deta_sig_p2=(jtcTH1Player*) jsig_p2->projX(_name+"_sig_deta_p2_*_*", -1, 1, "e", 0);
				 deta_sig_p2->rebinX(5);
			 }
			 deta_sb_p2 =(jtcTH1Player*) jsig_p2->profileX(_name+"_sb_deta_p2_*_*", sb_ymin, sb_ymax, "e");
			 //deta_sig_p2->scale(0.5);
			 //deta_sb_p2->scale(1.0/(sb_ymax-sb_ymin));  
			 deta_sb_p2->rebinX(5);
			 dphi_rs = jrs->projY(_name+"_rs_dphi", -1, 1, "e", 0);
			 auto cphi = new plotManager();
			 dphi_rs->setXtitle("#Delta#phi");
			 cphi->initSquarePad(_name+"_c_dphi_p0","", n1, n2);
			 cphi->addm2TH1(dphi_rs, "", "", 1);
			 cphi->setXrange(0.5, 3.);
			 cphi->draw();
			 cphi->save(out_plot+"/raw_signal_dphi_"+_name+format);


			 deta_sig_p0= jrs->projX(_name+"_sig_deta_p0_check_*_*", -1, 1, "e", 1);

			 auto c0 = new plotManager();
			 deta_sig_p0->setXtitle("#Delta#eta");
			 c0->initSquarePad(_name+"_c_deta_p0","", n1, n2);
			 c0->addm2TH1(deta_sig_p0, "", "", 1);
			 c0->setXrange(-3.5, 3.499);
			 c0->draw();
			 c0->save(out_plot+"/raw_signal_deta_"+_name+format);

			 auto xc = new plotManager();
			 xc->initSquarePad(_name+"_c_deta_check_sig_p1","", n1, n2);
			 //c1->doHIarrange = 1;
			 for(int i=0; i< deta_sb_p2->Nrow();++i){
				 for(int j=0; j< deta_sb_p2->Ncol();++j){
					 //xc1->cd(i,j);
					 deta_sig_p1_rebin->at(i,j)->SetAxisRange(-3,2.99, "X");
					 //deta_sb_p2->at(i,j)->GetXaxis()->SetTitle("#Delta#eta");
					 //deta_sb_p2->at(i,j)->Draw();
					 //deta_sig_p2->at(i,j)->Draw("same");
					 xc->at(i,j)->kSetYRange=0;
				 }
			 }

			 xc->setXrange(-3.5, 3.49);
			 deta_sig_p1_rebin->setXtitle("#Delta#eta");
			 xc->addm2TH1(deta_sig_p1_rebin, "sig p1", "pl", 1);
			 xc->draw();
			 xc->drawLegend();
			 xc->save(out_plot+"/canvas_deta_p1_"+_name+format);

			 auto xc1 = new plotManager();
			 xc1->initSquarePad(_name+"_c_deta_check_side","", n1, n2);
			 //c1->doHIarrange = 1;
			 for(int i=0; i< deta_sb_p2->Nrow();++i){
				 for(int j=0; j< deta_sb_p2->Ncol();++j){
					 //xc1->cd(i,j);
					 deta_sb_p2->at(i,j)->SetAxisRange(-3.5,3.49, "X");
					 deta_sig_p2->at(i,j)->SetAxisRange(-3.5,3.49, "X");
					 //deta_sb_p2->at(i,j)->GetXaxis()->SetTitle("#Delta#eta");
					 //deta_sb_p2->at(i,j)->Draw();
					 //deta_sig_p2->at(i,j)->Draw("same");
					 //deta_sig_p2->at(i,j)->SetMarkerColor(kBlack);
					 set_errorbased_plot_range(deta_sb_p2->at(i,j),-3.5, 3.49);
					 xc1->at(i,j)->kSetYRange=0;
				 }
			 }

			 xc1->setXrange(-2.5, 2.499);
			 deta_sb_p2->setXtitle("#Delta#eta");
			 xc1->addm2TH1(deta_sb_p2, "side-band", "pl", 1);
			 xc1->addm2TH1(deta_sig_p2, "signal", "pl", 1);
			 xc1->draw();
			 xc1->drawLegend();
			 xc1->save(out_plot+"/canvas_sbCheck_"+_name+format);
			 auto c2 = new plotManager();
			 c2->initSquarePad(_name+"_c_deta_sig","", n1, n2);
			 deta_sig_p2->setXtitle("#Delta#eta");
			 c2->addm2TH1(deta_sig_p2_rebin, "", "", 1);
			 c2->setXrange(-3.5, 3.49);
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
			 deta_mix_p1->setXtitle("#Delta#eta");
			 c3->addm2TH1(deta_mix_p1, "", "", 1);
			 c3->setXrange(-3.5, 3.49);
			 c3->draw();
			 c3->save(out_plot+"/canvas_smthMixing_"+_name+format);
			 //			 auto jdr_sig_integral = jdr_sig_p2->contractX("dr_"+_name);
			 auto c4 = new plotManager();
			 c4->initSquarePad(_name+"_c_dr_p2_sig", "", n1, n2);
			 jdr_sig_p2->setXtitle("#Deltar");
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
		 jtcTH1Player *deta_sig_p0, *deta_sig_p1=nullptr, *deta_sb_p1=nullptr, *deta_sig_p2 = nullptr, *deta_sb_p2 = nullptr, *deta_mix_p1 = nullptr;
		 jtcTH1Player *dphi_rs, *dphi_sig_p1, *dphi_mix, *dphi_mix_p1;
		 float sb_ymin =1.4, sb_ymax=2.2;
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
	jmix_p1->write();
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
				jtc::scale_Y_TF1((TH2*)jsig_p1->at(i,j), f1, xmin, xmax);
			}
		}
	}
	c1->save(out_plot+"/proc_sbcorr_"+_name+format);
}

void jtcSignalProducer::makeMixTable(){
	jmix_p1 = new jtcTH1Player(_name+"_mixing_p1",jrs->Nrow(),jrs->Ncol());
	bool ptStart = 4;
	for(int i=0; i<jmix_p1->Nrow(); ++i){
		for(int j=0; j<jmix_p1->Ncol(); ++j){
			bool dosmooth = ptStart < i? 1: 0;
			if(usingSbMixing) 
				jmix_p1->add(jtc::sideBandMixingTableMaker((TH2D*)jrs->at(i,j),1.6,2.),i,j);
			else 
				jmix_p1->add(jtc::mixingTableMaker((TH2D*)jmix->at(i,j),dosmooth),i,j);
		}
	}
	jmix_p1->setName(_name+"_mixing_p1_*_*");
}

void jtcSignalProducer::produce(){
	jrs->bin_size_normalization();
	//	jmix_p1 = jmix->prepareMixTable(_name+"_mixing_p1_*_*", dosmooth);
	makeMixTable();
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
	auto deta_sb_p0 =  jrs->projX(_name+"_sb_deta_p0_*_*", sb_ymin, sb_ymax, "e", 0);
	if(!deta_sb_p1) {
		deta_sb_p1 = jsig_p1->projX(_name+"_sb_deta_p1_*_*", sb_ymin, sb_ymax, "e", 0);
		deta_sb_p1->rebin(4);
	}
	deta_sb_p0->rebin(4);
	auto deta_me = jmix_p1->projX(_name+"_me_sb_deta_p0_*_*", sb_ymin, sb_ymax, "e", 0);
	deta_me->rebin(4);
	int n = deta_sb_p0->at(0,0)->FindBin(0);

	for(int j=0; j<n1; j++){
		for(int k=0; k<n2; k++){
			//		cout<<"ploting :"<<j<<" , "<<k<<endl;
			//		c1->cd(j,n2-1-k);
			//		//jmix->at(j,k)->SetAxisRange(-3.5, 3.499, "X");
			//		//jmix->at(j,k)->Draw("colz");
			c1->at(j,k)->kSetYRange=0;
			auto h = deta_sb_p0->at(j,k);
			int nn = 1;
			float s1 = h->Integral(n-nn, n+nn);
			float s2 = deta_sb_p1->at(j,k)->Integral(n-nn, n+nn);
			float s3 = deta_me->at(j,k)->Integral(n-nn, n+nn);
			deta_sb_p1->at(j,k)->GetXaxis()->SetTitle("#Delta#eta");
			deta_sb_p0->at(j,k)->Scale((2.0*nn+1)/s1);
			deta_sb_p1->at(j,k)->Scale((2.0*nn+1)/s1);
			deta_me->at(j,k)->Scale((2.0*nn+1)/s3);
			set_errorbased_plot_range(deta_sb_p1->at(j,k),-2.5, 2.49, 10);
		}
	}
	c1->addm2TH1(deta_sb_p1, "after ME");	
	c1->addm2TH1(deta_sb_p0, "before ME");	
	c1->addm2TH1(deta_me, "ME");	
	//c1->addm2TH1(jmix, "", "", 1);
	//c1->draw("colz");
	c1->setXrange(-2, 2);
	c1->draw();
	c1->drawLegend();
	c1->save(out_plot+"/debug_sb_"+_name+format);
	//c1->save(out_plot+"/mixColz_"+_name+format);
}

#endif
