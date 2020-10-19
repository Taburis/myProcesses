
import myProcesses.condor.merge_job as mj
#job_name = "bjtc_djet_std"
job_name = "bjtc_djet_sube"
inputf = "/eos/cms/store/group/phys_heavyions/wangx/aa2018bjet/crab_bjtc_aa2018_QCDjetMC_P8H_drum_format2_sube/"
#job_name = "bjtc_bjet_std"
#job_name = "bjtc_bjet_sube"
#job_name = "bjtc_data_hardproe_jet80or100_JEU_up"
#job_name = "bjtc_bjetMC_gspWeighted"
#job_name = "bjtc_data_hardproe_jet80or100_JER"
#inputf = "/eos/cms/store/group/phys_heavyions/wangx/aa2018bjet/crab_bjtc_aa2018_bjetP8H_drum_format2_std/"
#inputf = "/eos/cms/store/group/phys_heavyions/wangx/aa2018bjet/crab_bjtc_aa2018_QCDjetMC_P8H_drum_format2_std/"
#inputf = "/eos/cms/store/group/phys_heavyions/wangx/aa2018bjet/crab_bjtc_aa2018_bjetP8H_drum_format2_sube/"
merge = mj.cjob_merge(name = job_name, nfiles=50, time ='1h',output_dir = './data', path = inputf)
merge.make_cfg()
merge.submit()
