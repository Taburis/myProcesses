
## Install

### Using in parallel with CMSSW

```bash
scram project CMSSW_X_Y_Z
cd CMSSW_X_Y_Z
cmsenv
git cms-init
```

git clone this repo to the `$CMSSW_BASE/src/` and then:
```bash
cd $CMSSW_BASE/src/
git cms-addpkg myProcess/
scram b -j4
```


