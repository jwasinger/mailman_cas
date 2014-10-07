#!/usr/bin/env python
#
# This file is part of KoreanCodecs.
#
# Copyright(C) Hye-Shik Chang <perky@FreeBSD.org>, 2002.
#
# KoreanCodecs is free software; you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published
# by the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# KoreanCodecs is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with KoreanCodecs; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#
# $Id: setup.py,v 1.29 2002/07/23 18:25:31 perky Exp $
#

import sys
from distutils.core import setup, Extension
from distutils.command.install import install

flavors = {
    'aliases': 1,
    'extension': 1,
}
for flname in flavors.keys():
    if '--without-'+flname in sys.argv:
        sys.argv.remove('--without-'+flname)
        flavors[flname] = 0
    if '--with-'+flname in sys.argv:
        sys.argv.remove('--with-'+flname)
        flavors[flname] = 1

class Install(install):
    def initialize_options (self):
        install.initialize_options(self)
        if flavors['aliases']:
            if sys.hexversion >= 0x2010000:
                self.extra_path = ("korean", "import korean.aliases")
            else:
                self.extra_path = "korean"
    def finalize_options (self):
        org_install_lib = self.install_lib
        install.finalize_options(self)
        self.install_libbase = self.install_lib = \
            org_install_lib or self.install_purelib

setup (name = "KoreanCodecs",
       version = "2.0.5",
       description = "Korean Codecs for Python Unicode Support",
       long_description = "This package provides Unicode codecs that "
            "make Python aware of Korean character encodings such as "
            "EUC-KR, CP949 and ISO-2022-KR. By using this package, "
            "Korean characters can be treated as a character string "
            "instead of a byte sequence.",
       author = "Hye-Shik Chang",
       author_email = "perky@FreeBSD.org",
       license = "LGPL",
       url = "http://sourceforge.net/projects/koco",
       cmdclass = {'install': Install},
       packages = ['korean',
                   'korean.mappings',
                   'korean.c',
                   'korean.python'],
       ext_modules = flavors['extension'] and [
           Extension("korean.c._koco", ["src/_koco.c"]),
           Extension("korean.c.hangul", ["src/hangul.c"]),
       ] or [])
