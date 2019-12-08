
#ifndef MATRIXTH1PTR_H
#define MATRIXTH1PTR_H
#include "myProcesses/jtc/plugin/matrixTObjPtr.h"
class matrixTH1Ptr : public matrixTObjPtr<TH1>{
		public : matrixTH1Ptr(): matrixTObjPtr<TH1>(){};
				 virtual ~matrixTH1Ptr(){ };
				 matrixTH1Ptr(const char * _name, int n, int m): matrixTObjPtr<TH1>(_name, n,m) {}
				 matrixTH1Ptr(matrixTH1Ptr& m2): matrixTObjPtr<TH1>(TString("cl_"+m2.name), m2.Nrow(), m2.Ncol()) {}
				 matrixTH1Ptr(const char * name, matrixTH1Ptr& m2): matrixTObjPtr<TH1>(name, m2.Nrow(), m2.Ncol()) {}
				 void setName(const char* _name) {
						 name = _name;
						 for(int j=0; j<matrixTObjPtr<TH1>::ncol; ++j){
								 for(int i=0; i<matrixTObjPtr<TH1>::nrow; i++){
										 TString hn = name+Form("_%d_%d", i, j);
										 at(i,j)->SetName(hn);
								 }
						 }
				 }
				 void scale(float s) {
						 for(int j=0; j<matrixTObjPtr<TH1>::ncol; ++j){
								 for(int i=0; i<matrixTObjPtr<TH1>::nrow; i++){
										 at(i,j)->Scale(s);
								 }
						 }
				 }
				 void setAxisRange(float x0, float x1, TString ax){
						 for(int j=0; j<matrixTObjPtr<TH1>::ncol; ++j){
								 for(int i=0; i<matrixTObjPtr<TH1>::nrow; i++){
										 at(i,j)->SetAxisRange(x0, x1, ax);
								 }
						 }
				 }
				 matrixTH1Ptr * clone(const char * newname){
						 auto m2 = new matrixTH1Ptr(newname, matrixTObjPtr<TH1>::nrow, matrixTObjPtr<TH1>::ncol);
						 for(int j=0; j<matrixTObjPtr<TH1>::ncol; ++j){
								 for(int i=0; i<matrixTObjPtr<TH1>::nrow; i++){
										 TString hn = name+Form("_%d_%d", i, j);
										 auto hh = (TH1*) at(i,j)->Clone(hn);
										 m2->add(hh, i, j);
								 }
						 }
						 m2->doFree = 1;
						 return m2;
				 }
				 matrixTH1Ptr * operator+( matrixTH1Ptr & rhs){
						 //check if the matrix shapes are the same
						 if( matrixTObjPtr<TH1>::ncol != rhs.ncol || matrixTObjPtr<TH1>::nrow != rhs.nrow) return 0;
						 auto m2 = clone((std::string("sum_")+name).c_str());
						 for(int j=0; j<matrixTObjPtr<TH1>::ncol; ++j){
								 for(int i=0; i<matrixTObjPtr<TH1>::nrow; i++){
										 m2->at(i,j)->Add(rhs(i,j));
								 }
						 }
						 return m2;
				 }
				 matrixTH1Ptr * operator-( matrixTH1Ptr & rhs){
						 //check if the matrix shapes are the same
						 if( matrixTObjPtr<TH1>::ncol != rhs.ncol || matrixTObjPtr<TH1>::nrow != rhs.nrow) return 0;
						 auto m2 = clone((std::string("sum_")+name).c_str());
						 for(int j=0; j<matrixTObjPtr<TH1>::ncol; ++j){
								 for(int i=0; i<matrixTObjPtr<TH1>::nrow; i++){
										 m2->at(i,j)->Add(rhs(i,j), -1);
										 //m2->at(i,j)->Add(m2->at(i,j), rhs(i,j), 1, -1);
								 }
						 }
						 return m2;
				 }

				 matrixTH1Ptr * divide(matrixTH1Ptr &rhs){
						 if( matrixTObjPtr<TH1>::ncol != rhs.ncol || matrixTObjPtr<TH1>::nrow != rhs.nrow) return 0;
						 auto m2 = clone((std::string("division_")+name).c_str());
						 for(int j=0; j<matrixTObjPtr<TH1>::ncol; ++j){
								 for(int i=0; i<matrixTObjPtr<TH1>::nrow; i++){
										 m2->at(i,j)->Divide(rhs(i,j));
								 }
						 }
						 return m2;
				 }

				 matrixTH1Ptr * operator/( matrixTH1Ptr & rhs){
						 return divide(rhs);
				 }

				 matrixTH1Ptr * operator%( matrixTH1Ptr & rhs){
						 if( matrixTObjPtr<TH1>::ncol != rhs.ncol || matrixTObjPtr<TH1>::nrow != rhs.nrow) return 0;
						 auto m2 = clone((std::string("bdivided_")+name).c_str());
						 for(int j=0; j<matrixTObjPtr<TH1>::ncol; ++j){
								 for(int i=0; i<matrixTObjPtr<TH1>::nrow; i++){
										 m2->at(i,j)->Divide(m2->at(i,j), rhs(i,j), 1, 1, "B");
								 }
						 }
						 return m2;
				 }
				 void normalized_by_area(float x = 1, float y = -1){
						 for(int j=0; j<matrixTObjPtr<TH1>::ncol; ++j){
								 for(int i=0; i<matrixTObjPtr<TH1>::nrow; i++){
										 auto h = at(i,j);
										 float area;
										 if(x< y){
												 int n1 = h->GetXaxis()->FindBin(x);
												 int n2 = h->GetXaxis()->FindBin(y);
												 area = h->Integral(n1, n2, "width");
										 }else area = h->Integral("width");
										 h->Scale(1.0/area);
								 }
						 }
						 return;
				 }
				 matrixTH1Ptr* rebin(const char* name, int nbins, const float *bins){
						 return rebin(name, nbins, (Double_t *) bins);
				 }

				 matrixTH1Ptr* rebin(const char* name, int nbins, const Double_t *bins){
						 auto m2new = new matrixTH1Ptr(name, this->Nrow(), this->Ncol());
						 for(int j=0; j<matrixTObjPtr<TH1>::ncol; ++j){
								 for(int i=0; i<matrixTObjPtr<TH1>::nrow; i++){
										 auto h = this->at(i,j);
										 for(int k=1; k< h->GetNbinsX()+1; ++k){
												 float w = h->GetBinWidth(k);
												 h->SetBinContent(k, h->GetBinContent(k)*w);
												 h->SetBinError(k, h->GetBinError(k)*w);
										 }
										 auto hnew = h->Rebin(nbins, Form("%s_%d_%d", name, i,j), bins);
										 m2new->add(hnew, i,j);
										 for(int k=1; k< hnew->GetNbinsX()+1; ++k){
												 float w = hnew->GetBinWidth(k);
												 hnew->SetBinContent(k, hnew->GetBinContent(k)/w);
												 hnew->SetBinError(k, hnew->GetBinError(k)/w);
										 }
								 }
						 }
						 return m2new;
				 }
				 void rebin(int nbins = 2){
						 for(int j=0; j<matrixTObjPtr<TH1>::ncol; ++j){
								 for(int i=0; i<matrixTObjPtr<TH1>::nrow; i++){
										 this->at(i,j)->Rebin(nbins);
								 }
						 }
				 }
				 void smooth(Int_t n = 1, Option_t * opt = ""){
						 for(int j=0; j<matrixTObjPtr<TH1>::ncol; ++j){
								 for(int i=0; i<matrixTObjPtr<TH1>::nrow; i++){
										 at(i,j)->Smooth(n, opt);
								 }
						 }
				 }
				 void setAxisRange(Double_t x0, Double_t x1, Option_t* axis){
						 for(int j=0; j<matrixTObjPtr<TH1>::ncol; ++j){
								 for(int i=0; i<matrixTObjPtr<TH1>::nrow; i++){
										 at(i,j)->SetAxisRange(x0, x1, axis);
								 }
						 }
				 }
				 /* 
					matrixTH1Ptr* rotate2D(const char* name){
					auto m2new = new matrixTH1Ptr(name, this->Nrow(), this->Ncol());
					for(int j=0; j<matrixTObjPtr<TH1>::ncol; ++j){
					for(int i=0; i<matrixTObjPtr<TH1>::nrow; i++){
					auto h = jtc_utility::rotate2D(Form("%s_%d_%d", name, i,j), (TH2D*)at(i,j));
					add(h, i,j);
					}
					}
					return m2new;
					}*/
};

template<typename T> 
class multi_canvas : public TCanvas{
		public: multi_canvas(matrixTObjPtr<T> & p): TCanvas(TString(p.name+"_canvas"), TString(p.name), int(p.ncol*350), int(p.nrow*325))
				{
						nrow= p.nrow;
						ncol= p.ncol;
						m2ptr = &p;
						Divide(ncol, nrow, 0, 0);
						//				for(int i=0; i< nrow; ++i){
						//						for(int j=0; j< ncol; ++j){
						//								CD(i,j);
						//								gPad->SetMargin(0., 0., 0., 0.);
						//								gPad->Update();
						//						}
						//				}
				}
				multi_canvas(TString name, TString title, int mrow, int mcol, float wd=325, float hi=325): TCanvas(name, title, mcol*wd, mrow*hi)
		{
				nrow= mrow;
				ncol= mcol;
				Divide(ncol, nrow);
				for(int i=0; i< nrow; ++i){
						for(int j=0; j< ncol; ++j){
								CD(i,j);
								gPad->SetMargin(0.15, 0.01, 0.15, 0.03);
								gPad->Update();
						}
				}
		}
				int flatten(int n, int m){
						if(n > nrow -1 || m > ncol -1 ) {
								std::cout<<" ==== ERROR: index ("<<n<<", "<<m<<") exceeds the range!!! current shape ["<<nrow<<", "<<ncol<<"] ===="<<std::endl;
								return 0;
						}
						//slightly different from the matrixPtrHolder, the row and column in canvas start from 1 and up, 0 stands for the whole pad.
						return n*ncol+m+1;
				}
				//CD(irow, icol);
				void CD(int n, int m){ this->cd(flatten(n,m));} 
				void draw(){
						this->SetMargin(0.5, 0.2, 0.4, 0.40);
						//gStyle->SetOptStat(0);
						for(int i=0; i< nrow; ++i){
								for(int j=0; j< ncol; ++j){
										cd(i,j);
										m2ptr(i,j)->Draw();
								}
						}
				}

		public:
				int nrow, ncol;
				matrixTObjPtr<T> *m2ptr;
};

#endif
