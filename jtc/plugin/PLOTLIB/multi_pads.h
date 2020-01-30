
#ifndef MULTI_PADS_H
#define MULTI_PADS_H
#include "TCanvas.h"
#include "myProcesses/jtc/plugin/matrixTH1Ptr.h"
#include "myProcesses/jtc/plugin/Utility.h"
#include "myProcesses/jtc/plugin/matrixTH1Ptr.h"
#include "myProcesses/jtc/plugin/PLOTLIB/stackPlot.h"
#include "myProcesses/jtc/plugin/PLOTLIB/fast_pad.h"
#include "myProcesses/jtc/plugin/PLOTLIB/overlay_pad.h"

using namespace std;
template <typename T>
class multi_pads : public TCanvas, public base_pad{
	public : multi_pads(TString name, TString title, int n, int m):base_pad(), TCanvas(false), nrow(n), ncol(m){
			 if( typeid(T) == typeid(overlay_pad))
				 init<T>(name, title,325, 400);
			 else init<T>(name, title,350, 350);
		 } 
		 virtual ~multi_pads(){}
		 template <typename T2>
			 void init(TString name, TString title, float width, float high){
				 //SetCanvasSize(ncol*width,nrow*high);
				 Constructor(name, title, ncol*width,nrow*high);
				 this->cd(0);
				 //								 gPad->SetFillColor(0);
				 Divide(ncol, nrow);
				 fpads.setup(nrow, ncol);	 
				 for(int i=0; i< nrow; ++i){
					 for(int j=0; j< ncol; ++j){
						 //				 cout<<i<<" : "<<j<<endl;
						 CD(i,j);
						 //gPad->DrawFrame(0,0,1,1);
						 auto pad = new T2(Form("fp_%d_%d",i,j));
						 //pad->init();
						 fpads.add(pad,i,j);
					 }
				 }
			 }
		 virtual void addm2TH1(matrixTH1Ptr*m2){
			 if(ncol*nrow != (m2->Ncol())*(m2->Nrow())){
				 cout<<"Error: Can't add the m2TH1Ptr, size doesn't match to subpads!"<<endl;
				 return;}
			 m2s.emplace_back(m2);
			 for(int i=0; i< m2->Nrow(); ++i){
				 for(int j=0; j< m2->Ncol(); ++j){
					 TPoint point;
					 if(arrange == nullptr){
						 point = flatten_arrange(i,j, m2->Nrow(), m2->Ncol());
					 }else point = arrange(i,j);
					 //cout<<i<<" | "<<j<<"   "<<int(point.GetX())<<" : "<<int(point.GetY())<<endl;
					 add(m2->at(i,j),int(point.GetX()), int(point.GetY()));
				 }}
		 }
		 TPoint flatten_arrange(int i, int j, int row, int col){
			 TPoint p;
			 int n = i+j*row;
			 p.SetX(floor(n/ncol));
			 p.SetY(n%ncol);
			 return p;
		 }
		 // this the 1-1 mapping from hist to pad
		 TPoint (*arrange)(int i, int j) = nullptr;
		 void (*plot_style_init)(TH1* ) = nullptr;
		 int flatten(int n, int m){
			 if(n > nrow -1 || m > ncol -1 ) {
				 std::cout<<" ==== ERROR: index ("<<n<<", "<<m<<") exceeds the range!!! current shape ["<<nrow<<", "<<ncol<<"] ===="<<std::endl;
				 return 0;
			 }
			 //slightly different from the matrixPtrHolder, the row and column in canvas start from 1 and up, 0 stands for the whole pad.
			 return n*ncol+m+1;
		 }
		 void CD(int n, int m){ this->cd(flatten(n,m));}
		 void add(TH1* h, int n, int m){ fpads.at(n,m)->add(h);}
		 T* at(int i,int j){return fpads.at(i,j);}
		 void addhLine(float y){yline = y; doline = 1; };
		 void draw(TString opt = "" ){
			 fpads.at(0,0)->legend = legend;
			 fpads.at(0,0)->doLegend = doLegend;
			 for(int i=0; i< nrow; ++i){
				 for(int j=0; j< ncol; ++j){
					 //cout<<i<<":"<<j<<endl;
					 CD(i,j);
					 fpads.at(i,j)->copy_config(*this);
					 fpads.at(i,j)->draw(opt);
				 }
			 }
		 }
		 void addLegend(TString pos){
			 doLegend = 1;
			 if( pos == "upperright") legend = new TLegend(0.7, 0.7, 0.93, 0.88);
			 legend->SetLineColor(0);
		 }
		 void labelHist(TString lab, int i){
			 legend->AddEntry(m2s.at(i)->at(0,0), lab);
		 }
		 void addm2Text(matrixObjectHolder<TString>*m2, float x, float y){
			 for(int i=0; i< m2->Nrow(); ++i){
				 for(int j=0; j< m2->Ncol(); ++j){
					 TPoint p ;
					 if(arrange ==nullptr) p = flatten_arrange(i,j, m2->Nrow(), m2->Ncol());
					 else p = arrange(i,j);
					 fpads.at(p.GetX(),p.GetY())->drawText(x, y, m2->at(i,j));
				 }
			 }
		 }
		 void setRatioYrange(float min, float max){
		 }

		 int Ncol(){return ncol;}
		 int Nrow(){return nrow;}
		 int ncol, nrow;
		 matrixPtrHolder<T> fpads;
		 std::vector<matrixTH1Ptr*> m2s;
};


#endif
