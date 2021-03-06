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
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

# NOTE: Makefile.in is converted into Makefile by the configure script
# in the parent directory.  Once configure has run, you can recreate
# the Makefile by running just config.status.

# Variables set by configure


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
MAILDIR=	$(exec_prefix)/mail
SCRIPTSDIR= 	$(prefix)/bin

SHELL=		/bin/sh

SCRIPTS=	mmsitepass newlist rmlist add_members \
		list_members remove_members clone_member update arch \
		sync_members check_db withlist check_perms find_member \
		version config_list list_lists dumpdb cleanarch \
		list_admins genaliases change_pw mailmanctl qrunner inject \
		unshunt fix_url.py convert.py transcheck b4b5-archfix \
		list_owners msgfmt.py show_qfiles discard rb-archfix \
		reset_pw.py export.py

BUILDDIR=	../build/bin

# Modes for directories and executables created by the install
# process.  Default to group-writable directories but
# user-only-writable for executables.
DIRMODE=	775
EXEMODE=	755
FILEMODE=	644
INSTALL_PROGRAM=$(INSTALL) -m $(EXEMODE)


# Rules

all:

install:
	for f in $(SCRIPTS); \
	do \
	    $(INSTALL) -m $(EXEMODE) $(BUILDDIR)/$$f $(DESTDIR)$(SCRIPTSDIR); \
	done

finish:

clean:

distclean:
	-rm Makefile
