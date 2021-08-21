# DD4HEP Port

This directory contains support scripts for the `dd4hep` port development

The code itself is [integrated in ATHENA](https://eicweb.phy.anl.gov/EIC/detectors/athena/-/tree/master)


## Quick Start

- Obtain the `eic_container` Singularity image (see below)
  and clone the `EIC/detectors/athena` repository
- Edit paths in `environ.sh` for your setup
- `startEICshell.sh`
- `source environ.sh`
- `buildATHENA.sh`
  - also runs `dd_web_display` and produces a `root` file in
    the `athena` directory
- [https://eic.phy.anl.gov/geoviewer/](https://eic.phy.anl.gov/geoviewer/)
  and open resulting `root` file
- `checkOverlaps.sh` checks for overlaps


## Installation notes for personal computer

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
