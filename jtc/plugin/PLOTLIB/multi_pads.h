
#ifndef MULTI_PADS_H
#define MULTI_PADS_H
#include "TCanvas.h"
#include "myProcesses/jtc/plugin/Utility.h"
#include "myProcesses/jtc/plugin/matrixTH1Ptr.h"
#include "myProcesses/jtc/plugin/PLOTLIB/stackPlot.h"
#include "myProcesses/jtc/plugin/PLOTLIB/fast_pad.h"
#include "myProcesses/jtc/plugin/PLOTLIB/overlay_pad.h"
#include "myProcesses/jtc/plugin/PLOTLIB/stack_pad.h"

using namespace std;
template <typename T>
class multi_pads : public TCanvas, public base_pad{
	public : multi_pads(TString name, TString title, int n, int m, float w=-1, float h=-1):base_pad(), TCanvas(false), nrow(n), ncol(m){
			 float width , hight;
			 if( typeid(T) == typeid(overlay_pad) || typeid(T) == typeid(stack_pad) ){
				width = (w==-1) ? 325: w;
				hight = (h==-1) ? 400: h;
			 }
			 else {
				width = (w==-1) ? 350: w;
				hight = (h==-1) ? 350: h;
			 }
			 initial<T>(name, title,width, hight);
			 if( typeid(T) == typeid(overlay_pad) ) is_overlay_pad = 1;
		 } 
		 virtual ~multi_pads(){}
		 template <typename T2>
			 void initial(TString name, TString title, float width, float high){
				 //SetCanvasSize(ncol*width,nrow*high);
				 Constructor(name, title, ncol*width,nrow*high);
				 this->cd(0);
				 //								 gPad->SetFillColor(0);
				 Divide(ncol, nrow,.002, 0.002);
				 fpads._setup_(nrow, ncol);	 
				 for(int i=0; i< nrow; ++i){
					 for(int j=0; j< ncol; ++j){
						 //				 cout<<i<<" : "<<j<<endl;
						 CD(i,j);
						 //gPad->DrawFrame(0,0,1,1);
						 auto pad = new T2(name+Form("_fp_%d_%d",i,j));
						 //pad->init();
						 fpads.add(pad,i,j);
					 }
				 }
			 }
		 void addm2TH1(matrixTH1Ptr *m2){
			 if(ncol*nrow != (m2->Ncol())*(m2->Nrow())){
				 //cout<<ncol*nrow<<" : "<<(m2->Ncol())<<" , "<<m2->Nrow()<<endl;
				 std::cout<<"Error: Can't add the m2TH1Ptr, size doesn't match to subpads!"<<std::endl;
	std::cout<<"Details: canvas size: ("<<nrow<<","<<ncol<<"); the input size: ("<<m2->Nrow()<<","<<m2->Ncol()<<")."<<std::endl;
				 return;}
			 m2s.emplace_back(m2);
			 for(int i=0; i< m2->Nrow(); ++i){
				 for(int j=0; j< m2->Ncol(); ++j){
					 TPoint point;
					 if(doHIarrange) point = HI_arrange(i,j);
					 else if(arrange == nullptr){
						 point = flatten_arrange(i,j, m2->Nrow(), m2->Ncol());
					 }else point = arrange(i,j);
					 //cout<<i<<" | "<<j<<"   "<<int(point.GetX())<<" : "<<int(point.GetY())<<endl;
					 //cout<<i<<" | "<<j<<"  : "<<m2->at(i,j)->GetTitle()<<endl;
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
		 TPoint HI_arrange(int i, int j){
			 TPoint p;
			 p.SetX(i);
			 p.SetY(ncol-j-1);
			 return p;
		 }
		void setLogy(int i, int j){fpads.at(i,j)->doLogy = 1;}
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
		 virtual void add(TH1* h, int n, int m){ fpads.at(n,m)->addTH1(h);}
		 T* at(int i,int j){return fpads.at(i,j);}
		 void addhLine(float y){yline = y; doline = 1; };
		 void draw(TString opt = "" ){
			 fpads.at(0,0)->legend = base_pad::legend;
			 fpads.at(0,0)->doLegend = doLegend; 
			 for(int i=0; i< nrow; ++i){
				 for(int j=0; j< ncol; ++j){
					 CD(i,j);
					 fpads.at(i,j)->copy_config(*this);
					 if(is_overlay_pad )((overlay_pad*) fpads.at(i,j))->ratio_title = ratio_title;
					 fpads.at(i,j)->draw(opt);
				 }
			 }
		 }
		 void addLegend(TString pos){
			 doLegend = 1;
			 if( pos == "upperright"){
				 base_pad::legend = new TLegend(0.6, 0.7, 0.93, 0.88);
			 }else if(pos=="bottommiddle"){
				 base_pad::legend = new TLegend(0.45, 0.2, 0.7, 0.45);
			 }
//			 base_pad::legend->SetLineColor(0);
		 }
		 void addLegend(float x1, float y1, float x2, float y2){
			 doLegend = 1;
			 base_pad::legend = new TLegend(x1,y1,x2,y2);
		 }
		 void labelHist(TString lab, int i){
			 base_pad::legend->AddEntry(fpads.at(0,0)->hist.at(i), lab);
		 }
		 void addm2Text(matrixObjectHolder<TString>*m2, float x, float y){
			 for(int i=0; i< m2->Nrow(); ++i){
				 for(int j=0; j< m2->Ncol(); ++j){
					 TPoint p ;
					 if(doHIarrange) p = HI_arrange(i,j);
					 else if(arrange ==nullptr) p = flatten_arrange(i,j, m2->Nrow(), m2->Ncol());
					 else p = arrange(i,j);
					 fpads.at(p.GetX(),p.GetY())->drawText(x, y, m2->at(i,j));
				 }
			 }
		 }
		 void setRatioYrange(float min, float max){
			 for(int i=0; i< nrow; ++i){
				 for(int j=0; j< ncol; ++j){
					 fpads.at(i,j)->rymax=max;
					 fpads.at(i,j)->rymin=min;
				 }}
		 }

		 int Ncol(){return ncol;}
		 int Nrow(){return nrow;}
		 int ncol, nrow;
		 matrixPtrHolder<T> fpads;
		 std::vector<matrixTH1Ptr*> m2s;
		 TString ratio_title;
		 bool doHIarrange = 0;
		 bool is_overlay_pad = 0;
};


#endif
