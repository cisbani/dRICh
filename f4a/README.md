# dRICh Fun4All porting

- `util/startFun4all.sh [(optional)/path/to/fun4all/repo]`
- `util/buildDetector.sh` or `util/buildDetector.sh clean
- `source env.sh`
  - `echo $LD_LIBRARY_PATH` should include `./install/lib`
- `pushd drich/macros`
- `root -l gui.C`
- `QTGui()`
