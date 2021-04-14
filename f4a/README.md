# dRICh Fun4All porting

- `util/startFun4all.sh [(optional)/path/to/fun4all/repo]`
- `util/buildDetector.sh` or `util/buildDetector.sh clean`
- `source env.sh`
  - `echo $LD_LIBRARY_PATH` should include `./install/lib`
- `util/gui.sh`
- `QTGui()`

## changelog for shared code, or ESCalate implementation
This is a summary of changes needed in code outside this
directory, in order to allow the `fun4all` implementation to
build and run and not complain. Hopefully they won't
conflict with the `ESCalate` implementation development.
- `g4dRIChOptics.hh`
  - replace `fmt` dependence with `stdio` functions
    - `fmt` is not in `fun4all` singularity container;
      this is just a quick fix
  - empty virtual methods with non-void return values have
    been converted to pure virtuals
    - change `virtual int func(){};` to `virtual int func()=0;`
- `ci_DRICH_Config.hh`
  - comment out `InitializationContext.hh` include directive
    (not sure what this is)
  - add `G4GenericMessenger.hh` include directive
  - should this header be moved to `share/source`?
