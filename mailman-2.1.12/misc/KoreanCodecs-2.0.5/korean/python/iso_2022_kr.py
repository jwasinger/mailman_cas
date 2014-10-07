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
# $Id: iso_2022_kr.py,v 1.11 2002/07/19 00:01:53 perky Exp $
#

import codecs
from korean.mappings import ksc5601_hangul
encmap_hangul = ksc5601_hangul.encoding_map
decmap_hangul = ksc5601_hangul.decoding_map
encmap_ideo, decmap_ideo = {}, {}
encmap_misc, decmap_misc = {}, {}

US_ASCII      = 1
KSC5601_1987  = 2

CHARSETS = {
    "\033(B": US_ASCII,
    "\033$)C": KSC5601_1987,
}
SI = '\x0f'
SO = '\x0e'
ESC = '\033'

DESIGNATIONS = {}
for k, v in CHARSETS.items():
    DESIGNATIONS[v] = k

# StreamReader was adopted from Tamito KAJIYAMA's iso-2022-jp codec.

class Codec(codecs.Codec):
    # Unicode to character buffer
    def encode(self, data, errors='strict'):
        global encmap_ideo, encmap_misc

        if errors not in ('strict', 'ignore', 'replace'):
            raise ValueError, "unknown error handling"
        buffer = []
        new_charset = charset = US_ASCII
        new_shiftstate = shiftstate = 0
        for c in data:
            if c in ('\n', '\r'):
                new_shiftstate = 0

            if c < u'\u0080':
                new_shiftstate = 0
                s = c.encode("ascii", errors)
            elif encmap_hangul.has_key(c):
                new_charset = KSC5601_1987
                new_shiftstate = 1
                s = encmap_hangul[c]
            else:
                if not encmap_misc:
                    from korean.mappings import ksc5601_misc
                    encmap_misc = ksc5601_misc.encoding_map
                if encmap_misc.has_key(c):
                    new_charset = KSC5601_1987
                    new_shiftstate = 1
                    s = encmap_misc[c]
                else:
                    if not encmap_ideo:
                        from korean.mappings import ksc5601_ideograph
                        encmap_ideo = ksc5601_ideograph.encoding_map
                    if encmap_ideo.has_key(c):
                        new_charset = KSC5601_1987
                        new_shiftstate = 1
                        s = encmap_ideo[c]
                    elif errors == 'replace':
                        new_charset = KSC5601_1987
                        new_shiftstate = 1
                        s = '\xa1\xa1'
                    elif errors == 'strict':
                        raise UnicodeError, "cannot map \\u%04x to ISO-2022-KR" % ord(c)
                    else:
                        continue

            if charset != new_charset:
                charset = new_charset
                buffer.append(DESIGNATIONS[charset])
            if new_shiftstate != shiftstate:
                shiftstate = new_shiftstate
                buffer.append([SI, SO][shiftstate])

            if shiftstate:
                s = chr(ord(s[0])&0x7F) + chr(ord(s[1])&0x7F)
            buffer.append(s)
        if shiftstate:
            buffer.append(SI)
            #buffer.append(DESIGNATIONS[US_ASCII])
        return (''.join(buffer), len(data))

    # character buffer to Unicode
    def decode(self, data, errors='strict'):
        global decmap_ideo, decmap_misc
        
        if errors not in ('strict', 'ignore', 'replace'):
            raise ValueError, "unknown error handling"
        buffer = []
        data = str(data) # character buffer compatible object
        size = len(data)
        charset = US_ASCII
        shiftstate = 0
        escstart = -1
        p = 0
        while p < size:
            if data[p] in ('\n', '\r'):
                shiftstate = 0

            if escstart >= 0:
                if data[p].isalpha():
                    escstr = data[escstart:p+1]
                    if CHARSETS.has_key(escstr):
                        charset = CHARSETS[escstr]
                    elif errors == 'strict':
                        raise UnicodeError, "unsupported charset found: %s" % repr(data[escstart:p+1])
                    escstart = -1
                p += 1
            elif data[p] == SO:
                shiftstate = 1
                p += 1
            elif data[p] == SI:
                shiftstate = 0
                p += 1
            elif data[p] == ESC:
                escstart = p
                p += 1
            else:
                if not shiftstate and (
                        charset == US_ASCII or data[p] < '\x80'): # ascii
                    buffer.append(unicode(data[p], "ascii", errors))
                    p += 1
                else:
                    c = data[p:p+2]
                    p += 2
                    if len(c) == 2:
                        c = chr(ord(c[0])|0x80) + chr(ord(c[1])|0x80)
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
                        raise UnicodeError, "unexpected byte 0x%02x%02x found" % tuple(map(ord, c))
                        # XXX: only 1byte?
        
        return (u''.join(buffer), len(data))

class StreamWriter(Codec, codecs.StreamWriter):
    pass

class StreamReader(Codec, codecs.StreamReader):
    pass
    # not implemented.
    # (JapaneseCodecs's implementation is so different to adopt.)

### encodings module API

def getregentry():
    return (Codec().encode,Codec().decode,StreamReader,StreamWriter)
