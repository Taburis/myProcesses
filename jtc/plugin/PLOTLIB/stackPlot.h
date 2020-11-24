
#ifndef stackPlot_H
#define stackPlot_H
#include "THStack.h"
#include "TLegend.h"
#include "myProcesses/liteFrame/plugin/Utility.h"

class stackPlot: public THStack{
	public : stackPlot(TString name):THStack(name, ""){}
		 stackPlot(TString name, TH1* h, Option_t * axis = "x", Int_t fbin1=1,Int_t lbin1=-1,Int_t fbin2=1,Int_t lbin2 = -1, Option_t* proj="", Option_t * draw_opt=""): THStack(h, axis, name, "", fbin1, lbin1, fbin2, lbin2, proj, draw_opt){}
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
		 void add(TH1* h, TString leg, TString opt = "f"){
			 Add(h); legend->AddEntry(h, leg, opt);
		 }
		 void addLabel(int i ,TString label) {
			 auto list = GetHists();
			 legend->AddEntry(list->At(i), label);
		 }
		 void addLegend(float x1 = 0.65, float y1=0.7, float x2=0.93, float y2=0.93){
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
		 TH1* sumStack(){
			 auto list = GetHists();	
			 bool create = true;
			 for(const auto&& obj: *list){
				 if(create){
					 hsum = (TH1*) ((TH1*)obj)->Clone(Form("%s_hsum",this->GetName()));
					 create = false;
				 }
				 else hsum->Add((TH1*)obj);}
			 return hsum;
		 }
		 void normalizeReference(TH1*h){
			int n0= h->GetNbinsX();
			h->Scale(1.0/h->Integral(1, n0));
			divide_bin_size(h);
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
			 sumStack();
			 float sum = 1;
			 int n0;
			 if(hsum!=0){
				 n0 = hsum->GetNbinsX();
				 sum = hsum->Integral(1, n0);
				 hsum->Scale(1.0/sum);
				 divide_bin_size(hsum);
	cout<<"integral: "<<hsum->Integral()<<endl;
			 }
			 for(const auto&& obj: *list){
				 ((TH1*)obj)->Scale(1.0/sum);
				 divide_bin_size((TH1*)obj);
			 }
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
		 TH1* hsum=0;
};

#endif
