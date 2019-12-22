

//to speed up the mixing, dump the needed mixing information to local to boost the mixing
#include "xTagger.h"
#ifndef JTCFASTPRODUCER_H
class candidate;
#endif 
#ifndef EVENT_MAP_H
class eventMap;
#endif
#include <vector>
#include <stdlib.h>
class jtcMixingHelper{
	public : jtcMixingHelper(){}
		 ~jtcMixingHelper(){}
		 void buildTree(){
			 buff=TFile::Open("mixing_buff.root", "recreate");
			 mt= new TTree("mixing", "");
			 mt->Branch("vz", &vz);
			 mt->Branch("hibin", &hibin);
			 mt->Branch("ntrks", &ntrks);
			 mt->Branch("trktag", &trktag, "trktag[ntrks]/I");
			 mt->Branch("trkpt" , &trkpt ,"trkpt[ntrks]/F");
			 mt->Branch("trkphi", &trkphi,"trkphi[ntrks]/F");
			 mt->Branch("trketa", &trketa,"trketa[ntrks]/F");
			 mt->Branch("trkw"  , &trkw,"trkw[ntrks]/F");
		 }
		 void addTrk(std::vector<candidate> &trk, float vz0, int hibin0=0){
			 buff->cd();
			 vz = vz0;  hibin = hibin0; ntrks = trk.size();
			 for(int ptr=0; ptr<trk.size(); ptr++){
				 trkpt[ptr]  = trk[ptr].pt;	
				 trketa[ptr] = trk[ptr].eta;	
				 trkphi[ptr] = trk[ptr].phi;	
				 trktag[ptr] = trk[ptr].tag.tag;
				 trkw[ptr]   = trk[ptr].weight;
			 }
			 mt->Fill();
			 trk.clear();
		 }
		 void loadTrks(Long64_t j, std::vector<candidate> &trk){
			//cout<<"ntrks: "<<ntrks<<endl;
			//cout<<"vz: "<<vz<<endl;
			 mt->GetEntry(j);
			 trk.clear(); trk.reserve(ntrks);
			 for(int i=0; i<ntrks; ++i){
				 xTagger tg(trktag[i]);
				 candidate tk(tg,trkpt[i],trketa[i],trkphi[i],trkw[i]);
				 trk.emplace_back(tk);
			 }
		 }
		 void loadTree(){
			 buff = TFile::Open("mixing_buff.root");
			 mt = (TTree*) buff->Get("mixing");
			 mt->SetBranchAddress("vz", &vz);
			 mt->SetBranchAddress("hibin", &hibin);
			 mt->SetBranchAddress("ntrks", &ntrks);
			 mt->SetBranchAddress("trktag", trktag );
			 mt->SetBranchAddress("trkpt" , trkpt   );
			 mt->SetBranchAddress("trkphi", trkphi  );
			 mt->SetBranchAddress("trketa", trketa  );
			 mt->SetBranchAddress("trkw"  , trkw    );
		 }
		void write_buff(){
			buff->cd();
			mt->Write();
			buff->Close();
		}


		 TTree* mt;
		 int ntrks=0;
		 float vz=0;
		 int hibin=-1;
		 int trktag[9999];
		 float trkpt[9999], trketa[9999], trkphi[9999], trkw[9999];
		 int ncent_mix, nvz_mix = 30;
		 std::vector<unsigned int>** mixTable;
		 TFile*buff;
};


