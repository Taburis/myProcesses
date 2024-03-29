
#ifndef HISTMANAGER_H
#define HISTMANAGER_H
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TFile.h"
#include <vector>
#include <unordered_map>
#include "myProcesses/liteFrame/plugin/toolkit.h"
using namespace std;
class histManager {
	public: 
		histManager(){
		};
		~histManager(){
			for(auto & h: hists){
				h->Delete();
			}
			hists.clear();
		};
		string get_file_name(string path){
			auto pos = path.rfind("/");
			if(pos!=string::npos)return std::move(path.substr(pos+1));
			else return std::move(path);
		}
		string get_folder(string path){
			auto pos = path.rfind("/");
			if(pos!=string::npos)return std::move(path.substr(0,pos));
			else return std::move("");
		}
		void add(const char* name, TH1* h);

		//TH1 constructor
		template <typename T> T* regHist (const char* name, const char* title, int nbin, double x, double y);
//		template <typename T> T* regHist (const char* name, const char* title, int nbin, const float *bins);
		template <typename T> T* regHist (const char* name, const char* title, Int_t nbin, const Double_t *bins);
		//TH2 constructor
		template <typename T> T* regHist (const char* name, const char* title, int nbin, const Double_t *bins, Int_t nbin2, Double_t ylow, Double_t yup);
		template <typename T> T* regHist (const char* name, const char* title, int nx, double x1, double x2, 
				int ny, double y1, double y2);
		//TH3 constructor
		template <typename T> T* regHist (const char* name, const char* title, Int_t nx, Double_t xmin,Double_t xmax, Int_t ny, Double_t ymin, Double_t ymax, Int_t nz, Double_t zmin, Double_t zmax){
			auto fname = get_file_name(name);
			T *h = new T(fname.c_str(), title, nx, xmin, xmax, ny, ymin, ymax, nz, zmin, zmax);
			add(name, (TH1*) h);
			return h;
		}
		template <typename T> T* regHist (const char* name, const char* title, int nx, float*xs, int ny, float*ys, int nz, float*zs){
			auto fname = get_file_name(name);
			T *h = new T(fname, title, nx, xs, ny,ys, nz,zs);
			add(name, (TH1*) h);
			return h;
		}
		template <typename T> T* regHist (const char* name, const char* title, int nx, Double_t*xs, int ny, Double_t*ys, int nz, Double_t*zs){
			auto fname = get_file_name(name);
			T *h = new T(fname.c_str(), title, nx, xs, ny,ys, nz,zs);
			add(name, (TH1*) h);
			return h;
		}
		template <typename T> T* get(const char* name);
		template <typename T> int fill(const char * name, double x, double w=1){
			return get<T>(name)->Fill(x, w);
		}
		void sumw2();
		//void write();
		void write(TFile *f);
	public:
		std::unordered_map<string, vector<TH1*>> map;
		std::vector<TDirectory*> dirs;
		std::vector<TH1*> hists;	
};

void histManager::add(const char* path, TH1* h){
	auto folder = get_folder(string(path));
	vector<string> vec = toolkit::string_split(path,'/');
	if(vec.size() == 1){
		folder = "./";
	}
	//cout<<"adding "<<h->GetName()<<" to folder: "<<folder.c_str()<<endl;
	if(map.count(folder) < 1){
		vector<TH1*> hs;
		map[folder] = std::move(hs);
	}
	map[folder].emplace_back(h);
	hists.emplace_back(h);
//	cout<<h->GetName()<<" : "<<path<<" : "<<hists.size()<<endl;
}

template <typename T> T* histManager::regHist (const char* name, const char* title, int nbin, double x, double y){
	auto fname = get_file_name(name);
	T *h = new T(fname.c_str(), title, nbin, x, y);
	add(name, (TH1*) h);
	return h;
}

//template <typename T> T* histManager::regHist (const char* name, const char* title, int nbin, const float* bins){
//	auto fname = get_file_name(name);
//	T *h = new T(fname.c_str(), title, nbin, bins);
//	add(name, (TH1*) h);
//	return h;
//}
template <typename T> T* histManager::regHist (const char* name, const char* title, int nbin, const Double_t* bins){
	auto fname = get_file_name(name);
	T *h = new T(fname.c_str(), title, nbin, bins);
	add(name, (TH1*) h);
	return h;
}

template <typename T> T* histManager::regHist (const char* name, const char* title, int nbin, const Double_t* bins, Int_t nbin2, Double_t ylow, Double_t yup){
	auto fname = get_file_name(name);
	T *h = new T(fname.c_str(), title, nbin, bins, nbin2, ylow, yup);
	add(name, (TH1*) h);
	return h;
}

template <typename T> T* histManager::regHist (const char* name, const char* title, int nx, double x1, double x2,
		int ny, double y1, double y2){
	auto fname = get_file_name(name);
	T *h = new T(fname.c_str(), title, nx, x1, x2, ny, y1, y2);
	add(name, (TH1*) h);
	return h;
}


void histManager::sumw2(){
	for(auto &h : hists){
		//cout<<"sumw2: "<<h->GetName()<<endl;
		h->Sumw2();
	}
}

void histManager::write(TFile *f){
	for(auto && pair: map ){
		bool doset = 1;
		TDirectory *dir;
		vector<string> split = toolkit::string_split(pair.first.c_str(), '/');
		if(split[0]=='.'){
			doset = 0;
			f->cd();
		}
		else {
		//	cout<<"creating the folder: "<<pair.first<<endl;
			dir = f->mkdir(pair.first.c_str());
			dirs.emplace_back(dir);
			dir->cd();
		}
		for(auto & h : pair.second){
		//	cout<<h->GetName()<<endl;
			if(doset) h->SetDirectory(dir);
			h->Write();
		}
	}
}
#endif
