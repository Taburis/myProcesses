from CRABAPI.RawCommand import crabCommand
from argparse import ArgumentParser
import myProcesses.t2operation.eos_utility as eos
import subprocess

parser = ArgumentParser()
parser.add_argument("-d", dest = "job_dir", help="directory containning the job information")
args = parser.parse_args()

crab_dir = args.job_dir

res = crabCommand('status', dir=crab_dir)

jobstatus = res['jobList']

nfailed = 0
check_list =[]
nfinished = 0
isrunning = 0
ntransfer = 0
nidle =0

def get_job_id(res):
	ids = res['proxiedWebDir'].split(':')[1]
	ids = ids.split('/')[-1]
	return ids

for i in jobstatus:
	if i[0] == 'failed':
		nfailed+=1
		check_list.append(i[1])
	elif i[0] == 'finished':
		nfinished +=1
	elif i[0] == 'running':
		isruning = 1
	elif i[0] == 'transferring':
		ntransfer+=1
	elif i[0] == 'idle':
		nidle+=1

if nfinished == len(jobstatus):
	cmd = raw_input('job finished, type "hadd" to start merge file:')
	if cmd == 'hadd':
		ids = get_job_id(res)
		data_dir = crab_dir+ids+'/0000/'
		eos.merge(data_dir, './mergedData', '/mnt/hadoop/store/user/wangx/')
elif nfinished != 0 and nidle == 0 and nfailed !=0 and not isrunning:
	print('job running finished:',nfailed,'jobs failed, do resubmit?')

#print(nfinished,'jobs are finished')
#print(ntransfer,'jobs are transferring')
#print(nfailed,'jobs are failed')
#print(nidle,'jobs are idle')
