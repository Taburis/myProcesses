
#include "xAxis.h"

class centralityHelper{
	public : 
		centralityHelper(int nhibin, float *hibins){
			nbins = nhibin;
			hibin = hibins;
			centax = new xAxis(nbins, hibin);
			makeLabels();
		}
		~centralityHelper(){}
		int jcent(int hibin){
			return centax->findBin(hibin);
		}
		void makeLabels(){
			centLabel = new TString[nbins];
			for(int i=0; i<nbins; ++i){
				centLabel[i]=Form("Cent:%d-%d%%",(int)floor(hibin[i]/2),(int)floor(hibin[i+1]/2));
			}
			return centLabel;
		}
		
		float *hibin;
		int nbins;
		xAxis *centax; 
		TString *centLabel;
};

