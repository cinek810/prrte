#
# Copyright (c) 2015-2019 Intel, Inc.  All rights reserved.
# Copyright (c) 2020      Cisco Systems, Inc.  All rights reserved
# $COPYRIGHT$
#
# Additional copyrights may follow
#
# $HEADER$
#

# main library setup
noinst_LTLIBRARIES = libmca_schizo.la
libmca_schizo_la_SOURCES =

# pkgdata setup
dist_prtedata_DATA =

# local files
headers = schizo.h
libmca_schizo_la_SOURCES += $(headers)

# Conditionally install the header files
if WANT_INSTALL_HEADERS
prtedir = $(prteincludedir)/$(subdir)
nobase_prte_HEADERS = $(headers)
endif

include base/Makefile.am

distclean-local:
	rm -f base/static-components.h
