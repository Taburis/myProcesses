
import myProcesses.condor.merge_job as mj
#job_name = "bjtc_djet_std"
#job_name = "bjtc_djet_sube"
#job_name = "bjtc_data_hardproe_jet80or100"
#inputf = "/eos/cms/store/group/phys_heavyions/wangx/aa2018bjet/crab_bjtc_aa2018_hardprobe_data_jet80or100/"
#job_name = "bjtc_bjet_std"
#inputf = "/eos/cms/store/group/phys_heavyions/wangx/aa2018bjet/crab_bjtc_aa2018_bjetP8H_drum_format2_std_nominal/"
#job_name = "bjtc_bjet_sube"
#inputf = "/eos/cms/store/group/phys_heavyions/wangx/aa2018bjet/crab_bjtc_aa2018_bjetP8H_drum_format2_sube_nominal/"
#job_name = "bjtc_djet_std_cJetReweight"
#inputf = "/eos/cms/store/group/phys_heavyions/wangx/aa2018bjet/crab_bjtc_aa2018_QCDjetMC_P8H_drum_format2_std_cJetReweight/"
job_name = "bjtc_data_JESSmearing"
inputf = "/eos/cms/store/group/phys_heavyions/wangx/aa2018bjet/crab_bjtc_aa2018_hardprobe_data_jet80or100_JESSmearing/"
#job_name = "bjtc_bjet_sube_gspWeighted"
#inputf = "/eos/cms/store/group/phys_heavyions/wangx/aa2018bjet/crab_bjtc_aa2018_bjetP8H_drum_format2_sube_GSPWeighted/"
#job_name = "bjtc_djet_sube_neg"
#inputf = "/eos/cms/store/group/phys_heavyions/wangx/aa2018bjet/crab_bjtc_aa2018_QCDjetMC_P8H_drum_format2_sube_neg_cont/"
#job_name = "bjtc_djet_sube"
#inputf = "/eos/cms/store/group/phys_heavyions/wangx/aa2018bjet/crab_bjtc_aa2018_QCDjetMC_P8H_drum_format2_sube_nominal/"
#job_name = "bjtc_bjet_std"
#job_name = "bjtc_bjetMC_gspWeighted"
#job_name = "bjtc_data_hardproe_jet80or100_JER"
#inputf = "/eos/cms/store/group/phys_heavyions/wangx/aa2018bjet/crab_bjtc_aa2018_bjetP8H_drum_format2_std/"
#inputf = "/eos/cms/store/group/phys_heavyions/wangx/aa2018bjet/crab_bjtc_aa2018_QCDjetMC_P8H_drum_format2_std/"
#inputf = "/eos/cms/store/group/phys_heavyions/wangx/aa2018bjet/crab_bjtc_aa2018_bjetP8H_drum_format2_sube/"
merge = mj.cjob_merge(name = job_name, nfiles=50, time ='1h',output_dir = './data', path = inputf)
merge.make_cfg()
merge.submit()
