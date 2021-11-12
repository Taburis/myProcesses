#ifndef bJTCFORMAT2SIGNALPRODUCER_H
#define bJTCFORMAT2SIGNALPRODUCER_H
//#include "myProcesses/jtc/plugin/seagullFitter.h"
#include "myProcesses/jtc/plugin/jtcSignalProducer.h"

Double_t fs1(Double_t *x, Double_t *par);
Double_t fsExp(Double_t *x, Double_t *par);
Double_t fLaurent(Double_t *x, Double_t *par);
Double_t fLaurent2(Double_t *x, Double_t *par);

double seagullPolyExp(double *x, double *par){
  if(x[0] >= 0) return par[0]+par[1]*TMath::Exp(x[0]*par[2])+par[3]*x[0]*x[0];
  return par[0]+par[4]*TMath::Exp(x[0]*par[5])+par[6]*x[0]*x[0];
}

double seagullExp(double *x, double *par){
  if(x[0] >= 0) return par[0]+par[1]*TMath::Exp(x[0]*par[2])+par[5]*x[0];
  return par[0]+par[3]*TMath::Exp(x[0]*par[4])+par[5]*x[0];
}

class bjtcFormat2SignalProducer : public jtcSignalProducer{
	public : bjtcFormat2SignalProducer(TString name0, int npt, int ncent):jtcSignalProducer(name0,npt,ncent){
		 }
		 ~bjtcFormat2SignalProducer(){}
		 virtual void produce() override;
		 void makeMixTable();
		 bool sb_check(TH1*, TH1*,  TF1* f);
		 virtual void sb_correction(jtcTH1Player *j2) override;
		 void sb_smth_correction(jtcTH1Player *j2) ;
		 virtual void sb_correction2(jtcTH1Player *j2);
		 float sb_chi2Test(TH1* sig, TH1* sb);
		 jtcTH1Player* rebin(TString name, jtcTH1Player* js);

		 virtual void loadSig(TString name, TFile *f)override{
			 fsig = f;
			 jrs = new jtcTH1Player(name, n1, n2); jrs->autoLoad(f);
			 if(doIntegral){
				 jrs = (jtcTH1Player*) jrs->contractX(name+"_pTsum");
			 }
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
			 if(doIntegral){
				 jmix = (jtcTH1Player*) jmix->contractX(name+"_pTsum");
			 }
		 }	

		 ParaSet *ps;
		 bool dorebin = 0; 		
		 bool doIntegral = 0; 		
		 TString *centLabels, *ptLabels;
};

void bjtcFormat2SignalProducer::makeMixTable(){
	jmix_p1 = new jtcTH1Player(_name+"_mixing_p1",jrs->Nrow(),jrs->Ncol());
	//cout<<"nrow: "<<jrs->Nrow()<<" , ncol: "<<jrs->Ncol()<<endl;
	int ptStart = 1;
	for(int i=0; i<jmix_p1->Nrow(); ++i){
		for(int j=0; j<jmix_p1->Ncol(); ++j){
			bool dosmooth = ptStart < i? 1: 0;
			//cout<<i<<": "<<dosmooth<<"_------------------------"<<endl;
			if(usingSbMixing) 
			jmix_p1->add(jtc::sideBandMixingTableMaker((TH2D*)jrs->at(i,j),1.6,2.),i,j);
			else 
			jmix_p1->add(jtc::mixingTableMaker((TH2D*)jmix->at(i,j),dosmooth),i,j);
		}
	}
	jmix_p1->setName(_name+"_mixing_p1_*_*");
}

void bjtcFormat2SignalProducer::produce(){
	sb_ymin = 1.6;
	sb_ymax = 2.2;
	//sb_ymin = 1.8;
	//sb_ymax = 2.4;
	if(doIntegral) this->n1 = 1;
	jrs->bin_size_normalization();
	//jmix_p1 = jmix->prepareMixTable(_name+"_mixing_p1", dosmooth);
	//cout<<"here"<<endl;
	makeMixTable();
	cout<<"nrow: "<<jrs->Nrow()<<" , ncol: "<<jrs->Ncol()<<endl;
	cout<<"nrow: "<<jmix_p1->Nrow()<<" , ncol: "<<jmix_p1->Ncol()<<endl;
	jsig_p1 = (jtcTH1Player*)((matrixTH1Ptr*)jrs)->divide(*jmix_p1);
	cout<<"nrow: "<<jsig_p1->Nrow()<<" , ncol: "<<jsig_p1->Ncol()<<endl;
	jsig_p1 ->setName(_name+"_sig_p1_*_*");
	if(doSbCorrection) {
		// two sb correction methods are used for different pt bins
		//sb_correction2(jsig_p1);
		sb_correction(jsig_p1);
		//sb_smth_correction(jsig_p1);
	}
	jsig_p2 = jsig_p1->bkgSub(_name+"_sig_p2", 1.5, 2.5);
	jdr_sig_p0 = jrs->drIntegral(_name+"_sig_p0_dr");
	jdr_sig_p1 = jsig_p1->drIntegral(_name+"_sig_p1_dr");
	jdr_sig_p2 = jsig_p2->drIntegral(_name+"_sig_p2_dr");
	plot_slice_sideband();
	debug();
	//debug_mixing();
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
void bjtcFormat2SignalProducer::sb_smth_correction(jtcTH1Player *j2){
	float xmin = -2.2, xmax = 2.2, centerleft = -0.03, centerright = 0.03;
	deta_sb_p1 = j2->projX(_name+"_sig_deta_p1_*_*", sb_ymin, sb_ymax, "e", 0);
	for(int j=0; j<deta_sb_p1->Nrow(); j++){
		for(int k=0; k<deta_sb_p1->Ncol(); k++){
			deta_sb_p1->at(j,k)->SetAxisRange(xmin, xmax, "X");
			deta_sb_p1->at(j,k)->Smooth(3, "R");
		}
	}
	for(int i=0; i<3;++i){
		for(int j=0; j<n2;++j){
			//cout<<"smoothing "<<i<<" , "<<j<<endl;
			auto h = deta_sb_p1->at(i,j);
			auto h2= j2->at(i,j);
			int n0 = h->FindBin(centerleft);
			int n1 = h->FindBin(centerright);
			int nx0 = h->GetXaxis()->FindBin(xmin);
			int nx1 = h->GetXaxis()->FindBin(xmax);
			double scale = h->Integral(n0,n1)/(n1-n0+1);
			int nn = 0;
			if(h->Integral() ==0){
				cout<<"side band is empty, please extend the SB range!"<<endl;
				continue;
			}
			while(scale==0 && nn < 5) {
				n0-=10;
				n1+=10;
				nn++;
				scale = double(h->Integral(199,302))/(n1-n0+1);
				//cout<<"loop agian: "<<n0<<" , "<<n1<<": "<<scale<<endl;
			}
			//h->Scale(1.0/scale);
			cout<<"(sb normalization, should be around 1) center value: "<<h->GetBinContent(h->FindBin(0))/scale<<endl;
			for(int k=1; k<h->GetNbinsX()+1; k++){
				if( k<nx0 || k> n1) continue;
				if( k>=n0 && k<=n1) continue;
				double cc = h->GetBinContent(k)/scale;
				if (cc ==0) continue;
				for(int l=1; l<h2->GetNbinsY()+1; l++){
					double c0 = h2->GetBinContent(k,l);
					double e0 = h2->GetBinError(k,l);
					h2->SetBinContent(k,l, c0/cc);
					h2->SetBinError(k,l, e0/cc);
				}
			}	
		}
	}
}
void bjtcFormat2SignalProducer::sb_correction2(jtcTH1Player *j2){
	float xmin = -2.8, xmax = 2.8, centerleft = -0.1, centerright = 0.1;
	deta_sig_p1 = jsig_p1->projX(_name+"_sig_deta_p1_*_*", -1, 1, "e", 0);
	deta_sb_p1 = jsig_p1->projX(_name+"_sb_deta_p1_*_*", sb_ymin, sb_ymax, "e", 0);
	auto fLau = new TF1("fexp", fLaurent2, xmin, xmax, 5);
	auto fpol1 = new TF1("fpol1", "pol0",  xmin, xmax);
	auto fpol2 = new TF1("fpol2", "pol2", xmin, xmax);
	TF1* fcand;
	deta_sig_p1->scale(1.0/2);
	deta_sb_p1->scale(1.0/(sb_ymax-sb_ymin));
	auto c = new plotManager();
	c->initSquarePad(_name+"_c_deta_sig_p1", "", n1, n2);
	c->addm2TH1(deta_sb_p1, "", "", 1);
	c->setXrange(-3.5,3.499);
	//c->draw();
	for(int i=0; i<n1;++i){
		for(int j=0; j<n2;++j){
			deta_sig_p1->at(i,j)->Rebin(5);
			deta_sb_p1->at(i,j)->Rebin(5);
		}
	}
	//c->addm2TH1(deta_sb_p1);
	TLatex tx; tx.SetTextSize(.12);
	// 
	for(int i=0; i<n1;++i){
		for(int j=0; j<n2;++j){
			TH1* hsig = deta_sig_p1->at(i,j);
			TH1* hsb  = deta_sb_p1->at(i,j);
			//hsb->Smooth(1);
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
}

void bjtcFormat2SignalProducer::sb_correction(jtcTH1Player *j2){
	float xmin = -2.5, xmax = 2.5, centerleft = -0.1, centerright = 0.1;
	//float xmin = -2.6, xmax = 2.59, centerleft = -0.1, centerright = 0.1;
	deta_sig_p1 = jsig_p1->profileX(_name+"_sig_deta_p1_*_*", -1, 1, "e");
	deta_sb_p1 = jsig_p1->profileX(_name+"_sb_deta_p1_*_*", sb_ymin, sb_ymax, "e");
	//auto fLau = new TF1("seagullFitExp",seagullExp,-3,3,6);
	//auto fLau = new TF1("fexp", fLaurent2, xmin, xmax, 3);
	auto fLau = new TF1("fexp", fLaurent, xmin, xmax, 6);
	auto fpol1 = new TF1("fpol1", "pol0",  xmin, xmax);
	auto fpol2 = new TF1("fpol2", "pol2", xmin, xmax);
	TF1* fcand;
	//deta_sig_p1->scale(1.0/2);
	//deta_sb_p1->scale(1.0/(sb_ymax-sb_ymin));
	auto c = new plotManager();
	c->initSquarePad(_name+"_c_deta_sig_p1", "", n1, n2);
	c->addm2TH1(deta_sb_p1, "", "", 1);
	c->setXrange(-3.5,3.499);
	//c->draw();
	for(int i=0; i<n1;++i){
		for(int j=0; j<n2;++j){
			deta_sig_p1->at(i,j)->Rebin(10);
			deta_sb_p1->at(i,j)->Rebin(10);
		}
	}
	//c->addm2TH1(deta_sb_p1);
	TLatex tx; tx.SetTextSize(.12);
	// 
	for(int i=0; i<n1;++i){
		for(int j=0; j<n2;++j){
			TH1* hsig = deta_sig_p1->at(i,j);
			TH1* hsb  = deta_sb_p1->at(i,j);
			//hsb->Smooth(1);
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
			//double initialLevel = hsb->GetBinContent(hsb->FindBin(0));
			//fLau->SetParameters(initialLevel,-1,-1,-1,1); 
			auto ptr2 = hsb->Fit(fLau,"S", "", xmin, xmax);
			float chi2ndof_Lau = ptr2->Chi2();
			//float chi2ndof_Lau = ptr2->Chi2()/ptr2->Ndf();
			float range = 0.1;
			float center = th1_ave_content(deta_sb_p1->at(i,j), -range, range);
			float dis = th1_ave_error(deta_sb_p1->at(i,j), -range, range);
			float left = fLau->Eval(-1.2);
			float right = fLau->Eval(1.2);
			float scale = .7;
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
Double_t fLaurent2(Double_t *x, Double_t *p){
	if(x[0]<0) return p[0]+p[1]/(x[0]-1);
	else return p[0]-p[2]/(x[0]+1);
	//if(x[0]<0) return p[0]+p[1]/(x[0]-1)+p[2]*x[0];
	//else return p[0]-p[1]/(x[0]+1)+p[3]*x[0];
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
