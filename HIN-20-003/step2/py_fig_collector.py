
import subprocess 
import os

#eospath = '/eos/user/w/wangx/www/AN20-029/bjtc_c2bin_50mix_wf001_cwfix'
class copyer:
	def __init__(self):
		self.lin=[]
		self.lout=[]
		self.folder = []
		self.main = 'figures'
#		self.eos = ''

	def add(self, path, fig, name):
		self.lin.append(path+'/'+fig)
		self.lout.append(name)

	def check(self):
		ismissing = False
		for f in self.lin:
			if not os.path.exists(f):
				print('missing file:',f)
				ismissing = True
		return ismissing

	def mkdir(self):
		for f in self.lout:
			ff = f[:f.rfind('/')]
			if ff in self.folder: continue
			self.folder.append(ff)
		for f in self.folder:
			os.system(('mkdir -p {FOLDER}').format(FOLDER=self.main+'/'+f))
			
	def copy(self):
		if self.check() : return
		self.mkdir()
		for i in range(len(self.lin)):
			f = self.lin[i]
			tar = self.main+'/'+self.lout[i]
#			path = self.eos+'/'+f
			os.system(('cp {target} {newfile}').format(target=f,newfile=tar))

	def tarball(self):
		print('=================== Making tarball  ====================')
		os.system(('tar -zvcf {newfile} {target}').format(target=self.main,newfile=self.main+'.tar.gz'))
		
