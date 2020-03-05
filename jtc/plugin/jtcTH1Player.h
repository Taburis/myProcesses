
#ifndef JTCTH1PLAYER_H
#define JTCTH1PLAYER_H
//#include "edmJtcUtility.h"
#include "TString.h"
#include "myProcesses/jtc/plugin/matrixTH1Ptr.h"
#include "myProcesses/jtc/plugin/jtcError.h"
#include "myProcesses/jtc/plugin/jtcLib.h"

class jtcTH1Player : public matrixTH1Ptr{
	// to use this class, a configure file needs to be loaded in g_cfg;
	public : jtcTH1Player (): matrixTH1Ptr(){};
		 virtual ~jtcTH1Player(){};
		 jtcTH1Player (const char *_name, int n = 6, int m=1): matrixTH1Ptr(_name, n,m) {}
		 jtcTH1Player (const char *_name, jtcTH1Player &j2): matrixTH1Ptr(_name, j2) {}
		 jtcTH1Player (jtcTH1Player &j2): matrixTH1Ptr(j2){}
		 jtcTH1Player * phiSideband(float x, float y, bool dorebin = 1, TString opt = "");
		 //				 void add_frac_error(float frac);
		 void ring_corr(matrixTH1Ptr * m2, float drmax = -1, bool check = 0);
		 void ringCorr();
		 jtcTH1Player* bkgSub(const char * name, float side1 = 1.5, float side2 = 2.5);
		 //				 jtcTH1Player* getSideMixTable(const char *name, float sidemin, float sidemax);
		 jtcTH1Player* drIntegral(const char *name, int ndr=jtc_default::ndrbin, float *drbins=jtc_default::drbins);
		 jtcTH1Player* projX(const char * name, float a0 , float a1 , TString opt="e", bool dorebin = 1);
		 jtcTH1Player* projY(const char * name, float a0 , float a1 , TString opt="e", bool dorebin = 1);
		 jtcTH1Player* getSignal_ME_based(const char *name, float sidemin, float sidemax, bool );
		 jtcTH1Player* contractX(const char *name);
		 jtcTH1Player* contractY(const char *name);
		 jtcTH1Player* rotate2D(const char* name);
		 jtcTH1Player* prepareMixTable(const char* name, bool dosmooth = 1);
		 void duplicateX(TString name, int n);
		 void duplicateY(TString name, int n);
		 void doChi2Test(jtcTH1Player *, Option_t* opt);
		 //				 void doGeoCorr(jtcTH1Player* j2);
		 void bin_size_normalization();
		 void setDrError();
		 void setDrError(jtcTH1Player *j2);
		 jtcTH1Player* getBkgError();
		 void drawBkgErrorCheck(int i, int j);
		 float bkgError(int i, int j){return m2error->at(i,j)->bg_err;}
		 float meError(int i, int j){return m2error->at(i,j)->me_err;}
		 float bkgLevel(int i, int j){return m2error->at(i,j)->bkg_level;}
		 bool loadBkgError(jtcTH1Player * );
		 void rebinX(int n){
			 for(int j=0; j<Ncol(); ++j){ for(int i=0; i<Nrow(); i++){
				 at(i,j)->RebinX(n);
			 }}
		 }
		 //				 void set2DConst(float x);
		 void absError(float x);

	public : bool kGotError = 0;
		 matrixPtrHolder<jtcError>* m2error = nullptr;
};

jtcTH1Player* jtcTH1Player::phiSideband(float x, float y,bool dorebin, TString opt){
	auto m2 = new jtcTH1Player(Form("phisb_%s",this->getName()), nrow, ncol);
	for(auto i=0; i<ref.size() ; ++i) {
		auto h = jtc::projX(dorebin, (TH2D*)ref[i], x, y, opt);
		(m2->ref).at(i) = (TH1*)h;
	}
	return m2;
}

void jtcTH1Player::ring_corr(matrixTH1Ptr * m2, float drmax, bool errCheck){
	for(int j=0; j<matrixTObjPtr<TH1>::ncol; ++j){
		for(int i=0; i<matrixTObjPtr<TH1>::nrow; i++){
			auto h = at(i,j);
			auto corr = m2->at(i,j);
			for(int k=1; k<h->GetNbinsX()+1; ++k){
				for(int l=1; l<h->GetNbinsY()+1; ++l){
					float deta = h->GetXaxis()->GetBinCenter(k);
					float dphi = h->GetYaxis()->GetBinCenter(l);
					float dr = pow(deta*deta+dphi*dphi, 0.5);
					if(dr > drmax && drmax > 0) continue;
					int nn = corr->GetXaxis()->FindBin(dr);
					if(errCheck){
						float error = corr->GetBinError(nn);
						if( fabs(corr->GetBinContent(nn)-1)<error) continue; 
					}
					h->SetBinContent(k,l, h->GetBinContent(k,l)/corr->GetBinContent(nn));
				}
			}
		}
	}
	return ;
}

jtcTH1Player * jtcTH1Player::bkgSub(const char * name, float side1, float side2){
	// this function will return the bkg
	jtcTH1Player * bkg2 = new jtcTH1Player(Form("bkg_%s",this->getName()), Nrow(), Ncol());
	jtcTH1Player * res = (jtcTH1Player*) this->clone(name);
	for(int j=0; j<matrixTObjPtr<TH1>::ncol; ++j){
		for(int i=0; i<matrixTObjPtr<TH1>::nrow; i++){
			auto bkg = (TH2D*) jtc::getV2Bkg((TH2D*)this->at(i,j), side1, side2);
			res->at(i,j)->Add(res->at(i,j), bkg, 1, -1);
			bkg2->add(bkg, i, j);
		}
	}
	bkg2->doFree = 1;
	delete bkg2;
	return res;
}

jtcTH1Player * jtcTH1Player::projX(const char * name, float a0 , float a1 , TString opt, bool dorebin){
	jtcTH1Player *m2p = new jtcTH1Player(name, *this);
	for(int j=0; j<Ncol(); ++j){
		for(int i=0; i<Nrow(); i++){
			auto h = jtc::projX(dorebin, (TH2D*)this->at(i,j), a0, a1, opt);
			m2p->add(h, i, j);
		}
	}
	m2p->setName(name);
	return m2p;
}

jtcTH1Player * jtcTH1Player::projY(const char * name, float a0 , float a1 , TString opt, bool dorebin){
	jtcTH1Player *m2p = new jtcTH1Player(name, *this);
	for(int j=0; j<Ncol(); ++j){
		for(int i=0; i<Nrow(); i++){
			auto h = jtc::projY(dorebin, (TH2D*)this->at(i,j), a0, a1, opt);
			m2p->add(h, i, j);
		}
	}
	m2p->setName(name);
	return m2p;
}

jtcTH1Player* jtcTH1Player::getBkgError(){
	auto j2 = projX(Form("proX_%s", getName()), -1, 0.99);
	m2error = new matrixPtrHolder<jtcError>(j2->Nrow(), j2->Ncol());
	for(int j=0; j<Ncol(); ++j){
		for(int i=0; i<Nrow(); i++){
			auto er = new jtcError();
			er->getBkgError((TH1D*)j2->at(i,j));
			m2error->add(er, i,j);
		}
	}
	j2->m2error = m2error;
	kGotError = 1;
	return j2;
}

jtcTH1Player* jtcTH1Player::drIntegral(const char* name, int ndr, float *drbins){
	auto j2dr = new jtcTH1Player(name, *this);
	for(int j=0; j<Ncol(); ++j){
		for(int i=0; i<Nrow(); i++){
			auto hdr = jtc::doDrIntegral(Form("%s_%d_%d",name,i,j), (TH2D*) at(i,j), ndr, drbins);
			j2dr->add(hdr, i, j);
		}
	}
	j2dr->doFree = 1;
	//		j2dr->setName(name);
	return j2dr;
}


void jtcTH1Player::setDrError(){
	if( !kGotError ){ 
		std::cout<<"please load or get the error info. for "<<getName()<<" first!"<<std::endl;
		return;
	}
	for(int j=0; j<Ncol(); ++j){
		for(int i=0; i<Nrow(); i++){
			m2error->at(i,j)->makeDrError(this->at(i,j));
		}
	}
	return;
}
/*
   void jtcTH1Player::add_frac_error(float frac){
   for(int j=0; j<Ncol(); ++j){
   for(int i=0; i<Nrow(); i++){
   jtc::add_frac_error((TH1*)this->at(i,j), frac);
   }
   }
   return;
   }
   */

void jtcTH1Player::setDrError(jtcTH1Player *j2){
	loadBkgError(j2);
	setDrError();
}

bool jtcTH1Player::loadBkgError(jtcTH1Player * j2){
	m2error = j2->m2error;
	kGotError = j2->kGotError;
	return kGotError;
}

/*
   jtcTH1Player* jtcTH1Player::getSideMixTable(const char *name, float sidemin, float sidemax){
   auto me = new jtcTH1Player(name, *this);
   for(int j=0; j<Ncol(); ++j){
   for(int i=0; i<Nrow(); i++){
   auto hme = jtc::sideBandMixingTableMaker((TH2D*)this->at(i,j), sidemin, sidemax);
   hme->SetName(Form("%s_%d_%d", name, i, j));
   me->add(hme, i, j);
   }
   }
   me->doFree = 1;
   return me;
   }
   */
void jtcTH1Player::bin_size_normalization(){
	for(int j=0; j<Ncol(); ++j){
		for(int i=0; i<Nrow(); i++){
				//float w1 = this->at(i,j)->GetXaxis()->GetBinWidth(1);
				//float w2 = this->at(i,j)->GetYaxis()->GetBinWidth(1);
				//this->at(i,j)->Scale(1.0/w1/w2);
			divide_bin_size((TH2*) this->at(i,j));
		}
	}
}
/*
   jtcTH1Player* jtcTH1Player::getSignal_ME_based(const char *name, float sidemin, float sidemax, bool doBkgSub){
   auto me = this->getSideMixTable(Form("%s_sideME", name), sidemin, sidemax);
   auto step1 = (jtcTH1Player*)((*this)/(*me));
   jtcTH1Player* res;
   if(doBkgSub){
   auto step2 = step1->bkgSub(name, 1.5, 2.5);
   res = step2;
   delete step1;
   delete me;
   } else {
   res = step1; 
   res->setName(name);
   delete me;
   }
   return res;
   }
   */
/*
   void jtcTH1Player::doGeoCorr(jtcTH1Player* j2){
   auto corr = jtc::drGeoTest((TH2D*) j2->at(0,0), (TH1D*) this->at(0,0));
   for(int j=0; j<Ncol(); ++j){
   for(int i=0; i<Nrow(); i++){
   at(i,j)->Divide(corr);
   }
   }
   }
   */

jtcTH1Player* jtcTH1Player::contractY(const char *name){
	jtcTH1Player * jc = new jtcTH1Player(name, 1 , Ncol());
	for(int j=0; j<Nrow(); ++j){
		auto h = (TH1*) at(j,0)->Clone(Form("%s_%d_0", name, j));
		jc->add(h, 0, j);
		for(int i=1; i<Ncol(); i++){
			//cout<<i<<" : "<<at(i,j)->GetName()<<endl;
			h->Add(at(j,i));
		}
	}
	return jc;

}

jtcTH1Player* jtcTH1Player::contractX(const char *name){
	jtcTH1Player * jc = new jtcTH1Player(name, 1 , Ncol());
	for(int j=0; j<Ncol(); ++j){
		auto h = (TH1*) at(0,j)->Clone(Form("%s_0_%d", name, j));
		jc->add(h, 0, j);
		for(int i=1; i<Nrow(); i++){
			//cout<<i<<" : "<<at(i,j)->GetName()<<endl;
			h->Add(at(i,j));
		}
	}
	return jc;
}

void jtcTH1Player::doChi2Test(jtcTH1Player *j2, Option_t* opt){
	Double_t *par = 0;
	m2error = new matrixPtrHolder<jtcError>(j2->Nrow(), j2->Ncol());
	for(int j=0; j<Ncol(); ++j){
		for(int i=0; i<Nrow(); i++){
			auto er = new jtcError();
			m2error->add(er, i,j);
			m2error->at(i,j)->chi2 = at(i,j)->Chi2Test(j2->at(i,j), opt, par);
			std::cout<<"for ("<<i<<", "<<j<<") p value: "<<m2error->at(i,j)->chi2<<std::endl;
		}
	}
	return ;
}

//void jtcTH1Player::set2DConst(float x){
//		for(int j=0; j<Ncol(); ++j){
//				for(int i=0; i<Nrow(); i++){
//						jtc::set2DConst(x,(TH1*)at(i,j));
//				}
//		}
//}

void jtcTH1Player::absError(float x){
	for(int j=0; j<Ncol(); ++j){
		for(int i=0; i<Nrow(); i++){
			auto h = at(i,j);
			for(int k=1; k<h->GetNbinsX()+1; k++){
				h->SetBinError(k, x);
			}
		}
	}
}

jtcTH1Player* jtcTH1Player::rotate2D(const char* name){
	auto m2new = new jtcTH1Player(name, this->Nrow(), this->Ncol());
	for(int j=0; j<Ncol(); ++j){
		for(int i=0; i<Nrow(); i++){
			auto h = jtc::rotate2D(Form("%s_%d_%d", name, i,j), (TH2D*)at(i,j));
			m2new->add(h, i,j);
		}
	}
	return m2new;
}

jtcTH1Player* jtcTH1Player::prepareMixTable(const char* name, bool dosmooth){
	auto m2new = new jtcTH1Player(name, this->Nrow(), this->Ncol());
	for(int j=0; j<Ncol(); ++j){
		for(int i=0; i<Nrow(); i++){
			m2new->add(jtc::mixingTableMaker((TH2D*)at(i,j), dosmooth), i,j);
		}
	}
	return m2new;
}

void jtcTH1Player::duplicateY(TString name, int ncol){
	int nr=Nrow(), nc= Ncol();
	if(nc != 1) {
		std::cout<<"Abort: duplicateX can only be done when size of column is 1."<<std::endl;
		return;
	}
	std::vector<TH1*> temp; temp.swap(ref);
	matrixObjectHolder<TH1*>::_setup_(Nrow(), ncol);
	for(int i=0; i<Nrow(); ++i){
		temp.at(i)->SetName(name+Form("_%d_%d",i,0));
		auto hsame = temp.at(i);
		add(hsame, i,0);
		for(int j=1; j<Ncol(); ++j){
			auto h = (TH1*) hsame->Clone(name+Form("_%d_%d", i,j));
			add(h, i, j);
		}
	}
}

void jtcTH1Player::duplicateX(TString name, int nrow){
	int nr=Nrow(), nc= Ncol();
	if(nr != 1) {
		std::cout<<"Abort: duplicateY can only be done when size of row is 1."<<std::endl;
		return;
	}
	std::vector<TH1*> temp; temp.swap(ref);
	matrixObjectHolder<TH1*>::_setup_(nrow, Ncol());
	for(int j=0; j<Ncol(); ++j){
		temp.at(j)->SetName(name+Form("_%d_%d",0,j));
		auto hsame = temp.at(j);
		add(hsame, 0,j);
		for(int i=1; i<Nrow(); ++i){
			auto h = (TH1*) hsame->Clone(name+Form("_%d_%d", i,j));
			add(h, i, j);
		}
	}
}
#endif
