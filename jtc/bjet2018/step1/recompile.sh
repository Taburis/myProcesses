#!/bin/csh

setenv base $CMSSW_BASE/src/
rm lib.tar.gz
tar -zvcf lib.tar.gz -C $base myProcesses/liteFrame/plugin -C $base myProcesses/jtc/plugin -C $base myProcesses/jtc/JEC2018PbPb -C $base myProcesses/jtc/config -C $base myProcesses/hiforest/plugin -C $base myProcesses/jtc/bjet2018/config
# for testing
tar xfz lib.tar.gz

