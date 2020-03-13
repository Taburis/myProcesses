
import myProcesses.condor.merge_job as mj
<<<<<<< HEAD
<<<<<<< HEAD
job_name = "bjtc_dijetMC_drum_skim_newBuffer"
inputf = "/eos/cms/store/group/phys_heavyions/wangx/aa2018bjet/crab_bjtc_aa2018_dijetP8H_drum_fineMatch_newbuffer/"
job_name = "bjtc_bjetMCPbPb_drum_50mix_run0"
inputf = "/eos/cms/store/group/phys_heavyions/wangx/aa2018bjet/crab_bjtc_aa2018_bijetP8H_drum_50mix_run1/"
#job_name = "bjtc_bjetMC_drum_skim"
#job_name = "bjtc_bjetMC_drum_skim"
#inputf = "/eos/cms/store/group/phys_heavyions/wangx/aa2018bjet/crab_bjtc_aa2018_bijetP8H_drum_test/"
merge = mj.cjob_merge(name = job_name, nfiles=20, time ='1h',output_dir = './data', path = inputf)

merge.make_cfg()
merge.submit()
