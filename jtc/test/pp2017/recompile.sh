#!/bin/csh

#voms-proxy-init -voms cms
rm lib.tar.gz
#rm corrTable.tar.gz
tar -zvcf lib.tar.gz ../../plugin/*.h
tar -xvf lib.tar.gz # for testing
#tar -zvcf corrTable.tar.gz corrTable
#rm -r bJTC_5TeV_Study/*
#crab submit -c crab_bjtc.py
