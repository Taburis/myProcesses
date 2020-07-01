
import myProcesses.condor.merge_job as mj
job_name = "bjtc_djet_std_v2"
#job_name = "bjtc_djet_sube_v2"
#job_name = "bjtc_bjet_std_v2"
#job_name = "bjtc_bjet_sube_run1"
job_name = "bjtc_data_hardproe"
inputf = "/eos/cms/store/group/phys_heavyions/wangx/aa2018bjet/crab_bjtc_aa2018_dijetP8H_drum_sube_v3_4cent_pthat50"
#job_name = "bjtc_bjetMC_drum_skim"
#job_name = "bjtc_bjetMC_drum_skim"
#inputf = "/eos/cms/store/group/phys_heavyions/wangx/aa2018bjet/crab_bjtc_aa2018_bijetP8H_drum_test/"
merge = mj.cjob_merge(name = job_name, nfiles=50, time ='1h',output_dir = './data', path = inputf)
merge.make_cfg()
merge.submit()
