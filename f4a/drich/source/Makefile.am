AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = \
  -I$(includedir) \
  -I$(OFFLINE_MAIN)/include \
  -I$(ROOTSYS)/include \
  -I$(G4_MAIN)/include/ \
  -I$(G4_MAIN)/include/Geant4 \
  -I$(DRICH_HOME)/share/source \
  -I$(DRICH_HOME)/esc/ci_DRICH

AM_LDFLAGS = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  -L$(OFFLINE_MAIN)/lib64

pkginclude_HEADERS = \
  dRIChSubsystem.h\
	dRIChTree.h

lib_LTLIBRARIES = \
  libdRICh.la

libdRICh_la_SOURCES = \
  dRIChSubsystem.cc\
  dRIChDetector.cc\
	dRIChHit.cc\
  dRIChSteppingAction.cc\
	dRIChTree.cc

libdRICh_la_LIBADD = \
  -lphool \
  -lSubsysReco\
  -lg4detectors\
  -lg4testbench 

BUILT_SOURCES = testexternals.cc

noinst_PROGRAMS = \
  testexternals

testexternals_SOURCES = testexternals.cc
testexternals_LDADD   = libdRICh.la

testexternals.cc:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

clean-local:
	rm -f $(BUILT_SOURCES)
