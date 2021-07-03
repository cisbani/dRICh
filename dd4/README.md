Developer's scripts for `dd4hep` port

# Setup

- Obtain the `eic_container` Singularity image (see below) and clone the 
`EIC/detectors/athena` repository
- Edit paths in `environ.sh` for your setup
- `startEICshell.sh`
- `source environ.sh`
- `buildATHENA.sh`
  - also runs `dd_web_display` and produces a `root` file in the `athena`
    directory
- [https://eic.phy.anl.gov/geoviewer/](https://eic.phy.anl.gov/geoviewer/)
  and open resulting `root` file


# Installation notes for personal computer:

Get container installation script, either from `wget` or `git`:
- `wget https://eicweb.phy.anl.gov/containers/eic_container/-/raw/master/install.sh`
- `git clone https://eicweb.phy.anl.gov/containers/eic_container.git`

Create install directory, and move or symlink `install.sh` there:
- `mkdir install; cd install`
- `ln -s ../eic_container/install.sh ./`
Run `install.sh` from within this `install` directory
  - if you are low on disk space on `/`, change the temp directory for
    singularity (which defaults to `/tmp`): e.g., 
    `export SINGULARITY_TMPDIR=$HOME/tmp`
    - you may also need to set `TMPDIR`
  - if you have `/cfmvs` access from your local computer, you may have
    to disable `cvmfs` detection in `install.sh` (change `if [ -d
    /cvmfs/...]` to `if false; then`)

Execute `eic-shell`
- see also `startEICshell.sh`, in case of environment conflicts
