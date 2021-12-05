#!/usr/bin/env python

import os
import sys
import subprocess
from argparse import ArgumentParser

parser = ArgumentParser()
parser.add_argument("-r", dest="version", help="commit: git log XX.tex to show the commit header")
parser.add_argument("-t", dest="target", help="new file compared to the older one")
parser.add_argument("-n", dest="name", help="output name")
args = parser.parse_args()

version = args.version
target = args.target
name = args.name

cmd = ['latexdiff-vc','-r',version,'--append-context2cmd="abstract"', target]
subprocess.call(cmd)

newcmds ="""
\definecolor{RED}{rgb}{1,0,0}\definecolor{BLUE}{rgb}{0,0,1} %DIF PREAMBLE
\providecommand{\DIFadd}[1]{{\protect\color{blue}\uwave{#1}}} %DIF PREAMBLE
\providecommand{\DIFdel}[1]{{\protect\color{red}\sout{#1}}} %DIF PREAMBLE
%DIF SAFE PREAMBLE %DIF PREAMBLE
\providecommand{\DIFaddbegin}{} %DIF PREAMBLE
\providecommand{\DIFaddend}{} %DIF PREAMBLE
\providecommand{\DIFdelbegin}{} %DIF PREAMBLE
\providecommand{\DIFdelend}{} %DIF PREAMBLE
%DIF FLOATSAFE PREAMBLE %DIF PREAMBLE
\providecommand{\DIFaddFL}[1]{\DIFadd{#1}} %DIF PREAMBLE
\providecommand{\DIFdelFL}[1]{\DIFdel{#1}} %DIF PREAMBLE
\providecommand{\DIFaddbeginFL}{} %DIF PREAMBLE
\providecommand{\DIFaddendFL}{} %DIF PREAMBLE
\providecommand{\DIFdelbeginFL}{} %DIF PREAMBLE
\providecommand{\DIFdelendFL}{} %DIF PREAMBLE
"""

lines = open(target.split('.')[0]+'-diff'+version+'.tex','r').readlines();
addCmd = True
print('open: ',name)
with open(name,'w') as f :
    for line in lines:
        if addCmd : 
            if "cmsNoteHeader" in line: f.writelines(newcmds)
        f.write(line)


