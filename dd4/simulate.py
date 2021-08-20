#!/usr/bin/env python

import sys, getopt, os, re
import subprocess, shlex
import math


# ARGUMENTS
#####################

testNum = -1 
detector = 'drich'
particle = 'pi+'
runType = 'run'
numEvents = 10

helpStr = f'''
{sys.argv[0]} <TESTNUM> [OPTIONS]

<REQUIRED ARGUMENTS>:

    TESTNUM:    -t <testnum>: specify which test to run
    
            10:   focal point, in dRICh acceptance
                    ( recommend: optDbg=1 / mirDbg=0 / sensDbg=1 )
            11:   focal point, broad range test
                    ( recommend: optDbg=1 / mirDbg=1 / sensDbg=1 )

[OPTIONAL ARGUMENTS]

    OPTIONS:    -a: athena full simulation
                -d: dRICh only (default)
                -p [particle]: name of particle to throw
                    - pi+
                    - opticalphoton
                -n [numEvents]: number of events (usu. at each point)
                -v: visualize, instead of run
    '''

if(len(sys.argv)<=1):
    print(helpStr)
    sys.exit(2)
try: opts, args = getopt.getopt(sys.argv[1:],'t:adp:n:vh')
except getopt.GetoptError:
    print('\n\nERROR: invalid argument\n',helpStr)
    sys.exit(2)
for opt, arg in opts:
    if(opt=='-t'): testNum = int(arg)
    if(opt=='-a'): detector = 'athena'
    if(opt=='-d'): detector = 'drich'
    if(opt=='-p'): particle = arg
    if(opt=='-n'): numEvents = int(arg)
    if(opt=='-v'): runType = 'vis'
    if(opt=='-h'):
        print(helpStr)
        sys.exit()

if(testNum<0):
    print('\n\nERROR: test number required, e.g. `-t3`\n',helpStr)
    sys.exit(2)

### override settings for optics test
if(testNum>=10):
    print("optics test, overriding some settings...")
    particle='opticalphoton'
    runType='vis'


sep='-'*40
print(sep)
print(f'testNum = {testNum}')
print(f'detector = {detector}')
print(f'particle = {particle}')
print(f'numEvents = {numEvents}')
print(f'runType = {runType}')
print(sep)


# set compact file
if(detector=="athena"):
    compactFile='athena.xml'
elif(detector=="drich"):
    compactFile='compact/subsystem_views/drich_only.xml'
else:
    print('ERROR: unknown detector\n')
    sys.exit(2)

### start macro file
workDir = os.getcwd()
m = open(workDir+"/macro/tmp.mac",'w+')

### common settings
macroCommon = open(workDir+"/macro/"+runType+"_common.mac",'r')
for line in macroCommon: m.write(line)
macroCommon.close()

### set particle energy
energy = '8.0 GeV'
if(particle=="opticalphoton"): energy = '3.0 eV'

### append particle info
m.write(f'/gps/verbose 2\n')
m.write(f'/gps/particle {particle}\n')
m.write(f'/gps/number 1\n')
m.write(f'/gps/ene/mono {energy}\n')
#m.write(f'/gps/ene/type Gauss\n')
#m.write(f'/gps/ene/sigma 3.0 GeV\n')

### append source settings
m.write(f'/gps/position 0 0 0 cm\n')

### define envelope acceptance limits [units=cm]
padding = 5.0
rMin = 19.0 - padding
rMax = 200.0 - padding
zMax = 335.0

# derived attributes
thetaMin = math.atan2(rMin,zMax)
thetaMax = math.atan2(rMax,zMax)
etaMin = -math.log(math.tan(0.5*thetaMin))
etaMax = -math.log(math.tan(0.5*thetaMax))
print('ENVELOPE\n')
print(f'rMin = {rMin} cm')
print(f'rMax = {rMax} cm')
print(f'zMax = {zMax} cm\n')
print(f'thetaMin = {math.degrees(thetaMin)} deg')
print(f'thetaMax = {math.degrees(thetaMax)} deg\n')
print(f'etaMin = {etaMin}')
print(f'etaMax = {etaMax}\n')
print(sep)



### TESTS ############################

if( testNum == 1 ):
    m.write(f'\n# aim at +x dRICh sector\n')
    m.write(f'/gps/direction 0.25 0.0 0.0\n')
    m.write(f'/run/beamOn {numEvents}\n')

elif( testNum == 10 ):
    m.write(f'\n# opticalphoton scan test, drich range\n')
    m.write(f'/vis/scene/endOfEventAction accumulate {numEvents}\n')
    m.write(f'/gps/pos/type Point\n')
    m.write(f'/gps/pos/radius 0.1 mm\n')
    m.write(f'/gps/ang/type iso\n')
    m.write(f'/gps/ang/mintheta {math.pi-thetaMax} rad\n')
    m.write(f'/gps/ang/maxtheta {math.pi-thetaMin} rad\n')
    m.write(f'/gps/ang/minphi {math.pi} rad\n')
    m.write(f'/gps/ang/maxphi {math.pi+0.01} rad\n')
    m.write(f'/run/beamOn {numEvents}\n')

elif( testNum == 11 ):
    m.write(f'\n# opticalphoton scan test, broad range\n')
    m.write(f'/vis/scene/endOfEventAction accumulate {numEvents}\n')
    m.write(f'/gps/pos/type Point\n')
    m.write(f'/gps/pos/radius 0.1 mm\n')
    m.write(f'/gps/ang/type iso\n')
    m.write(f'/gps/ang/mintheta {math.pi/2} rad\n')
    m.write(f'/gps/ang/maxtheta {math.pi-thetaMin} rad\n')
    m.write(f'/gps/ang/minphi {math.pi} rad\n')
    m.write(f'/gps/ang/maxphi {math.pi+0.01} rad\n')
    m.write(f'/run/beamOn {numEvents}\n')

else:
    print("ERROR: unknown test number\n")
    m.close()
    sys.exit(2)

#########################################################
# MACRO FILE BUILT


# print macro
m.seek(0,0)
print(m.read())
print(sep)

# set output file
outputFile = workDir+"/out/sim_"+runType+".root"

# simulation executable and arguments
cmd = "npsim"
cmd += " --runType " + runType
cmd += " --compactFile " + compactFile
cmd += " --random.seed 1 "
cmd += " --macro " + m.name
cmd += " --outputFile " + outputFile
cmd += " --enableG4GPS"

# run simulation
subprocess.call( shlex.split(cmd), cwd = os.environ['DRICH_DD4_ATHENA'] )
m.close()
