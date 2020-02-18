
#include "myProcesses/hiforest/plugin/eventMap_hiForest.h" // for the hiforest file
#include "myProcesses/jtc/plugin/jtcFastProducer.C"
#include "myProcesses/jtc/config/config_aa2018_bjet.h"
#include "myProcesses/jtc/plugin/xAxis.h"
#include "myProcesses/jtc/plugin/matrixTH1Ptr.h"
#include "myProcesses/jtc/plugin/plotLib.h"
#include "myProcesses/jtc/plugin/histManager.h"
using namespace AA2018bJet;
void trkMapCheck(){
	config_init();

	//TString path = "mixing_buffer.root";
	TString path = "/eos/cms/store/group/phys_heavyions/wangx/PH2018_JetSamples/mixingBuffer/minbias_MC_mixing_buffer.root";
	TString out_plot = "/eos/user/w/wangx/www/AN20-029/step0";
	//TString path = "../data/minbias2018_MC_mixingSkim.root";
	bool isMC  = 1;
	auto ptax = new xAxis(npt, ptbins);
	auto ctax = new xAxis(ncent, centbins);
	auto hm = new histManager();

	auto m1 = new matrixTH1Ptr("trkmap", npt, ncent);
	auto m2 = new matrixTH1Ptr("genmap", npt, ncent);
	for(int i=0; i<npt; ++i){
		for(int j=0; j<ncent; ++j){
			TString title = "trks: "+ptLabels[i]+", "+centLabels[j];
			auto h = hm->regHist<TH2D>(Form("tkMap_P%d_C%d",i,j), title, 100, -2.5, 2.5, 128, -3.2, 3.2);
			m1->add((TH1D*)h,i,j);
			if(isMC){
				title = "gens: "+ptLabels[i]+", "+centLabels[j];
				auto h2 = hm->regHist<TH2D>(Form("genMap_P%d_C%d",i,j), title, 100, -2.5, 2.5, 128, -3.2, 3.2);
				m2->add((TH1D*)h2,i,j);
			}
		}
	}
cout<<"!"<<endl;
	hm->sumw2();
	int ntrks=0, ngps=0;
	float vz=0;
	int hibin=-1;
	int trktag[9999];
	float trkpt[9999], trketa[9999], trkphi[9999], trkw[9999];
	int gptag[9999];
	float gppt[9999], gpeta[9999], gpphi[9999], gpw[9999];

	auto buff = TFile::Open(path);
	auto mbuff = (TTree*) buff->Get("mixing");
	mbuff->SetBranchAddress("vz", &vz);
	mbuff->SetBranchAddress("hibin", &hibin);
	mbuff->SetBranchAddress("ntrks", &ntrks);
	mbuff->SetBranchAddress("trktag", trktag );
	mbuff->SetBranchAddress("trkpt" , trkpt   );
	mbuff->SetBranchAddress("trkphi", trkphi  );
	mbuff->SetBranchAddress("trketa", trketa  );
	mbuff->SetBranchAddress("trkw"  , trkw    );
	if(isMC){
		mbuff->SetBranchAddress("ngps", &ngps);
		mbuff->SetBranchAddress("gptag", gptag );
		mbuff->SetBranchAddress("gppt" , gppt   );
		mbuff->SetBranchAddress("gpphi", gpphi  );
		mbuff->SetBranchAddress("gpeta", gpeta  );
		mbuff->SetBranchAddress("gpw"  , gpw    );
	}
	Long64_t nevt = mbuff->GetEntries();
	std::cout<<"total events: "<<nevt<<endl;
	for(auto ievt = 0; ievt<nevt; ++ievt){
		mbuff->GetEntry(ievt);
		if(ievt%100000==0) cout<<ievt<<endl;
		int ict = ctax->find_bin_in_range(hibin);
		if(ict<0) continue;
		for(int i=0; i<ntrks; i++){
			int ipt = ptax->find_bin_in_range(trkpt[i]);
			if(ipt<0) continue;
			((TH2*)m1->at(ipt,ict))->Fill(trketa[i], trkphi[i], trkw[i]);
		}
		if(!isMC) continue;
		for(int i=0; i<ngps; i++){
			int ipt = ptax->find_bin_in_range(gppt[i]);
			if(ipt<0) continue;
			((TH2*)m2->at(ipt,ict))->Fill(gpeta[i], gpphi[i], gpw[i]);
		}
	}
	auto c =new multi_pads<base_pad>("trk", "", npt, ncent);
	c->doHIarrange=1;
	c->addm2TH1(m1);
	c->setYrange(-3.2, 3.2);
	c->draw("colz");
	c->SaveAs(out_plot+"/mix_trk_map.jpg");
	
	auto wf = TFile::Open("scan_mixing_buffer.root", "recreate");
	hm->write();
	wf->Close();
}
