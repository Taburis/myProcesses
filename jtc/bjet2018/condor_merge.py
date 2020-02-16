
import myProcesses.condor.merge_job as mj

merge = mj.cjob_merge(name = "tes", nfiles=100, time ='1h',output_dir = './data', path = '/eos/cms/store/group/phys_heavyions/wangx/aa2018bjet/')
#merge = mj.cjob_merge(name = "bjtc_dijetMC", nfiles=100, time ='1h',output_dir = './data', path = '/eos/cms/store/group/phys_heavyions/wangx/aa2018bjet/')

merge.make_cfg()
#merge.submit()
