#!/bin/csh

setenv base $CMSSW_BASE/src/
#voms-proxy-init -voms cms
rm lib.tar.gz
#rm corrTable.tar.gz
tar -zvcf lib.tar.gz -C $base myProcesses/jtc/plugin -C $base myProcesses/hiforest/plugin 
#tar -zvcf lib.tar.gz ../../../../../myProcesses/jtc/plugin ../../../../../myProcesses/hiforest/plugin
# for testing
#tar -tf lib.tar.gz
tar xfz lib.tar.gz
#rm -r bJTC_5TeV_Study/*
#crab submit -c crab_bjtc.py
