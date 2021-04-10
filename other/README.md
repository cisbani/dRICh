# How To

Working notes and memo to run the codes for scatterbrains

## escalate
### Compile the g4e dRICh subdetector:

* define invironmental variables (use ejpm script)
  `source $HOME/.local/share/ejpm/env.sh`

* go into the directory of your g4e built:
 `cd /space/cisbani/escalate/g4e/g4e-master/build`

* now make everything ... (customize your directories !!)
 `cmake -w -DG4E_SILENCE_WARNINGS=1 -DCMAKE_INSTALL_PREFIX=/space/cisbani/escalate/g4e/g4e-master -DCMAKE_CXX_STANDARD=14 /space/cisbani/escalate/g4e/g4e-master && cmake --build . -- -j 4&& cmake --build . --target install`

### Run the simulator (customize your files)

`/space/cisbani/escalate/g4e/g4e-master/bin/g4e --gui /space/cisbani/escalate/myesc/drich.mac`
