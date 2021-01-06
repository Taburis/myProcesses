
## HiForest related tools


### forestSkimer

The module to further skim the HiForest tree and work on top of the HiForest Sequence. It can be easily customized as needs (it use the `eventMap_hiForest` to read the HiForest data so it will needs to be customized as well). To use it, load it in the config file:
```py
process.load("myProcesses.hiforest.forestSkimer_cff")
```

Append the module `process.forestSkim` to the end of the `EndPath`. Current setup require the jet set enabled the WTARecluster by adding following line to config:
```py
process.JET_ANALYZER_NAM.doWTARecluster= cms.untracked.bool(True)
```

Note that after appended this module, the ouput of the `HiForestAOD.root` will be supressed and a new file `skim.root` will contain the skimed data.


### eventMap

This is a read conventions used for all the analysis in this package to avoid any crash rasing from different data/branch names.

Here is a example to use the 'eventMap_skim.h'. Supposing the input ROOT file is 'skim.root', the way to initiate this object is
```cpp
auto f = TFile::Open("eventMap_skim.h");
auto evt = new eventMap(f);
evt->isMC = 1; // if the input file is a MC sample
evt->isHI = 1; // if the input file contained a centrality bin branch
evt->loadJet(); //loading the jets, including the gen jets if it exists.
evt->loadTrack(); //loading the tracks
evt->loadGenParticle();
Long64_t nevt = evt->evtTree->GetEntriesFast();
for(auto jevt = 0; jevt< nevt; jevt++){
	evt->getEvent(jevt);
	//your analysis body
	for(auto ijet=0; ijet<evt->nJet();ijet++){
		//reco jet loop	
	}
	for(auto ijet=0; ijet<evt->nGenJet();ijet++){
		//gen jet loop	
	}
	for(auto itrk=0; itrk<evt->nTrk();itrk++){
		//track loop	
	}
	for(auto igp=0; igep<evt->nGP();igp++){
		//gen particle loop	
	}
}
```

### simpleReader

Very simple file reader helper to load the `TH1` object automatically. Usage:
```
auto f = TFile::Open("File_Path");
simpleReader sr;
sr.load(f);
```
then the `sr["hist_name"]` is the `TH1*` pointed to that histogram in the files.
