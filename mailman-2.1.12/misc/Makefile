# Copyright (C) 1998-2006 by the Free Software Foundation, Inc.
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
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
# USA.

# NOTE: Makefile.in is converted into Makefile by the configure script
# in the parent directory.  Once configure has run, you can recreate
# the Makefile by running just config.status.

# Variables set by configure


srcdir= 	.
bindir= 	${exec_prefix}/bin
prefix=   	/usr/local/mailman
exec_prefix=	${prefix}
var_prefix=	/usr/local/mailman
DESTDIR=

CC=		gcc
CHMOD=  	@CHMOD@
INSTALL=	/usr/bin/install -c
PYTHON=		/usr/bin/python

DEFS=   	-DPACKAGE_NAME=\"\" -DPACKAGE_TARNAME=\"\" -DPACKAGE_VERSION=\"\" -DPACKAGE_STRING=\"\" -DPACKAGE_BUGREPORT=\"\" -DHAVE_STRERROR=1 -DHAVE_SETREGID=1 -DHAVE_SYSLOG=1 -DSTDC_HEADERS=1 -DHAVE_SYS_TYPES_H=1 -DHAVE_SYS_STAT_H=1 -DHAVE_STDLIB_H=1 -DHAVE_STRING_H=1 -DHAVE_MEMORY_H=1 -DHAVE_STRINGS_H=1 -DHAVE_INTTYPES_H=1 -DHAVE_STDINT_H=1 -DHAVE_UNISTD_H=1 -DHAVE_SYSLOG_H=1 -DGETGROUPS_T=gid_t -DHAVE_VSNPRINTF=1

# Customizable but not set by configure

OPT=		-g -O2
CFLAGS=		$(OPT) $(DEFS)
PACKAGEDIR=	$(prefix)/Mailman
DATADIR=	$(var_prefix)/data
ICONDIR=	$(prefix)/icons
SCRIPTSDIR=	$(prefix)/scripts

SHELL=		/bin/sh
PYTHONLIBDIR=	$(prefix)/pythonlib
SETUPINSTOPTS=	--install-lib $(DESTDIR)$(PYTHONLIBDIR) \
		--install-purelib $(DESTDIR)$(PYTHONLIBDIR) \
		--install-data $(DESTDIR)$(PYTHONLIBDIR)
SETUPCMD=	setup.py --quiet install $(SETUPINSTOPTS)
PKGDIR= 	$(srcdir)

EMAILPKG=	
JACODECSPKG=	JapaneseCodecs-1.4.11
KOCODECSPKG=	KoreanCodecs-2.0.5

PACKAGES= $(EMAILPKG) $(JACODECSPKG) $(KOCODECSPKG)

# Modes for directories and executables created by the install
# process.  Default to group-writable directories but
# user-only-writable for executables.
DIRMODE=	775
EXEMODE=	755
FILEMODE=	644
DATAMODE=	664
INSTALL_PROGRAM=$(INSTALL) -m $(EXEMODE)

PATHSDIRS=	bin cron scripts tests
ICONS=		$(srcdir)/*.jpg $(srcdir)/*.png

# Rules

all:

install: install-other install-packages

install-other:
	for i in $(ICONS); \
	do \
	  $(INSTALL) -m $(FILEMODE) $$i $(DESTDIR)$(ICONDIR); \
	done
	for d in $(PATHSDIRS); \
	do \
	    dir=$(DESTDIR)$(prefix)/$$d; \
	    $(INSTALL) -m $(FILEMODE) paths.py $$dir; \
	done
	$(INSTALL) -m $(EXEMODE) mailman $(DESTDIR)$(SCRIPTSDIR)
	$(INSTALL) -m $(FILEMODE) sitelist.cfg $(DESTDIR)$(DATADIR)

install-packages:
	if [ -z "$(EMAILPKG)" -a -d $(PYTHONLIBDIR)/email ] ; \
	then \
	    rm -rf $(PYTHONLIBDIR)/email* ; \
	fi
	for p in $(PACKAGES); \
	do \
	    gunzip -c $(srcdir)/$$p.tar.gz | (cd $(PKGDIR) ; tar xf -); \
	    (cd $(PKGDIR)/$$p ; umask 02 ; PYTHONPATH=$(PYTHONLIBDIR) $(PYTHON) $(SETUPCMD)); \
	done

finish:

clean:

distclean:
	-rm Makefile paths.py mailman
	-rm -rf $(PACKAGES)
