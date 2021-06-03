#!/bin/bash
# dev script to quickly re-run hitHistos.C

root -b -q drich/macros/hitHistos.C'("out/dRIChTree.root")' && mv -v {,out/}dRIChHists.root
