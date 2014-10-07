# Tamito KAJIYAMA <12 June 2001>
# $Id: hgen.py,v 1.3 2002/10/07 16:49:24 kajiyama Exp $

import sys
import os

progname = os.path.basename(sys.argv[0])

__version__ = "1.0"

# N is determined heuristically (must be >= 256)
N = 523

def read(filename, jis_column, ucs_column):
    file = open(filename)
    jis_map = []
    ucs_map = []
    for i in range(N):
        jis_map.append([])
        ucs_map.append([])
    while 1:
        line = file.readline()
        if not line:
            break
        if line[0] == '#':
            continue
        tokens = line.split()
        jis = int(tokens[jis_column], 16) | 0x8080
        ucs = int(tokens[ucs_column], 16)
        jis_map[jis % N].append((jis, ucs))
        ucs_map[ucs % N].append((ucs, jis))
    return jis_map, ucs_map

def dump(prefix, jis_map, ucs_map):
    for n in range(N):
        jis_map[n].sort()
        print "static unsigned char %s_jis_map_%d[] = {" % (prefix, n)
        print "    0x%02x," % len(jis_map[n])
        for jis, ucs in jis_map[n]:
            print "    0x%02x, 0x%02x, 0x%02x," % (jis/N, ucs/256, ucs%256)
        print "};"
    print "static unsigned char *%s_jis_map[] = {" % prefix
    for n in range(N):
        print "    %s_jis_map_%d," % (prefix, n)
    print "};"
    print
    for n in range(N):
        ucs_map[n].sort(lambda x, y: cmp((x[1], x[0]), (y[1], y[0])))
        print "static unsigned char %s_ucs_map_%d[] = {" % (prefix, n)
        print "    0x%02x," % len(ucs_map[n])
        for ucs, jis in ucs_map[n]:
            print "    0x%02x, 0x%02x, 0x%02x," % (ucs/N, jis/256, jis%256)
        print "};"
    print "static unsigned char *%s_ucs_map[] = {" % prefix
    for n in range(N):
        print "    %s_ucs_map_%d," % (prefix, n)
    print "};"

# Stuff not listed in CP932 table, but collected from Win32 API.
ms932_enc_appendix = {
#    ucs: sjis
    0xa1: 0x21,
    0xa6: 0x7c,
    0xa9: 0x63,
    0xaa: 0x61,
    0xab: 0x81e1,
    0xad: 0x2d,
    0xae: 0x52,
    0xaf: 0x8150,
    0xb2: 0x32,
    0xb3: 0x33,
    0xb5: 0x83ca,
    0xb7: 0x8145,
    0xb8: 0x8143,
    0xb9: 0x31,
    0xba: 0x6f,
    0xbb: 0x81e2,
    0xc0: 0x41,
    0xc1: 0x41,
    0xc2: 0x41,
    0xc3: 0x41,
    0xc4: 0x41,
    0xc5: 0x41,
    0xc6: 0x41,
    0xc7: 0x43,
    0xc8: 0x45,
    0xc9: 0x45,
    0xca: 0x45,
    0xcb: 0x45,
    0xcc: 0x49,
    0xcd: 0x49,
    0xce: 0x49,
    0xcf: 0x49,
    0xd0: 0x44,
    0xd1: 0x4e,
    0xd2: 0x4f,
    0xd3: 0x4f,
    0xd4: 0x4f,
    0xd5: 0x4f,
    0xd6: 0x4f,
    0xd8: 0x4f,
    0xd9: 0x55,
    0xda: 0x55,
    0xdb: 0x55,
    0xdc: 0x55,
    0xdd: 0x59,
    0xde: 0x54,
    0xdf: 0x73,
    0xe0: 0x61,
    0xe1: 0x61,
    0xe2: 0x61,
    0xe3: 0x61,
    0xe4: 0x61,
    0xe5: 0x61,
    0xe6: 0x61,
    0xe7: 0x63,
    0xe8: 0x65,
    0xe9: 0x65,
    0xea: 0x65,
    0xeb: 0x65,
    0xec: 0x69,
    0xed: 0x69,
    0xee: 0x69,
    0xef: 0x69,
    0xf0: 0x64,
    0xf1: 0x6e,
    0xf2: 0x6f,
    0xf3: 0x6f,
    0xf4: 0x6f,
    0xf5: 0x6f,
    0xf6: 0x6f,
    0xf8: 0x6f,
    0xf9: 0x75,
    0xfa: 0x75,
    0xfb: 0x75,
    0xfc: 0x75,
    0xfd: 0x79,
    0xfe: 0x74,
    0xff: 0x79,
    0x3094: 0x8394,
    0xf8f0: 0xa0,
    0xf8f1: 0xfd,
    0xf8f2: 0xfe,
    0xf8f3: 0xff,
}

def dump_ms932(sjisfile, j0208file):
    # build ms932 encode/decode map
    ms932 = open(sjisfile).readlines()
    ms932 = [l.split() for l in ms932 if l and l[0] != '#']
    ms932 = [(int(l[0],16), int(l[1], 16)) for l in ms932 if l and l[1][0] != '#']
    ms932_dec = {}
    ms932_enc = ms932_enc_appendix.copy()

    for mbcs, ucs in ms932:
        ms932_dec[mbcs] = ucs
        cur = ms932_enc.get(ucs, 0)
        if cur:
            # Decode to JIS 2-ku and 13-ku rather than IBMNEC/IBM gaiji.
            if cur >= 0x8800:
                ms932_enc[ucs] = mbcs
        else:
            ms932_enc[ucs] = mbcs

    # build JIS0208 encode/decode map
    j0208 = open(j0208file).readlines()
    j0208 = [l.split() for l in j0208 if l and l[0] != '#']
    j0208 = [(int(l[0],16), int(l[2], 16)) for l in j0208 if l and l[2][0] != '#']
    j0208_dec = {}
    j0208_enc = {}
    for mbcs, ucs in j0208:
        j0208_dec[mbcs] = ucs
        j0208_enc[ucs] = mbcs

    jis_map = []
    ucs_map = []

    for i in range(N):
        jis_map.append([])
        ucs_map.append([])

    # build ucs->sjis map
    for ucs, sjis in ms932_enc.iteritems():
        if ucs >= 0x80 and not (0xff61 <= ucs <= 0xff9f):
            if j0208_enc.get(ucs) != sjis:
                ucs_map[ucs % N].append((ucs, sjis))

    # build sjis->ucs map
    for sjis, ucs in ms932_dec.iteritems():
        if sjis >= 0x80 and not (0xa1 <= sjis <= 0xdf):
            if j0208_dec.get(sjis) != ucs:
                jis_map[sjis % N].append((sjis, ucs))

    dump("ms932", jis_map, ucs_map)

def main():
    table1_jis, table1_ucs = read("JIS0208.TXT", 1, 2)
    table2_jis, table2_ucs = read("JIS0212.TXT", 0, 1)
    print "/* This is an auto-generated file (by %s %s) */" % (
        progname, __version__)
    print "/* Do not edit!! */"
    print
    print "#define N", N
    print
    dump("jisx0208", table1_jis, table1_ucs)
    dump("jisx0212", table2_jis, table2_ucs)
    dump_ms932("MS932.TXT", "JIS0208.TXT")
    
if __name__ == "__main__":
    main()
