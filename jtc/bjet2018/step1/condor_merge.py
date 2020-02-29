
import myProcesses.condor.merge_job as mj
<<<<<<< HEAD
job_name = "bjtc_dijetMC_drum_skim_newBuffer"
inputf = "/eos/cms/store/group/phys_heavyions/wangx/aa2018bjet/crab_bjtc_aa2018_dijetP8H_drum_fineMatch_newbuffer/"
=======
job_name = "bjtc_dijetMC_drum_hiForest_Jussi"
inputf = "/eos/cms/store/group/phys_heavyions/wangx/aa2018bjet/crab_bjtc_aa2018_dijetP8H_drum_Jussi_hiForest/"
>>>>>>> refs/remotes/origin/AN20-029
#job_name = "bjtc_bjetMC_drum_skim"
#job_name = "bjtc_bjetMC_drum_skim"
#inputf = "/eos/cms/store/group/phys_heavyions/wangx/aa2018bjet/crab_bjtc_aa2018_bijetP8H_drum_test/"
merge = mj.cjob_merge(name = job_name, nfiles=20, time ='1h',output_dir = './data', path = inputf)

merge.make_cfg()
merge.submit()
