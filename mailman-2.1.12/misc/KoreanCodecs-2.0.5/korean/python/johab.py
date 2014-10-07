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
# $Id: johab.py,v 1.9 2002/07/19 00:01:53 perky Exp $
#

import codecs

from korean.hangul import Jaeum, Moeum, ishangul, split, join
encmap, decmap = {}, {}

johab2uni_chosung = {
    1: u'',         2: Jaeum.G,     3: Jaeum.GG,    4: Jaeum.N,
    5: Jaeum.D,     6: Jaeum.DD,    7: Jaeum.L,     8: Jaeum.M,
    9: Jaeum.B,     10: Jaeum.BB,   11: Jaeum.S,    12: Jaeum.SS,
    13: Jaeum.NG,   14: Jaeum.J,    15: Jaeum.JJ,   16: Jaeum.C,
    17: Jaeum.K,    18: Jaeum.T,    19: Jaeum.P,    20: Jaeum.H,
}
johab2uni_jungsung = {
    2: u'',         3: Moeum.A,     4: Moeum.AE,    5: Moeum.YA,
    6: Moeum.YAE,   7: Moeum.EO,    10: Moeum.E,    11: Moeum.YEO,
    12: Moeum.YE,   13: Moeum.O,    14: Moeum.WA,   15: Moeum.WAE,
    18: Moeum.OE,   19: Moeum.YO,   20: Moeum.U,    21: Moeum.WEO,
    22: Moeum.WE,   23: Moeum.WI,   26: Moeum.YU,   27: Moeum.EU,
    28: Moeum.YI,   29: Moeum.I
}
johab2uni_jongsung = {
    1: u'',         2: Jaeum.G,     3: Jaeum.GG,    4: Jaeum.GS,
    5: Jaeum.N,     6: Jaeum.NJ,    7: Jaeum.NH,    8: Jaeum.D,
    9: Jaeum.L,     10: Jaeum.LG,   11: Jaeum.LM,   12: Jaeum.LB,
    13: Jaeum.LS,   14: Jaeum.LT,   15: Jaeum.LP,   16: Jaeum.LH,
    17: Jaeum.M,    19: Jaeum.B,    20: Jaeum.BS,   21: Jaeum.S,
    22: Jaeum.SS,   23: Jaeum.NG,   24: Jaeum.J,    25: Jaeum.C,
    26: Jaeum.K,    27: Jaeum.T,    28: Jaeum.P,    29: Jaeum.H
}

uni2johab_chosung = {}
uni2johab_jungsung = {}
uni2johab_jongsung = {}
for k, v in johab2uni_chosung.items():
    uni2johab_chosung[v] = k
for k, v in johab2uni_jungsung.items():
    uni2johab_jungsung[v] = k
for k, v in johab2uni_jongsung.items():
    uni2johab_jongsung[v] = k

class Codec(codecs.Codec):

    # Unicode to character buffer
    def encode(self, data, errors='strict'):
        global encmap

        if errors not in ('strict', 'ignore', 'replace'):
            raise ValueError, "unknown error handling"
        buffer = []

        for c in data:
            if c < u'\u0080':
                buffer.append(c.encode("ascii", errors))
            elif ishangul(c):
                cho, jung, jong = split(c) # all hangul can success
                cho, jung, jong = (
                    uni2johab_chosung[cho],
                    uni2johab_jungsung[jung],
                    uni2johab_jongsung[jong]
                )
                code = 0x8000 | (cho<<10) | (jung<<5) | jong
                buffer.append(chr(code>>8) + chr(code&0xFF))
            else:
                if not encmap:
                    from korean.mappings import johab_ideograph
                    encmap = johab_ideograph.encoding_map

                if encmap.has_key(c):
                    buffer.append(encmap[c])
                elif errors == 'replace':
                    buffer.append('\x84\x41')
                elif errors == 'strict':
                    raise UnicodeError, "cannot map \\u%04x to JOHAB" % ord(c)

        return (''.join(buffer), len(data))

    # character buffer to Unicode
    def decode(self, data, errors='strict'):
        global decmap

        if errors not in ('strict', 'ignore', 'replace'):
            raise ValueError, "unknown error handling"

        buffer = []
        data = str(data) # character buffer compatible object
        size = len(data)
        p = 0
        while p < size:
            if data[p] < '\x80':
                buffer.append(unicode(data[p], "ascii", errors))
                p += 1
            else:
                c = data[p:p+2]
                p += 2
                if len(c) == 2:
                    code = (ord(c[0])<<8) | ord(c[1])
                    cho = (code >> 10) & 0x1f
                    jung = (code >> 5) & 0x1f
                    jong = (code) & 0x1f
                    if ( johab2uni_chosung.has_key(cho) and
                         johab2uni_jungsung.has_key(jung) and
                         johab2uni_jongsung.has_key(jong) ):
                        buffer.append( join([
                            johab2uni_chosung[cho],
                            johab2uni_jungsung[jung],
                            johab2uni_jongsung[jong]
                        ]) )
                        continue
                        
                    if not decmap:
                        from korean.mappings import johab_ideograph
                        decmap = johab_ideograph.decoding_map

                    if decmap.has_key(c):
                        buffer.append(decmap[c])
                        continue

                if errors == 'replace':
                    buffer.append(u'\uFFFD') # REPLACEMENT CHARACTER
                elif errors == 'strict':
                    raise UnicodeError, "unexpected byte 0x%02x%02x found" % tuple(map(ord, c))

        return (u''.join(buffer), size)


class StreamWriter(Codec, codecs.StreamWriter):
    pass


class StreamReader(Codec, codecs.StreamReader):

    def __init__(self, stream, errors='strict'):
        codecs.StreamReader.__init__(self, stream, errors)
        self.data = ''

    def _read(self, func, size):
        if size == 0:
            return u''
        if size is None or size < 0:
            data = self.data + func()
            self.data = ''
        else:
            data = self.data + func(max(size, 2) - len(self.data))
            size = len(data)
            p = 0
            while p < size:
                if data[p] < "\x80":
                    p = p + 1
                elif p + 2 <= size:
                    p = p + 2
                else:
                    break
            data, self.data = data[:p], data[p:]
        return self.decode(data)[0]

    def read(self, size=-1):
        return self._read(self.stream.read, size)

    def readline(self, size=-1):
        return self._read(self.stream.readline, size)

    def readlines(self, size=-1):
        data = self._read(self.stream.read, size)
        buffer = []
        end = 0
        while 1:
            pos = data.find(u'\n', end)
            if pos < 0:
                if end < len(data):
                    buffer.append(data[end:])
                break
            buffer.append(data[end:pos+1])
            end = pos+1
        return buffer
    def reset(self):
        self.data = ''

### encodings module API

def getregentry():
    return (Codec().encode,Codec().decode,StreamReader,StreamWriter)

