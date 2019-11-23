

#ifndef JTCUTI_H
#define JTCUTI_H

#include <fstream>
#include <algorithm>
#include <iostream>
#include <vector>
#include <stdlib.h>
#include "TH1.h"
#include "TH2.h"

struct histCase{
	//if want to add any hist, need to add it in the quickHistReg as well, and add the filling in the fillCase
	TH2D** sig;
	TH2D** sig_pTweighted;
	TH2D** mixing;
	TH1D** jet_pt;
	TH1D** jet_eta;
	TH1D** jet_phi;
};


double findDr(double eta1, double phi1, double eta2, double phi2){
	double dphi = phi1 - phi2;
	while (dphi > M_PI) dphi-= 2*M_PI;
	while (dphi <= -M_PI) dphi += 2*M_PI;
	return sqrt(pow(dphi,2)+pow(eta1-eta2,2));
}

void ReadFileList(std::vector<std::string> &my_file_names, TString file_of_names, bool debug)
{
        std:fstream file_stream(file_of_names);
        std::string line;
        my_file_names.clear();
        if( debug ) std::cout << "Open file " << file_of_names << " to extract files to run over" << std::endl;
        if( file_stream.is_open() ) {
                if( debug ) std::cout << "Opened " << file_of_names << " for reading" << std::endl;
                int line_num = 0;
                while( file_stream >> line ) {
                        if( debug ) std::cout << line_num << ": " << line << std::endl;
                        std::string tstring_line(line);
                        tstring_line.erase(std::remove(tstring_line.begin(), tstring_line.end(), '"'), tstring_line.end());
                        tstring_line.erase(std::remove(tstring_line.begin(), tstring_line.end(), ','), tstring_line.end());
                        tstring_line.erase(std::remove(tstring_line.begin(), tstring_line.end(), '['), tstring_line.end());
                        tstring_line.erase(std::remove(tstring_line.begin(), tstring_line.end(), ']'), tstring_line.end());
                        if( tstring_line != "" ) my_file_names.push_back(tstring_line);
                        line_num++;
                }
        } else {
                std::cout << "Error, could not open " << file_of_names << " for reading" << std::endl;
                assert(0);
        }
}




#endif
