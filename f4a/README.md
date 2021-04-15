# dRICh Fun4All porting

- if you have the fun4all singularity image, start a new container
  - you can do this with `util/startFun4all.sh /path/to/fun4all/Singularity`,
    where the path points to [Fun4all Singularity](https://github.com/eic/Singularity.git);
    this script will source `eic_setup.sh`
- build the dRICh by running `util/buildDetector.sh` or `util/buildDetector.sh clean`
  - libraries and headers will appear in `./install`, if successful
- set environment variables with `source env.sh`
  - `echo $LD_LIBRARY_PATH` should include `./install/lib`
- open a Qt GUI and shoot particle(s) with `util/shootPion.sh`
- use the command `/Fun4all/run 1` to shoot one particle and visualize

## changelog for shared code, or ESCalate implementation
This is a summary of changes needed in code outside this
directory, in order to allow the `fun4all` implementation to
build and run and not complain. Hopefully they won't
conflict with the `ESCalate` implementation development.
- `g4dRIChOptics.hh`
  - replace `fmt` dependence with `stdio` functions
    - `fmt` is not in `fun4all` singularity container;
      this is just a quick fix
  - `setOpticalParams` are empty virtual methods with
    non-void return
    - fix: added return dummy value `-1`
  - in `g4dRIChAerogel`, mode 0 (Vorobiev), `rho` has no
    value assigned to it, but it is needed later in
    `scaledE` and `scaledS`
    - quick fix: initialize `rho` with dummy value `0`
    - mode 3 (CLAS12) is currently being used
- `ci_DRICH_Config.hh`
  - comment out `InitializationContext.hh` include directive
    (not sure what this is)
  - add `G4GenericMessenger.hh` include directive
  - should this header be moved to `share/source`?
