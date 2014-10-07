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
# $Id: qwerty2bul.py,v 1.9 2002/07/19 00:01:53 perky Exp $
#

import codecs
from korean.hangul import Moeum, Jaeum, Chosung, Jungsung, Jongsung
from korean.hangul import ishangul, join, split, isJaeum, isMoeum

codekeymap = {
        Jaeum.G: 'r',        Jaeum.GG: 'R',       Jaeum.GS: 'rt',
        Jaeum.N: 's',        Jaeum.NJ:'sw',       Jaeum.NH: 'sg',       Jaeum.D: 'e',
        Jaeum.DD:'E',        Jaeum.L: 'f',        Jaeum.LG: 'fr',       Jaeum.LM: 'fa',
        Jaeum.LB:'fq',       Jaeum.LS:'ft',       Jaeum.LT: 'fx',       Jaeum.LP: 'fv',
        Jaeum.LH:'fg',       Jaeum.M: 'a',        Jaeum.B:  'q',        Jaeum.BB: 'Q',
        Jaeum.BS:'qt',       Jaeum.S: 't',        Jaeum.SS: 'T',        Jaeum.NG:  'd',
        Jaeum.J: 'w',        Jaeum.JJ:'W',        Jaeum.C:  'c',        Jaeum.K:  'z',
        Jaeum.T: 'x',        Jaeum.P: 'v',        Jaeum.H:  'g',

        Moeum.A: 'k',        Moeum.AE:'o',        Moeum.YA: 'i',        Moeum.YAE:'O',
        Moeum.EO:'j',        Moeum.E: 'p',        Moeum.YEO:'u',        Moeum.YE: 'P',
        Moeum.O: 'h',        Moeum.WA:'hk',       Moeum.WAE:'ho',       Moeum.OE: 'hl',
        Moeum.YO:'y',        Moeum.U: 'n',        Moeum.WEO:'nj',       Moeum.WE: 'np',
        Moeum.WI:'nl',       Moeum.YU:'b',        Moeum.EU: 'm',        Moeum.YI: 'ml',
        Moeum.I: 'l',

        u'': '',
}

keycodemap = {}
for k, v in codekeymap.items():
        keycodemap[v] = k
        keycodemap.setdefault(v.upper(), k)
keycodes = ''.join(keycodemap.keys())
del k, v


class Automata_Hangul2:
    
    # must Unicode in / Unicode out

    def __init__(self):
        self.clear()

    def pushcomp(self):
        if self.chosung and not self.jungsung:
            self.word_valid = 0
        self.word_comp.append(join([self.chosung, self.jungsung, self.jongsung]))
        self.clearcomp()

    def clearcomp(self):
        self.chosung = u''
        self.jungsung = u''
        self.jongsung = u''

    def clear(self):
        self.buff = ['']
        self.word_raw = []
        self.word_comp = []
        self.word_valid = 1
        self.clearcomp()

    def convert(self, s):
        self.clear()

        map(self.feed, s)
        self.finalize()

        return u''.join(self.buff)
    
    def finalize(self):
        if self.chosung or self.jungsung or self.jongsung:
            self.pushcomp()
        if self.word_raw or self.word_comp:
            if self.word_valid:
                self.buff.append(u''.join(self.word_comp))
            else:
                self.word_valid = 1
                self.buff.append(u''.join(self.word_raw))
            
            self.word_raw, self.word_comp = [], []

    def feed(self, c):
        self.word_raw.append(c)
        if c in keycodes:
            code = keycodemap[c]
            if isJaeum(code):
                if not self.chosung: # chosung O
                    if self.jungsung or self.jongsung:
                        self.word_valid = 0
                    else:
                        self.chosung = code
                elif not self.jungsung: # chosung O  jungsung X
                    if self.jongsung:
                        self.word_valid = 0
                    else:
                        self.pushcomp()
                        self.chosung = code
                elif not self.jongsung: # chosung O  jungsung O  jongsung X
                    if code not in Jongsung:
                        self.pushcomp()
                        self.chosung = code
                    else:
                        self.jongsung = code
                else: # full
                    trymul = codekeymap[self.jongsung] + c
                    if keycodemap.has_key(trymul): # can be multi jongsung
                        self.jongsung = keycodemap[trymul]
                    else:
                        self.pushcomp()
                        self.chosung = code
            else: # MOEUM...
                if not self.jongsung:
                    if not self.jungsung: # jungsung X  jongsung X
                        self.jungsung = code
                    else: # jungsung O  jongsung X
                        trymul = codekeymap[self.jungsung] + c
                        if keycodemap.has_key(trymul): # can be multi jungsung
                            self.jungsung = keycodemap[trymul]
                        else:
                            self.pushcomp()
                            self.jungsung = code
                else: # jongsung O
                    if len(codekeymap[self.jongsung]) > 1:
                        ojong = keycodemap[codekeymap[self.jongsung][:-1]]
                        ncho  = keycodemap[codekeymap[self.jongsung][-1]]
                        self.jongsung = ojong
                        self.pushcomp()
                        self.chosung = ncho
                        self.jungsung = code
                    else:
                        njong = self.jongsung
                        self.jongsung = u''
                        self.pushcomp()
                        self.chosung = njong
                        self.jungsung = code
        else: # non key code
            self.finalize()
            self.buff.append(c)


class Codec(codecs.Codec):

    BASECODEC = 'korean.cp949' # fallback codec of decoder

    # Unicode to key stroke
    def encode(self, data, errors='strict'):
        if errors not in ('strict', 'ignore', 'replace'):
            raise ValueError, "unknown error handling"

        r = []
        for c in data:
            if c <= u'\u0080':
                r.append(c.encode('ascii'))
            elif not ishangul(c):
                r.append(c.encode(self.BASECODEC, errors=errors))
            else:
                for k in split(c):
                    r.append(codekeymap[k])

        r = ''.join(r)
        return (r, len(r))

    # key stroke to Unicode
    def decode(self, data, errors='strict'):
        if errors not in ('strict', 'ignore', 'replace'):
            raise ValueError, "unknown error handling"

        s = unicode(data, self.BASECODEC, errors)
        am = Automata_Hangul2()
        r = am.convert(s)
        return (r, len(r))

class StreamWriter(Codec, codecs.StreamWriter):
    pass

class StreamReader(Codec, codecs.StreamReader):
    pass

def getregentry():
    return (Codec().encode, Codec().decode, StreamReader, StreamWriter)
