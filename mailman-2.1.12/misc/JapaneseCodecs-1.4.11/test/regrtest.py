#!/usr/bin/env python
# Tamito KAJIYAMA <19 December 2000>

import os
from test import regrtest

regrtest.STDTESTS = []
regrtest.main(testdir=os.getcwd())
