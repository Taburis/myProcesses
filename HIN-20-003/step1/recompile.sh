#!/bin/csh

setenv base $CMSSW_BASE/src/
rm lib.tar.gz
tar -zvcf lib.tar.gz -C $base myProcesses/liteFrame/plugin -C $base myProcesses/jtc/plugin -C $base myProcesses/HIN-20-003/JEC2018PbPb -C $base myProcesses/HIN-20-003/residualJEC2018 -C $base myProcesses/hiforest/plugin -C $base myProcesses/HIN-20-003/config
# for testing
tar xfz lib.tar.gz

