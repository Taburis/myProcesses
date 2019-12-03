
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
		Color_t dcolor[6] = {kBlue-9, kOrange+1, kViolet-5, kGreen+3, kRed, kRed+3};
		TLegend *legend;
};

#endif
