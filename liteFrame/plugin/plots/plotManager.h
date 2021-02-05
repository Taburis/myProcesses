

#ifndef PLOTMANAGER_H
#define PLOTMANAGER_H

#include "myProcesses/liteFrame/plugin/plots/basePad.h"
#include "myProcesses/jtc/plugin/matrixTH1Ptr.h"

class matrixTH1Ptr;

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

		void setColorPalette(int k){
			for(int i=0; i< nrow; ++i){
				for(int j=0; j< ncol; ++j){
					fpads.at(i,j)->style->SetPalette(k);
				}
			}
		}
		void draw(TString opt = ""){
			for(int i=0; i< nrow; ++i){
				for(int j=0; j< ncol; ++j){
					cd(i,j);
					//cout<<i<<" , "<<j<<endl;
					fpads.at(i,j)->doLogy = doLogy;
					fpads.at(i,j)->draw(opt);
				}
			}
		}
		void drawLegend(float x1, float y1, float x2, float y2){
			auto lg = new TLegend(x1,y1,x2,y2);
			lg->SetLineColor(0);
			this->at(0,0)->drawLegend(*lg);
		}
		void drawLegend(TString pos ="upperright"){
			auto lg = this->at(0,0)->bookLegend(pos);
			this->at(0,0)->drawLegend(*lg);
		}
		void setYrange(float min, float max){
			for(int i=0; i< nrow; ++i){
				for(int j=0; j< ncol; ++j){
					at(i,j)->ymin = min;
					at(i,j)->ymax = max;
				}
			}
		}
		void setRatioYrange(float min, float max){
			for(int i=0; i< nrow; ++i){
				for(int j=0; j< ncol; ++j){
					((overlayPad*)at(i,j))->rymin = min;
					((overlayPad*)at(i,j))->rymax = max;
				}
			}
		}

		void setXrange(float min, float max){
			for(int i=0; i< nrow; ++i){
				for(int j=0; j< ncol; ++j){
					//cout<<"before============= "<<at(i,j)->xmin<<" : "<<at(i,j)->xmax<<endl;
					at(i,j)->xmin = min;
					at(i,j)->xmax = max;
					//cout<<"after============= "<<at(i,j)->xmin<<" : "<<at(i,j)->xmax<<endl;
				}
			}
		}

		void drawHLine(float y, int style){
			for(int i=0; i< nrow; ++i){
				for(int j=0; j< ncol; ++j){
					//cout<<i<<" , "<<j<<endl;
					auto pad = fpads.at(i,j);
					pad->line.SetLineStyle(style);
					float xmin = pad->xmin;
					float xmax = pad->xmax;
					cd(i,j);
					pad->line.DrawLine(xmin,y, xmax, y);
				}
			}
		}

		void addm2TH1(matrixTH1Ptr *m2, TString label = "", TString labelOpt="pl", bool reverseColumn = 1){
			for(int i=0; i< nrow; ++i){
				for(int j=0; j< ncol; ++j){
					int k = j;
					if(reverseColumn) k = ncol - j -1;
					addHist(m2->at(i,j), i,k, label, labelOpt);
				}
			}
		}
		void save(TString path){
			this->c->SaveAs(path);
		}

		float weight, high;
		matrixPtrHolder<basePad> fpads;
		TCanvas *c;
		int ncol, nrow;
		bool doLogy=0;
};

#endif
