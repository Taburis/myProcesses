
#ifndef MATRIXTOBJPTR_H
#define MATRIXTOBJPTR_H
#include <string>
#include <iostream>
#include "TFile.h"
#include "TCanvas.h"
#include "TH1.h"

template<typename T>
class matrixObjectHolder{
	public : matrixObjectHolder(){};
		 matrixObjectHolder(int n, int m){_setup_(n,m);}
		 virtual void _setup_(int n, int m){nrow = n, ncol=m, ref.resize(n*m);};
		 T at(int n, int m){return ref[flatten(n, m)];}
		 T operator()(int n, int m){ return at(n, m);}
		 int flatten(int n, int m){
			 if(n > nrow-1 || m > ncol -1 ) {
				 std::cout<<" ==== ERROR: index exceeds the range!!! current shape ["<<nrow<<", "<<ncol<<"] ===="<<std::endl;
				 return 0;
			 }
			 // the start index of row/column is 0 rather than 1
			 return n+m*nrow;}
		 void add(T ptr, int i, int j){
			 // ref.push_back(ptr);
			 ref[flatten(i,j)] = ptr;
		 }
		 int Ncol(){ return ncol; }
		 int Nrow(){ return nrow; }
		 void transpose(){
			 std::vector<T> tmp(ref);
			 for(int i=0; i<nrow; i++){
				 for(int j=0; j<ncol; j++){
					 //cout<<"i "<<i<<", j "<<j<<endl;
					 ref[j+i*ncol] = tmp[flatten(i,j)];
					 //cout<<tmp[flatten(i,j)]->GetName()<<endl;
				 }
			 }
			 tmp.clear();
			 int trans = nrow;
			 nrow = ncol;
			 ncol = trans;
		 }
		 void cleanAll(){for(auto & it: ref) delete it;}
		 std::vector<T> ref;
		 bool isValid(int i, int j){// return 1 means i, j is valid
			 return flatten(i,j) < size(); }
		 int size(){return nrow*ncol;};

		 int nrow, ncol;
};

template<typename T>
class matrixPtrHolder : public matrixObjectHolder<T*>{
	public : matrixPtrHolder(){};
		 matrixPtrHolder(int n, int m):matrixObjectHolder<T*>(n,m){}
		 virtual ~matrixPtrHolder(){};

};

template<typename T> 
class matrixTObjPtr : public matrixObjectHolder<T*>{
	public :
		matrixTObjPtr(): matrixObjectHolder<T*>(){};	
		matrixTObjPtr(const char * _name, int n, int m): matrixObjectHolder<T*>(n,m) {
			name = _name;
		};
		virtual ~matrixTObjPtr(){
			doFree = 0;
		};
		matrixTObjPtr * deep_clone(const char * name_){
			auto m2 = new matrixTObjPtr<T>();
			m2->matrixTObjPtr<T>::setup(name_, matrixObjectHolder<T*>::nrow, matrixObjectHolder<T*>::ncol);
			return m2;
		}
		virtual void setup(const char* _name, int n, int m){name = _name, matrixObjectHolder<T*>::_setup_(n,m);};
		void autoLoad(TFile* f, TString path = ""){
			TString name0 = name;
			for(int j=0; j<matrixObjectHolder<T*>::ncol; ++j){
				for(int i=0; i<matrixObjectHolder<T*>::nrow; i++){
					auto hn = name0.ReplaceAll("*", "%d");
					hn = Form(hn, i,j);
					std::cout<<"loading: "<<hn<<std::endl;
					//auto hn = name+"_"+i+"_"+j;
					//std::cout<<"adding "+hn<<std::endl;
					auto ptr = (T*) f->Get(path+hn);
					TString hname = ptr->GetName();
					std::cout<<"adding "+hname<<std::endl;
					this->add(ptr, i,j);
				}
			}
		};
		matrixTObjPtr<T>* load_m2TObj(const char * name, int n, int m, TFile* file){
			auto newm2 = matrixTObjPtr<T>(name, n, m);
			newm2->autoLoad(file);
			return newm2;
		}
		void write(){ for(auto & it:matrixObjectHolder<T*>::ref) it->Write();}
		const char * getName(){return name.c_str();}

		std::string name;
		bool doFree = 0;
};



#endif
