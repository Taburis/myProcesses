

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
}

#endif
