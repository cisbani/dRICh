#!/usr/bin/env python

# scan through detector parameter space, and run npsim

import numpy as np 
import math as m
import sys, os, json, subprocess, shlex, re

# get original compact file
athenaDir = os.environ['DRICH_DD4_ATHENA']
compactInName = 'compact/drich_orig.xml'
compactInFile = open(athenaDir+'/'+compactInName,'r')

# constants [units=cm]
snoutLength = 50
vesselLength = 180
tankRadius = 200

# lists
def sep(): print('-'*40)
def dictPrint(d): print(json.dumps(d,indent=4))
it = {}
i=0
it['retune'] = list(np.linspace( 0.1, 0.3, 3 ))
it['centerZ'] = list(np.linspace( -50, 0, 6 ))
it['centerX'] = list(np.linspace( tankRadius+10, tankRadius+30, 3 ))
dictPrint(it)
sep()
print('i testName retune centerZ centerX radius')
for retune in it['retune']:
    for centerZ in it['centerZ']:
        for centerX in it['centerX']:
            sep()
            it['radius'] = list(np.linspace( snoutLength - centerZ + 40, vesselLength - centerZ - 40, 4 ))
            for radius in it['radius']:

                # set test name
                testName = 'sim-test'
                varList = [retune,centerZ,centerX,radius]
                for v in varList: testName += f'__{round(v,1)}'
                print(i,testName,varList)
                outRoot = testName+".root"

                # open new compact file
                compactOutName = 'compact/drich.xml'
                compactOutFile = open(athenaDir+'/'+compactOutName,'w')

                # read original compact file, altering specified settings
                inSphere = False
                compactInFile.seek(0,0)
                for line in compactInFile.readlines():
                    if '<sphere' in line: inSphere = True
                    if inSphere:
                        line = re.sub('radius=".*"',f'radius="{radius}*cm"',line)
                        line = re.sub('centerx=".*"',f'centerx="{centerX}*cm"',line)
                        line = re.sub('centerz=".*"',f'centerz="{centerZ}*cm"',line)
                    if '/' in line: inSphere = False
                    line = re.sub('focus_retune=".*"',f'focus_retune="{retune}"',line)
                    compactOutFile.write(line)
                compactOutFile.close()

                # run npsim, and process results
                # TODO: make multithreaded (/run/useMaximumLogicalCores)
                def execute(l): subprocess.call(shlex.split(l))
                execute(f"./simulate.py -t4 -r -n50 -o {outRoot}")
                execute(f"root -b -q drawHits.C'(\"out/{outRoot}\")'")

                i+=1

compactInFile.close()


# iteration 1:
#it['retune'] = list(np.linspace( 0.0, 0.6, 4 ))
#it['centerZ'] = list(np.linspace( -90, 30, 5 ))
#it['centerX'] = list(np.linspace( tankRadius-20, tankRadius+20, 3 ))
#it['radius'] = list(np.linspace( snoutLength - centerZ + 10, vesselLength - centerZ - 10, 4 ))

# best:
# 0.2__-30.0__220.0__126.7

# iteration 2
#it['retune'] = list(np.linspace( 0.1, 0.3, 3 ))
#it['centerZ'] = list(np.linspace( -50, 0, 6 ))
#it['centerX'] = list(np.linspace( tankRadius+10, tankRadius+30, 3 ))
#it['radius'] = list(np.linspace( snoutLength - centerZ + 40, vesselLength - centerZ - 40, 4 ))
