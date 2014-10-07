# Copyright (C) 2001-2007 by the Free Software Foundation, Inc.
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
#
# NOTE: You should ordinarily never need to run "make catalogs", but
# if you find you do, you will need GNU make, and the GNU gettext
# suite (e.g. msgfmt, msgmerge).

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
PACKAGEDIR= 	$(prefix)/messages
SHELL=		/bin/sh
DIRSETGID=	chmod g+s
MSGFMT= 	/usr/bin/python ../build/bin/msgfmt.py
MSGMERGE=	msgmerge

# CVS available languages
LANGUAGES=	ar ca cs da de es et eu fi fr gl he hr hu ia it ja ko lt nl \
		no pl pt pt_BR ro ru sk sl sr sv tr uk vi zh_CN zh_TW
LANGDIRS=	$(LANGUAGES:%=messages/%/LC_MESSAGES)
# Human readable po file
POFILES=	$(LANGUAGES:%=%/LC_MESSAGES/mailman.po)
# Binary generated mo file
MOFILES=	$(LANGUAGES:%=%/LC_MESSAGES/mailman.mo)
TARGETS=	$(MOFILES)

# Modes for directories and executables created by the install
# process.  Default to group-writable directories but
# user-only-writable for executables.
DIRMODE=	775
EXEMODE=	755
FILEMODE=	644
INSTALL_PROGRAM=$(INSTALL) -m $(EXEMODE)
PROG=		/usr/bin/python build/bin/pygettext.py

.SUFFIXES:	.po .mo
.po.mo:
	-$(MSGFMT) -o $@ $<

# Rules

all: mofiles

catalogs: $(TARGETS)

check:
	@for file in $(POFILES); \
	do \
		echo "Checking $$file"; \
		msgfmt -o /dev/null --statistics -v $$file; \
	done

install: doinstall

doinstall: mofiles
	@for d in $(LANGDIRS); \
	do \
	    dir=$(DESTDIR)$(prefix)/$$d; \
	    echo "Creating language directory $$dir"; \
	    $(srcdir)/../mkinstalldirs $$dir; \
	    chmod $(DIRMODE) $$dir; \
	    $(DIRSETGID) $$dir; \
	done
	@for d in $(LANGUAGES); \
	do \
	    readme=$(srcdir)/$$d/README.$$d; \
	    if test -f $$readme; \
	    then \
		$(INSTALL) -m $(FILEMODE) $$readme $(DESTDIR)$(prefix)/messages/$$d; \
	    fi; \
	done
	@for d in $(LANGUAGES); \
	do \
	    po=$(srcdir)/$$d/LC_MESSAGES/mailman.po; \
	    mo=$(srcdir)/$$d/LC_MESSAGES/mailman.mo; \
	    dir=$(DESTDIR)$(prefix)/messages/$$d/LC_MESSAGES; \
	    $(INSTALL) -m $(FILEMODE) $$po $$dir; \
 	    $(INSTALL) -m $(FILEMODE) $$mo $$dir; \
	done

mofiles: $(MOFILES)

finish:

clean:
	-rm -f */LC_MESSAGES/mailman.mo

fileclean:
	-rm -f marked.files docstring.files

potclean: fileclean
	-rm -f Makefile $(POTFILE)

distclean: clean potclean
	-rm -f Makefile

marked.files:
	@echo "Calculating marked input files for pygettext"
	(cd ..; find Mailman -path '*pythonlib' -prune -o -path '*Commands/cmd_*.py' -prune -o -name '*.py' -print >| messages/$@)

docstring.files:
	@echo "Calculating script input files for pygettext"
	(cd ..; grep -d skip -l -i '^#! .*python.*' `find bin cron scripts -name ".#*" -prune -o -print` | grep -v pygettext >| messages/$@)
	(cd ..; ls Mailman/Commands/cmd_*.py >> messages/$@)

potfile: marked.files docstring.files
	@echo "Running pygettext on $@; this make take a while."
	(cd ..; $(PROG) -p messages -d mailman -D -X messages/marked.files `cat messages/marked.files messages/docstring.files`)

# Update the individual mailman.po files with the new changes to the
# .pot file
%/LC_MESSAGES/mailman.po: mailman.pot
	@echo "Merging new template file with existing translations"
	$(MSGMERGE) -U $@ mailman.pot || touch $@


FORCE:
