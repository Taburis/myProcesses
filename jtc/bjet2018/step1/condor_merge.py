
import myProcesses.condor.merge_job as mj
#job_name = "bjtc_djet_std"
#job_name = "bjtc_djet_sube"
#job_name = "bjtc_bjet_std"
job_name = "bjtc_bjet_sube"
#job_name = "bjtc_data_hardproe"
inputf = "/eos/cms/store/group/phys_heavyions/wangx/aa2018bjet/crab_bjtc_aa2018_bjetP8H_drum_format2_sube_c5shift_q50_cwfix"
#inputf = "/eos/cms/store/group/phys_heavyions/wangx/aa2018bjet/crab_bjtc_aa2018_bijetP8H_drum_test/"
merge = mj.cjob_merge(name = job_name, nfiles=50, time ='1h',output_dir = './data', path = inputf)
merge.make_cfg()
merge.submit()
