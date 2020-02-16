
import myProcesses.condor.merge_job as mj

merge = mj.cjob_merge(name = "bjtc_dijetMC", nfiles=500, output_dir = './data', path = '/eos/cms/store/group/phys_heavyions/wangx/aa2018bjet/')

merge.make_cfg()
merge.submit()