# dRICh Fun4All porting

- `util/startFun4all.sh [(optional)/path/to/fun4all/repo]`
- `util/buildDetector.sh` or `util/buildDetector.sh clean`
- `source env.sh`
  - `echo $LD_LIBRARY_PATH` should include `./install/lib`
- `util/gui.sh`
- `QTGui()`

## changelog for shared code
- `g4dRIChOptics.hh`
  - replace `fmt` dependence with `stdio` functions
    - `fmt` is not in `fun4all` singularity container;
      this is just a quick fix
  - empty virtual methods with non-void return values have
    been converted to pure virtuals
    - change `virtual int func(){};` to `virtual int func()=0;`
