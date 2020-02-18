
import myProcesses.condor.merge_job as mj

#merge = mj.cjob_merge(name = "bjtc_dijetMC_110jet", nfiles=100, time ='2h',output_dir = './data', path = '/eos/cms/store/group/phys_heavyions/wangx/aa2018bjet/crab_bjtc_aa2018_dijetP8H_drum_110Jet_quickLook/')
merge = mj.cjob_merge(name = "bjtc_dijetMC", nfiles=100, time ='2h',output_dir = './data', path = '/eos/cms/store/group/phys_heavyions/wangx/aa2018bjet/crab_bjtc_aa2018_dijetP8H_drum_quickLook/')

merge.make_cfg()
merge.submit()
