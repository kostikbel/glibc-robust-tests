This is a quick-done port of glibc nptl robustness tests to FreeBSD.  Used to test the libthr implementation of the
robustness.  As such, all code there is implicitely under the glibc licence, which to my unedicated view is LGPL 2.1.
Go figure their LICENSES/COPYING.

The rh-pr628608.cc was taken from the RedHat bug 628608, https://bugzilla.redhat.com/show_bug.cgi?id=628608 .

If using locally, look at the GNUmakefile.  You might want to change CC/CXX.
