
#ifndef xTSetBase_H
#define xTSetBase_H

#include "TH1.h"
#include "TMath.h"
#include "TString.h"
#include "myProcesses/liteFrame/plugin/xTagger.h"
#include "myProcesses/liteFrame/plugin/histManager.h"
#include "myProcesses/liteFrame/plugin/Utility.h"

class xTSetBase{
	/*
	This object is supposed to be a collection template for histogram
	This collection are desgined to extend the TH1 to add centrality bins and a label, this label is implemented by xTag. 
	 */
        public :
                xTSetBase(){}
                xTSetBase(TString name0, histManager *hm0, int bit, int ncent0, const float *centbin){
			_name = name0;
                        ncent = ncent0;
                        ax = new xAxis(ncent, centbin);
                        centLabel=toolkit::makeCentLabels(ncent, centbin);
			hm = hm0; 
			isInit = 1;
			tag.addTag(bit);
                }
                ~xTSetBase(){}

		template <typename th1>
		th1** regHist(TString name, TString title, int nbin, float min, float max){
                        auto hist = new th1*[ncent];
                        for(int i=0; i<ncent;i++){
                                hist[i] = hm->regHist<th1>(_name+"/"+name+Form("_C%d",i), title+":"+centLabel[i], nbin, min, max);
				hists.emplace_back((TH1*)hist[i]);
                        }
			return hist;
		}

		template <typename th1>
		th1** regHist(TString name, TString title, int nbin, float * bins){
                        auto hist = new th1*[ncent];
                        for(int i=0; i<ncent;i++){
                                hist[i] = hm->regHist<th1>(_name+"/"+name+Form("_C%d",i), title+":"+centLabel[i], nbin, bins);
				hists.emplace_back((TH1*)hist[i]);
                        }
			return hist;
		}

		template<typename th1>
		th1** loadHist(TString name, TFile *f){
			if(!isInit) return 0;
			auto hist = new th1*[ncent];
                        for(int i=0; i<ncent;i++){
				hist[i] = (th1*)f->Get(_name+"/"+name+Form("_C%d",i));
				hists.emplace_back((TH1*)hist[i]);
			}
			return hist;
		}

		void selfNormalization(){
			for(auto & it : hists){
				it->Scale(1.0/it->Integral());
				divide_bin_size(it);
			}
		}

		template <typename th1>
                void fill(th1** hist, xTagger &bit, float hibin, float data, float weight = 1){
                        if(!(bit.select(tag))) return;
                        int jcent = ax->find_bin_in_range(hibin);
                        if(jcent<0) return;
                        hist[jcent]->Fill(data, weight);
                        return;
                }

		bool isInit = 0;
                int ncent=0;
		TString _name;
                xTagger tag;
                xAxis * ax;
                TString *centLabel;
                histManager *hm;
		std::vector<TH1*> hists;
};


#endif
