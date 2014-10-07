# Tamito KAJIYAMA <26 September 2001>

from common_euc_jp import test

test("japanese.c.euc-jp")

# a test of the fix of a buffer overflow bug in japanese.c.euc-jp
for i in range(1000):
    u = unicode("\x8f\xec\xbf\x8f\xe2\xc7" * i, "japanese.c.euc-jp")
    u.encode("japanese.c.euc-jp")
