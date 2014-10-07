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
# $Id: unijohab.py,v 1.8 2002/07/19 00:01:53 perky Exp $
#

import codecs
from korean.hangul import ishangul, disjoint, conjoin

class Codec(codecs.Codec):

    # Unicode to character buffer
    def encode(self, data, errors='strict'):
        if errors not in ('strict', 'ignore', 'replace'):
            raise ValueError, "unknown error handling"

        return disjoint(data).encode('utf-8', errors), len(data)

    # character buffer to Unicode
    def decode(self, data, errors='strict'):
        if errors not in ('strict', 'ignore', 'replace'):
            raise ValueError, "unknown error handling"

        return conjoin(unicode(data, 'utf-8', errors)), len(data)

class StreamWriter(Codec, codecs.StreamWriter):
    pass

class StreamReader(Codec, codecs.StreamReader):
    pass
    # XXX: Temporarily None.

### encodings module API

def getregentry():
    return (Codec().encode,Codec().decode,StreamReader,StreamWriter)
