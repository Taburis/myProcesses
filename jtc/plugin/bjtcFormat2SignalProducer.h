#ifndef bJTCFORMAT2SIGNALPRODUCER_H
#define bJTCFORMAT2SIGNALPRODUCER_H
//#include "myProcesses/jtc/plugin/seagullFitter.h"
#include "myProcesses/jtc/plugin/jtcSignalProducer.h"

Double_t fs1(Double_t *x, Double_t *par);
Double_t fsExp(Double_t *x, Double_t *par);
Double_t fLaurent(Double_t *x, Double_t *par);
class bjtcFormat2SignalProducer : public jtcSignalProducer{
	public : bjtcFormat2SignalProducer(TString name0, int npt, int ncent):jtcSignalProducer(name0,npt,ncent){
		 }
		 ~bjtcFormat2SignalProducer(){}
		 virtual void produce() override;
		 void makeMixTable();
		 bool sb_check(TH1*, TH1*,  TF1* f);
		 virtual void sb_correction(jtcTH1Player *j2) override;
		 //		 virtual void sb_correction2(jtcTH1Player *j2);
		 float sb_chi2Test(TH1* sig, TH1* sb);
		 jtcTH1Player* rebin(TString name, jtcTH1Player* js);

		 virtual void loadSig(TString name, TFile *f)override{
			 fsig = f;
			 jrs = new jtcTH1Player(name, n1, n2); jrs->autoLoad(f);
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
			 jmix = new jtcTH1Player(name, n1, n2); jmix->autoLoad(f);
		 }	

		 ParaSet *ps;
		 bool dorebin = 0; 		
		 TString *centLabels, *ptLabels;
};

void bjtcFormat2SignalProducer::makeMixTable(){
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

void bjtcFormat2SignalProducer::produce(){
	jrs->bin_size_normalization();
	//jmix_p1 = jmix->prepareMixTable(_name+"_mixing_p1", dosmooth);
	cout<<"here"<<endl;
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
	sb_ymin = 1.8;
	sb_ymax = 2.4;
	float xmin = -3.3, xmax = 3.3, centerleft = -0.1, centerright = 0.1;
	deta_sig_p1 = jsig_p1->projX(_name+"_sig_deta_p1_*_*", -1, 1, "e", 0);
	deta_sb_p1 = jsig_p1->projX(_name+"_sb_deta_p1_*_*", sb_ymin, sb_ymax, "e", 0);
	auto fLau = new TF1("fexp", fLaurent, xmin, xmax, 5);
	auto fpol1 = new TF1("fpol1", "pol0",  xmin, xmax);
	auto fpol2 = new TF1("fpol2", "pol2", xmin, xmax);
	TF1* fcand;
	deta_sig_p1->scale(1.0/2);
	deta_sb_p1->scale(1.0/(sb_ymax-sb_ymin));
	auto c = new plotManager();
	c->initSquarePad(_name+"_c_deta_sig_p1", "", n1, n2);
	c->addm2TH1(deta_sb_p1, "", "", 1);
	c->setXrange(-3.2,3.19);
	//c->draw();
	for(int i=0; i<n1;++i){
		for(int j=0; j<n2;++j){
			deta_sig_p1->at(i,j)->Rebin(5);
			deta_sb_p1->at(i,j)->Rebin(5);
		}
	}
	//c->addm2TH1(deta_sb_p1);
	TLatex tx; tx.SetTextSize(.12);
	for(int i=0; i<n1;++i){
		for(int j=0; j<n2;++j){
			TH1* hsig = deta_sig_p1->at(i,j);
			TH1* hsb  = deta_sb_p1->at(i,j);
			deta_sb_p1->at(i,j)->GetXaxis()->SetTitle("#Delta#eta");
			hsig->SetLineColor  (kRed);
			hsig->SetMarkerColor(kRed);
			hsig->SetMarkerSize(0.8);
			hsig->SetMarkerStyle(20);
			hsb ->GetXaxis()->SetRangeUser(xmin,xmax);
			c->cd(i, n2-1-j);
			//if(sb_check(hsig,hsb, 0)) hsig->Draw("same");

			auto ptr0 = deta_sb_p1->at(i,j)->Fit(fpol1,"S0", "", xmin, xmax);
			auto ptr1 = deta_sb_p1->at(i,j)->Fit(fpol2,"S0", "", xmin, xmax);
			//float chi2ndof_pol1 = ptr0->Chi2();
			//float chi2ndof_pol2 = ptr1->Chi2();
			float chi2ndof_pol1 = ptr0->Chi2()/ptr0->Ndf();
			float chi2ndof_pol2 = ptr1->Chi2()/ptr1->Ndf();
			fLau ->SetParLimits(1, 0, 1000);
			auto ptr2 = deta_sb_p1->at(i,j)->Fit(fLau,"S", "", xmin, xmax);
			float chi2ndof_Lau = ptr2->Chi2();
			//float chi2ndof_Lau = ptr2->Chi2()/ptr2->Ndf();
			float range = 0.1;
			float center = th1_ave_content(deta_sb_p1->at(i,j), -range, range);
			float dis = th1_ave_error(deta_sb_p1->at(i,j), -range, range);
			float left = fLau->Eval(-1.2);
			float right = fLau->Eval(1.2);
			float scale = .6;
	cout<<"------------------------------------------------------"<<endl;
	cout<<left-center<<" : "<<dis<<endl;
	cout<<"------------------------------------------------------"<<endl;
			if((left-center > scale*dis || right-center > scale*dis) && i<4){
				fcand = fLau;
				float scale = fcand->Eval(0);
				jtc::scale_Y_TF1((TH2*)jsig_p1->at(i,j), fcand);
				tx.DrawLatexNDC(0.3, 0.8, "#color[1]{Piece-wise}");
				std::cout<<"Fitting adapted"<<std::endl;
			}
			else if(chi2ndof_pol1 > 1.1*chi2ndof_pol2){
				fcand = fpol2;
				float scale = fcand->Eval(0);
				jtc::scale_Y_TF1((TH2*)jsig_p1->at(i,j), fcand);
				tx.DrawLatexNDC(0.5, 0.8, "#color[1]{Pol2}");
				ptr1 = deta_sb_p1->at(i,j)->Fit(fpol2,"S", "", xmin, xmax);
				std::cout<<"Fitting adapted"<<std::endl;

			}else {
				tx.DrawLatexNDC(0.3, 0.5, "#color[2]{Abandon}");
				std::cout<<"Drop fitting"<<std::endl;
			}
		}
	}
#ifdef JTC_DEBUG
	c->save(out_plot+"/proc_sbcorr_"+_name+format);
#endif
	//	auto c = new multi_pads<base_pad>(_name+"_c_deta_sig_p1", "", n1, n2);
}

bool bjtcFormat2SignalProducer::sb_check(TH1* hsig, TH1* hsb, TF1* f){
	//return true if the sb failed the check
	hsig->GetXaxis()->SetRangeUser(-3.,-1.5);
	hsb ->GetXaxis()->SetRangeUser(-3.,-1.5);
	Double_t chi2; Int_t ndof, igood;
	Double_t chi20; Int_t ndof0;
	Double_t probe = hsig->Chi2TestX(hsb,chi2,ndof,igood, "WW");
	//Double_t probe = hsig->Chi2Test(hsb, "WWP");
	hsig->GetXaxis()->SetRangeUser(1.5,3);
	hsb ->GetXaxis()->SetRangeUser(1.5,3);
	probe+= hsig->Chi2TestX(hsb,chi20,ndof0, igood ,"WW");
	Double_t chi2ndof = (chi2+chi20)/(ndof+ndof0);
	probe=probe/2;
	cout<<"Chi2 value: "<<chi2ndof<<endl;
	hsig->GetXaxis()->SetRangeUser(-3.,3);
	hsb ->GetXaxis()->SetRangeUser(-3.,3);
	if(probe < 0.9) return true;
	return false;
}


Double_t fs1(Double_t *x, Double_t *p){
	if(x[0]< 0 ) return p[0]+x[0]*p[1]+p[3]*pow(x[0],2)+p[5]*pow(x[0],3);
	else return p[0]+x[0]*p[2]+p[4]*pow(x[0],2)+p[6]*pow(x[0],3);
}
Double_t fsExp(Double_t *x, Double_t *p){
	if(x[0]<0) return p[0]+p[2]*TMath::Exp(x[0]*p[4])+p[6]*x[0]*x[0];
	else return p[0]+p[1]*TMath::Exp(x[0]*p[3])+p[5]*x[0]*x[0];
}

Double_t fLaurent(Double_t *x, Double_t *p){
	if(x[0]<0) return p[0]+p[1]/(x[0]-1)+p[2]*x[0]+p[4]*x[0]*x[0];
	else return p[0]-p[1]/(x[0]+1)+p[3]*x[0]+p[5]*x[0]*x[0];
	//if(x[0]<0) return p[0]+1.0/(p[2]*x[0]-1)+p[4]*x[0];
	//else return p[0]-1.0/(p[1]*x[0]+1)+p[3]*x[0];
	//if(x[0]<0) return p[0]+1.0/(p[2]*x[0]-1)+p[4]*x[0]+p[6]*x[0]*x[0];
	//else return p[0]-1.0/(p[1]*x[0]+1)+p[3]*x[0]+p[5]*x[0]*x[0];
}
#endif
