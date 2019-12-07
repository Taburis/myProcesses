
#ifndef stackPlot_H
#define stackPlot_H
#include "THStack.h"

class stackPlot: public THStack{
		public : stackPlot(TString name):THStack(name, ""){}
				 ~stackPlot(){}
				 void defaultColor(){
						 auto list = GetHists();
						 int nn = list->GetSize();
						 if(nn > 6){
								 std::cout<<"defaultColor failed, please set the color manually!"<<std::endl;
								 return;
						 }
						 for(int i=0; i<nn; ++i){
								 ((TH1*) list->At(i))->SetFillColor(dcolor[i]);
								 ((TH1*) list->At(i))->SetFillStyle(1001);
						 }
				 }
				 void add(TH1* h, TString leg){
						 Add(h); legend->AddEntry(h, leg, "f");
				 }
				 void addLegend(float x1 = 0.6, float y1=0.7, float x2=0.93, float y2=0.93){
						 legend = new TLegend(x1, y1, x2, y2);	legend->SetLineColor(0);
				 }
				 void addTopHist(TH1*h, TString leg){
						 hists.emplace_back(h); legend->AddEntry(h, leg, "lp");
				 }
				 void drawHists(){
						 for(auto &it: hists){
								 it->Draw("same pfc");
						 }
				 }
				 void normalizeStack(float x, float y){
						 auto list = GetHists();	
						 float sum =0 ;
						 for(const auto&& obj: *list){
								 int n0=((TH1*)obj)->FindBin(x);
								 int n1=((TH1*)obj)->FindBin(y);
								 sum+= ((TH1*)obj)->Integral(n0,n1);
						 }
						 for(const auto&& obj: *list)
								 ((TH1*)obj)->Scale(1.0/sum);
				 }
				 void normalizeStack(){
						 auto list = GetHists();	
						 float sum =0 ;
						 for(const auto&& obj: *list){
								 sum+= ((TH1*)obj)->Integral();}
						 for(const auto&& obj: *list)
								 ((TH1*)obj)->Scale(1.0/sum);
				 }
				 void normalizeOverlayHists(float x, float y){
						 for(auto & it : hists){
										 int n0=it->FindBin(x);
										 int n1=it->FindBin(y);
										 it->Scale(1.0/it->Integral(n0,n1));
						 }
				 }
				 void normalizeOverlayHists(){
						 for(auto & it : hists) it->Scale(1.0/it->Integral());
				 }
				 void normalizeAll(float x, float y){
						 normalizeOverlayHists(x,y);
						 normalizeStack(x,y);
				 }

				 Color_t dcolor[6] = {kBlue-9, kOrange+1, kViolet-5, kGreen+3, kRed, kRed+3};
				 TLegend *legend;
				 std::vector<TH1*> hists; // these hists will be draw on top of the stack;
};

#endif
