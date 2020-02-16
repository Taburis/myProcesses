import os, math
import condor_utility as cu

class cjob_merge:

    def __init__(self, name, nfiles, path, time, output_dir):
        self.inpath = path
        self.out_name = name + '.root'
        self.output_dir = output_dir
        self.workdir = 'merge_' + name
        self.nfiles = nfiles
        self.cfg_list = []
	self.time = cu.time_sequence[time]

    def make_cfg(self):
        workdir = self.workdir
        if not os.path.exists(('{FOLDER}').format(FOLDER=workdir)):
            os.system(('mkdir {FOLDER}').format(FOLDER=workdir))
            os.system(('mkdir {FOLDER}/outCondor').format(FOLDER=workdir))
            os.system(('mkdir {FOLDER}/data').format(FOLDER=workdir))
        cmssw = os.getenv('CMSSW_BASE')
        pwd = os.getenv('PWD')
        flist = []
        for dirpaths, dirnames, files in os.walk(self.inpath):
            for f in files:
                if f.split('.')[(-1)] != 'root':
                    continue
                flist.append(os.path.join(dirpaths, f))

        total = len(flist)
        print 'Find %d root files, will merge them in step of %d' % (total, self.nfiles)
        njob = int(math.ceil(float(total) / self.nfiles))
        print '%d jobs will be submitted...' % njob
        outlist = []
        script_temp = cmssw + '/src/myProcesses/condor/utility/script_condor_template.sh'
        cfg_temp = cmssw + '/src/myProcesses/condor/utility/condor_template.cfg'
        merge_temp = cmssw + '/src/myProcesses/condor/utility/merge_temp.py'
        env_setup = 'export PYTHONHOME="/cvmfs/cms.cern.ch/slc7_amd64_gcc820/external/python/2.7.15/"\npushd ' + cmssw + '/src\neval `scramv1 runtime -csh`\npushd ' + pwd + '/' + workdir
        for i in range(njob):
            endindex = (i + 1) * self.nfiles
            if endindex > total:
                endindex = -1
            outname = pwd + '/' + workdir + '/data/merge_part%d.root' % i
            outlist.append(outname)
            merge_list = ''
            for f in flist[i * self.nfiles:endindex]:
                merge_list = merge_list + f + ' '

            cmd = 'hadd -f ' + outname + ' ' + merge_list
            if njob == 1:
                cmd = cmd + '\n mv ' + outname + ' ' + output_dir + '/' + self.out_name
            else:
                cmd = cmd + '\npython last_merge.py'
            parlist = {'ENV_SETUP': env_setup}
            parlist['PRERUN'] = '\n'
            parlist['EXECUTABLE'] = cmd
            cu.subText(script_temp, workdir + '/script_' + str(i) + '.sh', parlist)
            parlist = {}
            parlist['SCRIPT'] = 'script_' + str(i) + '.sh'
            os.system('chmod 755 ' + workdir + '/script_' + str(i) + '.sh')
            parlist['KINDEX'] = str(i)
            parlist['FOLDER'] = pwd + '/' + workdir
            parlist['TIME'] = '+JobFlavour ='+self.time
            cu.subText(cfg_temp, workdir + '/condor_cfg_' + str(i) + '.cfg', parlist)
            self.cfg_list.append('condor_cfg_' + str(i) + '.cfg')

	parlist= {}
	cmd = ''
	for f in outlist:
		cmd=cmd+'inlist.append('+'"'+f+'")\n'
	parlist['INPUT'] = cmd
	parlist['OUTPUT'] = '"'+self.output_dir+'/'+self.out_name+'"'
        cu.subText(merge_temp, workdir + '/last_merge.py', parlist)


    def submit(self):
    	os.chdir(self.workdir)
        for cfg in self.cfg_list:
           os.system("condor_submit {CFG}".format(CFG=cfg)) 
	
