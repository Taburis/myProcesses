

#ifndef ANALYSIS_TOOLKIT_H
#define ANALYSIS_TOOLKIT_H
#include <sstream>
namespace toolkit {
	double findDr(double eta1, double phi1, double eta2, double phi2){
		double dphi = phi1 - phi2;
		while (dphi > M_PI) dphi-= 2*M_PI;
		while (dphi <= -M_PI) dphi += 2*M_PI;
		return sqrt(pow(dphi,2)+pow(eta1-eta2,2));
	}

	std::vector<string> string_split(const char * input, char c){
		string sinput(input);
		stringstream ss(sinput);
		vector<string> result;
		while(ss.good()){
			string substr;
			getline(ss, substr, c);
			result.emplace_back(substr);	
		}
		return result;
	}
	void divide_bin_size(TH1* h){
		// at most 2D histogram, this function works fine for TH2 histogram
		int nybins = h->GetNbinsY();
		int nxbins = h->GetNbinsX();
		for(int i=1; i<nxbins+1; ++i){
			for(int j=1; j<nybins+1; ++j){
				float wx = h->GetXaxis()->GetBinWidth(i);
				float wy = h->GetYaxis()->GetBinWidth(j);
				float cont= h->GetBinContent(i,j);
				float err = h->GetBinError(i,j);
				h->SetBinContent(i,j , cont/wx/wy);
				h->SetBinError(i, j, err/wx/wy);
			}
		}
	}

	TString* makeCentLabels(int nbins, int * hibin){
		//make a centrality label based on the centrality bins
		TString *centLabel = new TString[nbins];
		std::stringstream sstream1, sstream2;
		for(int i=0; i<nbins; ++i){
			sstream1.str(std::string());
			sstream2.str(std::string());
			sstream1<<float(hibin[i])/2;
			sstream2<<float(hibin[i+1])/2;
			centLabel[i]="Cent: "+sstream1.str()+"-"+sstream2.str()+"%";
		}
		return centLabel;
	}


}

#endif
