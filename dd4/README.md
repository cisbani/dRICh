# DD4HEP Port

This directory contains support scripts for the `dd4hep` port development

The code itself is [integrated in ATHENA](https://eicweb.phy.anl.gov/EIC/detectors/athena/-/tree/master)


# Quick Start

- Obtain the `eic_container` Singularity image (see below)
  and clone the `EIC/detectors/athena` repository
- Edit paths in `environ.sh` for your setup
- `startEICshell.sh` to start a shell in the container
- `source environ.sh` to set the environment (many scripts call this
  automatically)
- `buildATHENA.sh` to build the detector(s), and runs `dd_web_display` to
  produces a `root` file in the `athena` directory
- to view this file, open an instance of `jsroot` 
  (e.g., [here](https://eic.phy.anl.gov/geoviewer/)) and open the 
  resulting `root` file
- see [Shared Tools](#tools) section below for further information on 
  available development tools
- some [useful tables](#info) are also stored in this repository


# Installation notes for personal computer

- Create a separate directory, which will contain
  development source code and installations of the
  Singularity image

- Get container installation script, either from `wget` or
  `git`:
  - `wget
    https://eicweb.phy.anl.gov/containers/eic_container/-/raw/master/install.sh`
  - `git clone
    https://eicweb.phy.anl.gov/containers/eic_container.git`

- Create install directory, and move or symlink `install.sh`
  there:
  - `mkdir install; cd install`
  - `ln -s ../eic_container/install.sh ./`

- Run `install.sh` from within this `install` directory
  - run `install.sh -h` to see if there are options that may
    be useful for you, for example, if you are low on disk
    space on `/`, consider using `-t` to set a different tmp
    directory

- continue with setup directions for ATHENA full simulations

- Return to this repository, edit `environ.sh` according to
  your setup, then run `startEICshell.sh`. You are now in an
  `eic-shell` instance from which you can execute these
  development tools


<a name="tools"></a>
# Tools

Note: to build C++ code, type `make` (best if you are in the Singularity
container) which will produce `*.exe` executables

- `buildAthena.sh`: builds the full ATHENA detector, or just the dRICh
- `drawHits.cpp` and `drawSegmentation.cpp`
  - both of these are used to explore the dRICh hits readout, and
    represent examples of how to analyze the simulation output
- `overlapCheck.sh`: checks for geometry overlaps (often it is better
  to let the `athena` CI do this)
- `searchCompactParams.sh`: search for a parameter in all the `athena`
  compact files, given a keyword (case sensitive), and return the 
  calculated values; this is useful for quickly checking parameterizations
- `simulate.py`: run a variety of simulations for testing the dRICh; run
  this script with no arguments for usage and details
- `startEICshell.sh`: start a shell within the Singularity container


<a name="info"></a>
# Stored Information

Some information, such as Lookup Tables (LUTs), are stored in `text/`

- `materialDump.txt`: material/optical property tables for all materials
  used in the dRICh
  - this is obtained from the `fun4all` implementation
  - see `../share/source/g4dRIChOptics.hh`, calls to
    `G4MaterialPropertyTable::DumpTable()`
  - these tables are in a format to facilitate porting them to the 
    compact files
- `sensorLUT.dat`: convert sensor module number to (x,y) position
  (units=cm), for the sector on the +x axis
  - this is needed by `drawSegmentation.cpp`, although it may be possible
    to remove this dependency

