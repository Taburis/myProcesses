

#ifndef PLOTMANAGER_H
#define PLOTMANAGER_H

#include "myProcesses/liteFrame/plugin/plots/basePad.h"
#include "myProcesses/jtc/plugin/matrixTH1Ptr.h"


class plotManager {
	public : 
		plotManager(){}
		~plotManager(){}
		template <typename pd>
			void init(TString name, TString title, int n, int m, float w=-1, float h=-1){
				nrow = n; ncol = m; 
				c = new TCanvas(name, title, ncol*w, nrow*h);
				c->Divide(ncol,nrow, 0.002, 0.002);
				fpads._setup_(nrow, ncol);
				for(int i=0; i< nrow; ++i){
					for(int j=0; j< ncol; ++j){
						//       cout<<i<<" : "<<j<<endl;
						cd(i,j);
						auto pad = new pd(name+Form("_fp_%d_%d",i,j));
						fpads.add((basePad*) pad,i,j);
						//						pad->createPad();
					}
				}
			}

		void initSquarePad(TString name, TString title, int n, int m){
			init<squarePad>(name, title, n, m ,350, 350);
		}
		void initOverlayPad(TString name, TString title, int n, int m){
			init<overlayPad>(name, title, n, m ,325, 400);
		}
		int flatten(int n, int m){
			if(n > nrow -1 || m > ncol -1 ) {
				std::cout<<" ==== ERROR: index ("<<n<<", "<<m<<") exceeds the range!!! current shape ["<<nrow<<", "<<ncol<<"] ===="<<std::endl;
				return 0;
			}
			//slightly different from the matrixPtrHolder, the row and column in canvas start from 1 and up, 0 stands for the whole pad.
			return n*ncol+m+1;
		}

		void cd(int n, int m){ c->cd(flatten(n,m));}
		basePad* at(int n, int m){ return fpads.at(n,m);}
		void addHist(TH1* h, int n, int m, TString label = "", TString labelOpt = ""){ at(n,m)->addHist(h,label,labelOpt); }

		void draw(){
			for(int i=0; i< nrow; ++i){
				for(int j=0; j< ncol; ++j){
					cd(i,j);
					//cout<<i<<" , "<<j<<endl;
					fpads.at(i,j)->draw();
				}
			}
		}
		void drawLegend(TString pos ="upperright"){
			auto lg = this->at(0,0)->bookLegend(pos);
			this->at(0,0)->drawLegend(*lg);
		}
		void setXrange(float min, float max){
			for(int i=0; i< nrow; ++i){
				for(int j=0; j< ncol; ++j){
					at(i,j)->xmin = min;
					at(i,j)->xmax = max;
				}
			}
		}


		float weight, high;
		matrixPtrHolder<basePad> fpads;
		TCanvas *c;
		int ncol, nrow;
};

#endif
