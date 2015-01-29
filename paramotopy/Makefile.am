#use the local m4 folder.
ACLOCAL_AMFLAGS = -I m4

AM_YFLAGS = -d -p `basename $* | sed 's,y$$,,'`
AM_LFLAGS = -s -P`basename $* | sed 's,l$$,,'` -olex.yy.c

AM_CPPFLAGS = -I$(top_srcdir)/include


#set up the empty variables
bin_PROGRAMS =
BUILT_SOURCES =
CLEANFILES =
noinst_LIBRARIES =



include_HEADERS =


SUFFIXES = .c .h .y .l
#see https://www.gnu.org/software/automake/manual/html_node/Suffixes.html



#now include the Makemodule files from the subdirectories.  they will add on to the variables which are created in the top-level Makefile.am

include src/tinyxml/Makemodule.am
include src/paramotopy/Makemodule.am
include src/step2/Makemodule.am
include src/body/Makemodule.am







