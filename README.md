
## Install

git clone this repo to the `$CMSSW_BASE/src/HeavyIonsAnalysis/` and then:
```bash
cd $CMSSW_BASE/src/
cmsenv
scram b -j4
```

After the foresting finished, two out files created (`skim.root` and `HiForest.root`). Just keep the slimed file `skim.root` in the configure for crab to transfer.
