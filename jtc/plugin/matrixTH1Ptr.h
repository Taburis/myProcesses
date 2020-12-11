
#ifndef MATRIXTH1PTR_H
#define MATRIXTH1PTR_H
#include "myProcesses/jtc/plugin/matrixTObjPtr.h"
#include "TDirectory.h"
class matrixTH1Ptr : public matrixTObjPtr<TH1>{
	public : matrixTH1Ptr(): matrixTObjPtr<TH1>(){};
		 virtual ~matrixTH1Ptr(){ };
		 matrixTH1Ptr(const char * _name, int n, int m): matrixTObjPtr<TH1>(_name, n,m) {}
		 matrixTH1Ptr(matrixTH1Ptr& m2): matrixTObjPtr<TH1>(TString("cl_"+m2.name), m2.Nrow(), m2.Ncol()) {}
		 matrixTH1Ptr(const char * name, matrixTH1Ptr& m2): matrixTObjPtr<TH1>(name, m2.Nrow(), m2.Ncol()) {}
		 void setName(const char* _name) {
			// a example: setName("histogram_P*_C*");
			TString hname = _name ;
//cout<<"format ------: "<<hname<<endl;
			hname.ReplaceAll("*","%d");
			 for(int j=0; j<matrixTObjPtr<TH1>::ncol; ++j){
				 for(int i=0; i<matrixTObjPtr<TH1>::nrow; i++){
					 TString hn = Form(hname, i, j);
//cout<<"renaming to-------: "<<hn<<endl;
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
					 TString hn = TString(newname)+Form("_%d_%d", i, j);
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
		 matrixTH1Ptr * add2(TString name,  matrixTH1Ptr & rhs, float c1, float c2){
			 //check if the matrix shapes are the same
			 if( matrixTObjPtr<TH1>::ncol != rhs.ncol || matrixTObjPtr<TH1>::nrow != rhs.nrow) return 0;
			 auto m2 = clone(name);
			 for(int j=0; j<matrixTObjPtr<TH1>::ncol; ++j){
				 for(int i=0; i<matrixTObjPtr<TH1>::nrow; i++){
					 //m2->at(i,j)->Add(rhs(i,j), -1);
					 m2->at(i,j)->Add(m2->at(i,j), rhs(i,j), c1, c2);
				 }
			 }
			 return m2;
		 }


		 matrixTH1Ptr * divide(matrixTH1Ptr &rhs, TString opt = ""){
			 if( this->Ncol() != rhs.Ncol() || this->Nrow()!= rhs.Nrow()) return 0;
			 auto m2 = clone((std::string("division_")+name).c_str());
			 for(int j=0; j<matrixTObjPtr<TH1>::ncol; ++j){
				 for(int i=0; i<matrixTObjPtr<TH1>::nrow; i++){
					 m2->at(i,j)->Divide(m2->at(i,j), rhs(i,j), 1, 1, opt);
				 }
			 }
			 return m2;
		 }

		 matrixTH1Ptr * multiply(matrixTH1Ptr &rhs){
			 if( matrixTObjPtr<TH1>::ncol != rhs.ncol || matrixTObjPtr<TH1>::nrow != rhs.nrow) return 0;
			 auto m2 = clone((std::string("multiply_")+name).c_str());
			 for(int j=0; j<matrixTObjPtr<TH1>::ncol; ++j){
				 for(int i=0; i<matrixTObjPtr<TH1>::nrow; i++){
					 m2->at(i,j)->Multiply(rhs.at(i,j));
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
		 void setDirectory(TDirectory* dir){
			 for(int j=0; j<matrixTObjPtr<TH1>::ncol; ++j){
				 for(int i=0; i<matrixTObjPtr<TH1>::nrow; i++){
					 at(i,j)->SetDirectory(dir);
				 }}
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

#endif
