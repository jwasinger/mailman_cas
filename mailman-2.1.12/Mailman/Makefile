# Copyright (C) 1998-2003 by the Free Software Foundation, Inc.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software 
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

# NOTE: Makefile.in is converted into Makefile by the configure script
# in the parent directory.  Once configure has run, you can recreate
# the Makefile by running just config.status.

# Variables set by configure

VERSION=	@VERSION@


srcdir= 	.
bindir= 	${exec_prefix}/bin
prefix=   	/usr/local/mailman
exec_prefix=	${prefix}
DESTDIR=

CC=		gcc
CHMOD=  	@CHMOD@
INSTALL=	/usr/bin/install -c

DEFS=   	-DPACKAGE_NAME=\"\" -DPACKAGE_TARNAME=\"\" -DPACKAGE_VERSION=\"\" -DPACKAGE_STRING=\"\" -DPACKAGE_BUGREPORT=\"\" -DHAVE_STRERROR=1 -DHAVE_SETREGID=1 -DHAVE_SYSLOG=1 -DSTDC_HEADERS=1 -DHAVE_SYS_TYPES_H=1 -DHAVE_SYS_STAT_H=1 -DHAVE_STDLIB_H=1 -DHAVE_STRING_H=1 -DHAVE_MEMORY_H=1 -DHAVE_STRINGS_H=1 -DHAVE_INTTYPES_H=1 -DHAVE_STDINT_H=1 -DHAVE_UNISTD_H=1 -DHAVE_SYSLOG_H=1 -DGETGROUPS_T=gid_t -DHAVE_VSNPRINTF=1

# Customizable but not set by configure

OPT=		-g -O2
CFLAGS=		$(OPT) $(DEFS)
PACKAGEDIR= 	$(prefix)/Mailman
SHELL=		/bin/sh

MODULES=	$(srcdir)/*.py
SUBDIRS=	Cgi Logging Archiver Handlers Bouncers Queue MTA Gui Commands

# Modes for directories and executables created by the install
# process.  Default to group-writable directories but
# user-only-writable for executables.
DIRMODE=	775
EXEMODE=	755
FILEMODE=	644
INSTALL_PROGRAM=$(INSTALL) -m $(EXEMODE)


# Rules

all:
	for d in $(SUBDIRS); \
	do \
	    (cd $$d; $(MAKE)); \
	done

install-here: 
	for f in $(MODULES); \
	do \
	    $(INSTALL) -m $(FILEMODE) $$f $(DESTDIR)$(PACKAGEDIR); \
	done
	$(INSTALL) -m $(FILEMODE) mm_cfg.py.dist $(DESTDIR)$(PACKAGEDIR)
	if [ ! -f $(DESTDIR)$(PACKAGEDIR)/mm_cfg.py ]; \
	then \
	    $(INSTALL) -m $(FILEMODE) mm_cfg.py.dist $(DESTDIR)$(PACKAGEDIR)/mm_cfg.py; \
	fi

install: install-here
	for d in $(SUBDIRS); \
	do \
	    (cd $$d; $(MAKE) DESTDIR=$(DESTDIR) install); \
	done

finish:
	@for d in $(SUBDIRS); \
	do \
	    (cd $$d; $(MAKE) DESTDIR=$(DESTDIR) finish); \
	done

clean:
	for d in $(SUBDIRS); \
	do \
	    (cd $$d; $(MAKE) clean); \
	done

distclean:
	-rm Makefile Defaults.py mm_cfg.py.dist
	-rm *.pyc
	for d in $(SUBDIRS); \
	do \
	    (cd $$d; $(MAKE) distclean); \
	done
