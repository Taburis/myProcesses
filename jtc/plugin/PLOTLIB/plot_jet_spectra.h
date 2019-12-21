
#include "myProcesses/jtc/plugin/PLOTLIB/multi_pads.h"

template <typename T>
class plot_jet_spectra : public multi_pads<T>{
		public : //plot_jet_spectra(): multi_pads(){}
				 plot_jet_spectra(TString name, int n): multi_pads<T>(name,"", n, 3){} 
				 virtual ~plot_jet_spectra(){}
				 void load_spectra(TString name, TFile*f){
					auto m2 = new matrixTH1Ptr(name, multi_pads<T>::nrow, multi_pads<T>::ncol);
					TH1* h;
					for(int i=0; i< multi_pads<T>::nrow; ++i){
							h = (TH1*)f->Get(name+Form("_corrpt_%d",i));
							h->Scale(1.0/h->Integral());
							h->GetXaxis()->SetTitle("p_{T}^{jet}");
							m2->add(h,i,0);
							h = (TH1*)f->Get(name+Form("_eta_%d",i));
							h->Scale(1.0/h->Integral());
							h->GetXaxis()->SetTitle("#eta^{jet}");
							m2->add(h,i,1);
							h = (TH1*)f->Get(name+Form("_phi_%d",i));
							h->Scale(1.0/h->Integral());
							m2->add(h,i,2);
							h->GetXaxis()->SetTitle("#phi^{jet}");
					}
					multi_pads<T>::addm2TH1(m2);
				 }
};
