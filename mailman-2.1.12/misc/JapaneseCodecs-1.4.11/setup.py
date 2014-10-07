#!/usr/bin/env python
# Tamito KAJIYAMA <30 November 2000>
# $Id: setup.py,v 1.14 2004/11/28 10:07:12 kajiyama Exp $

from distutils.core import setup, Extension
from distutils.command.build_py import build_py

class Build_py(build_py):
    user_options = build_py.user_options + [
        ('with-aliases', None, "register aliases [default]"),
        ("without-aliases", None, "don't register aliases"),
        ]
    boolean_options = build_py.boolean_options + [
        "with-aliases", "without-aliases",
        ]
    negative_opt = {"without-aliases": "with-aliases"}
    def initialize_options (self):
        build_py.initialize_options(self)
        self.with_aliases = 1
    def finalize_options (self):
        build_py.finalize_options(self)
        if self.with_aliases:
            self.packages.append('japanese.aliases')

import os, sys

if os.sep == '/':
    sitedir = os.path.join("lib", "python" + sys.version[:3], "site-packages")
elif os.sep == ':':
    sitedir = os.path.join("lib", "site-packages")
else:
    if sys.version_info[0:3] >= (2, 2, 0):
        sitedir = os.path.join("lib", "site-packages")
    else:
        sitedir = "."

DESCRIPTION = """\
This package provides Unicode codecs that make Python aware
of Japanese character encodings such as EUC-JP, Shift_JIS and
ISO-2022-JP.  By using this package, Japanese characters can be
treated as a character string instead of a byte sequence."""

LICENSE = "a variant of the BSD license"

setup (name = "JapaneseCodecs",
       version = "1.4.11",
       description = "Japanese Codecs for Python Unicode Support",
       long_description = DESCRIPTION,
       author = "Tamito KAJIYAMA",
       author_email = "RD6T-KJYM@asahi-net.or.jp",
       url = "http://www.asahi-net.or.jp/~rd6t-kjym/python/",
       license = LICENSE, licence = LICENSE,
       platforms = ["anywhere Python runs"],
       cmdclass = {'build_py': Build_py},
       packages = ['japanese',
                   'japanese.python',
                   'japanese.c',
                   'japanese.mappings'],
       data_files = [(sitedir, ['japanese.pth'])],
       ext_modules = [
           Extension("japanese.c._japanese_codecs",
                     ["src/_japanese_codecs.c"])])
