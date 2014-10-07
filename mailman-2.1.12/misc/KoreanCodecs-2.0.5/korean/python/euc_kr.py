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
# $Id: euc_kr.py,v 1.8 2002/07/19 00:01:53 perky Exp $
#

import codecs

from korean.mappings import ksc5601_hangul
encmap_hangul, decmap_hangul = ksc5601_hangul.encoding_map, ksc5601_hangul.decoding_map
encmap_ideo, decmap_ideo = {}, {}
encmap_misc, decmap_misc = {}, {}

class Codec(codecs.Codec):

    # Unicode to character buffer
    def encode(self, data, errors='strict'):
        global encmap_ideo, encmap_misc

        if errors not in ('strict', 'ignore', 'replace'):
            raise ValueError, "unknown error handling"
        buffer = []

        for c in data:
            if c < u'\u0080':
                buffer.append(c.encode("ascii", errors))
            elif encmap_hangul.has_key(c):
                buffer.append(encmap_hangul[c])
            else:
                if not encmap_misc:
                    from korean.mappings import ksc5601_misc
                    encmap_misc = ksc5601_misc.encoding_map
                if encmap_misc.has_key(c):
                    buffer.append(encmap_misc[c])
                    continue

                if not encmap_ideo:
                    from korean.mappings import ksc5601_ideograph
                    encmap_ideo = ksc5601_ideograph.encoding_map
                if encmap_ideo.has_key(c):
                    buffer.append(encmap_ideo[c])
                    continue

                if errors == 'replace':
                    buffer.append('\xa1\xa1')
                elif errors == 'strict':
                    raise UnicodeError, "cannot map \\u%04x to EUC-KR" % ord(c)

        return (''.join(buffer), len(data))

    # character buffer to Unicode
    def decode(self, data, errors='strict'):
        global decmap_ideo, decmap_misc

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
                    if decmap_hangul.has_key(c):
                        buffer.append(decmap_hangul[c])
                        continue

                    if not decmap_misc:
                        from korean.mappings import ksc5601_misc
                        decmap_misc = ksc5601_misc.decoding_map
                    if decmap_misc.has_key(c):
                        buffer.append(decmap_misc[c])
                        continue
    
                    if not decmap_ideo:
                        from korean.mappings import ksc5601_ideograph
                        decmap_ideo = ksc5601_ideograph.decoding_map
                    if decmap_ideo.has_key(c):
                        buffer.append(decmap_ideo[c])
                        continue

                if errors == 'replace':
                    buffer.append(u'\uFFFD') # REPLACEMENT CHARACTER
                elif errors == 'strict':
                    raise UnicodeError, "unexpected byte 0x%s found" % (
                            ''.join(["%02x"%ord(x) for x in c]) )

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
