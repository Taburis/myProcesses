#ifndef bJTCFORMAT2SIGNALPRODUCER_H
#define bJTCFORMAT2SIGNALPRODUCER_H
//#include "myProcesses/jtc/plugin/seagullFitter.h"
#include "myProcesses/jtc/plugin/jtcSignalProducer.h"

Double_t fs1(Double_t *x, Double_t *par);
Double_t fsExp(Double_t *x, Double_t *par);
class bjtcFormat2SignalProducer : public jtcSignalProducer{
	public : bjtcFormat2SignalProducer(TString name0, int npt, int ncent):jtcSignalProducer(name0,npt,ncent){
		 }
		 ~bjtcFormat2SignalProducer(){}
		 virtual void produce() override;
		 void makeMixTable();
		 virtual void sb_correction(jtcTH1Player *j2) override;
		 jtcTH1Player* rebin(TString name, jtcTH1Player* js);
		 
		 virtual void loadSig(TString name, TFile *f)override{
			 fsig = f;
			 jrs = new jtcTH1Player(name, n1, 2); jrs->autoLoad(f);
		 }
		 void addSig(TString name, TFile *f){
			 jtcTH1Player jrs2(name, n1, n2); jrs2.autoLoad(f);
			 for(int i=0; i<n1; i++){
				 for(int j=0; j<n2; j++){
					 jrs->at(i,j)->Add(jrs2.at(i,j));
				 }
			 }
		 }
		 void addMix(TString name, TFile *f){
			 jtcTH1Player jmx2(name, n1, n2); jmx2.autoLoad(f);
			 for(int i=0; i<n1; i++){
				 for(int j=0; j<n2; j++){
					 jmix->at(i,j)->Add(jmx2.at(i,j));
				 }
			 }
		 }
		 virtual void loadMix(TString name, TFile *f)override{
			 fmix = f;
			 jmix = new jtcTH1Player(name, n1, 2); jmix->autoLoad(f);
		 }	

		 ParaSet *ps;
		 bool dorebin = 0;
		 TString *centLabels, *ptLabels;
};

void bjtcFormat2SignalProducer::makeMixTable(){
	jmix_p1 = new jtcTH1Player(_name+"_mixing_p1",jmix->Nrow(),jmix->Ncol());
	bool ptStart = 4;
	for(int i=0; i<jmix_p1->Nrow(); ++i){
		for(int j=0; j<jmix_p1->Ncol(); ++j){
			bool dosmooth = ptStart < i? 1: 0;
			jmix_p1->add(jtc::mixingTableMaker((TH2D*)jmix->at(i,j),dosmooth),i,j);
		}
	}
	jmix_p1->setName(_name+"_mixing_p1_*_*");
}

void bjtcFormat2SignalProducer::produce(){
	jrs->bin_size_normalization();
	jmix_p1 = jmix->prepareMixTable(_name+"_mixing_p1", dosmooth);
	makeMixTable();
	jsig_p1 = (jtcTH1Player*)((matrixTH1Ptr*)jrs)->divide(*jmix_p1);
	jsig_p1 ->setName(_name+"_sig_p1_*_*");
	if(doSbCorrection) {
		sb_correction(jsig_p1);
	}
	jsig_p2 = jsig_p1->bkgSub(_name+"_sig_p2", 1.5, 2.5);
	jdr_sig_p0 = jrs->drIntegral(_name+"_sig_p0_dr");
	jdr_sig_p1 = jsig_p1->drIntegral(_name+"_sig_p1_dr");
	jdr_sig_p2 = jsig_p2->drIntegral(_name+"_sig_p2_dr");
}

jtcTH1Player* bjtcFormat2SignalProducer::rebin(TString name, jtcTH1Player *js){
	auto jnew = new jtcTH1Player(name,n1, n2);
	for(int j=0; j<6; ++j){
		TString title = centLabels[0]+", "+ptLabels[j];
		jnew->add(js->at(j,0),j,0);
		jnew->at(j,0)->Add(js->at(j,1));	
		jnew->at(j,0)->SetTitle(title);
		jnew->add(js->at(j,2),j,1);
		jnew->at(j,1)->Add(js->at(j,3));
		title = centLabels[1]+", "+ptLabels[j];
		jnew->at(j,1)->SetTitle(title);
	}
	return jnew;
}

void bjtcFormat2SignalProducer::sb_correction(jtcTH1Player *j2){
	sb_ymin = 1.6;
	sb_ymax = 2.2;
	deta_sig_p1 = jsig_p1->projX(_name+"_sig_deta_p1_*_*", -1, 1, "e", 0);
//	deta_sig_p1->rebinX(5);
	deta_sb_p1 = jsig_p1->projX(_name+"_sb_deta_p1_*_*", sb_ymin, sb_ymax, "e", 0);
	auto c = new multi_pads<base_pad>(_name+"_c_deta_sig_p1", "", n1, n2);
	c->doHIarrange  = 1;
	c->addm2TH1(deta_sig_p1);
	c->setXrange(-3,2.99);
	c->draw();
	c->SaveAs(out_plot+"/sig_p1_"+_name+format);
	auto c1 = new multi_pads<base_pad>(_name+"_c_deta_sbcorr", "", n1, n2);
	float xmin = -3., xmax = 3., centerleft = -0.15, centerright = 0.15;
	int ncl = deta_sig_p1->at(0,0)->FindBin(centerleft);
	int ncr = deta_sig_p1->at(0,0)->FindBin(centerright);
	auto f1 = new TF1("f1", fsExp, xmin, xmax, 5);
	//auto f1 = new TF1("f1", fs1, xmin, xmax, 7);
	auto f2 = new TF1("f2", "pol2", xmin, xmax);
	auto fline = new TF1("fline", "pol0",  xmin, xmax);
	TLatex tx; tx.SetTextSize(.12);
	for(int i=0; i<n1;++i){
		for(int j=0; j<n2;++j){
			c1->CD(i, n2-1-j);
//			deta_sb_p1->at(i,j)->Rebin(5);
			auto ptr0 = deta_sb_p1->at(i,j)->Fit(fline,"S0", "", xmin, xmax);
			float init = deta_sb_p1->at(i,j)->GetBinContent(deta_sb_p1->at(i,j)->FindBin(0));
			f1->SetParameters(init, -1, 1,1, 1, 1);
			auto ptr1 = deta_sb_p1->at(i,j)->Fit(f1,"S0", "", xmin, xmax);
			auto ptr2 = deta_sb_p1->at(i,j)->Fit(f2,"S0", "", xmin, xmax);
			deta_sb_p1->at(i,j)->SetAxisRange( xmin-0.2, xmax+0.2, "X");
			deta_sb_p1->at(i,j)->GetXaxis()->SetTitle("#Delta#eta");
			deta_sb_p1->at(i,j)->Draw();
			float scale = 1.3;
			if(ptr0->Chi2()/ptr0->Ndf() > scale*ptr1->Chi2()/ptr1->Ndf()&& i<3){
				deta_sb_p1->at(i,j)->Fit(f1,"S", "", xmin, xmax);
				float integ = f1->GetParameter(0);
				jtc::scaleTF1(f1,1.0/integ, 5);
				//float integ = jsig_p1->at(i,j)->Integral(ncl,ncr)/(ncr-ncl+1);
				//jsig_p1->at(i,j)->Scale(integ);
				jtc::scale_Y_TF1((TH2*)jsig_p1->at(i,j), f1);
				c1->Update();
			}else {
				tx.DrawLatexNDC(0.3, 0.5, "#color[2]{Abandon}");
			}
		}
	}
	c1->SaveAs(out_plot+"/proc_sbcorr_"+_name+format);
}


Double_t fs1(Double_t *x, Double_t *p){
	if(x[0]< 0 ) return p[0]+x[0]*p[1]+p[3]*pow(x[0],2)+p[5]*pow(x[0],3);
	else return p[0]+x[0]*p[2]+p[4]*pow(x[0],2)+p[6]*pow(x[0],3);
}

Double_t fsExp(Double_t *x, Double_t *p){
	if(x[0]<0) return p[0]+p[2]/(x[0]-1)+p[4]*x[0];
	else return p[0]+p[1]/(x[0]+1)+p[3]*x[0];
}
//Double_t fsExp(Double_t *x, Double_t *p){
//	if(x[0]<0) return p[0]+p[2]*TMath::Exp(x[0]*p[4])+p[6]*x[0]*x[0];
//	else return p[0]+p[1]*TMath::Exp(x[0]*p[3])+p[5]*x[0]*x[0];
//}
//Double_t fsExp(Double_t *x, Double_t *p){
//	if(x[0]<0) return p[0]+p[2]*TMath::Exp(x[0]*p[4])+p[6]*x[0];
//	else return p[0]+p[1]*TMath::Exp(x[0]*p[3])+p[5]*x[0];
//}
#endif
