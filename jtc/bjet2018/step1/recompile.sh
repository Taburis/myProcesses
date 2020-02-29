#!/bin/csh

setenv base $CMSSW_BASE/src/
rm lib.tar.gz
tar -zvcf lib.tar.gz -C $base myProcesses/jtc/plugin -C $base myProcesses/jtc/config -C $base myProcesses/hiforest/plugin
# for testing
tar xfz lib.tar.gz

