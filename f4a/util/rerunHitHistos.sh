#!/bin/bash
# dev script to quickly re-run hitHistos.C

root -b -q drich/macros/hitHistos.C'("out/dRichTree.root")' && mv -v {,out/}dRichHists.root
