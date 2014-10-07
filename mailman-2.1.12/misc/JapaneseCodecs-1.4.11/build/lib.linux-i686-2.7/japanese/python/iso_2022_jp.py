# A codec for ISO-2022-JP [RFC1468] (so-called 7-bit JIS)
# Tamito KAJIYAMA <24 November 2000>

import codecs

from japanese.mappings import jis_x_0208

US_ASCII      = 1
JISX0201_1976 = 2
JISX0208_1978 = 3
JISX0208_1983 = 4

CHARSETS = {
    "\033(B": US_ASCII,
    "\033(J": JISX0201_1976,
    "\033$@": JISX0208_1978,
    "\033$B": JISX0208_1983,
}

DESIGNATIONS = {}
for k, v in CHARSETS.items():
    DESIGNATIONS[v] = k

class Codec(codecs.Codec):
    # Unicode to character buffer
    def encode(self, data, errors='strict',
               supported_errors=('strict', 'ignore', 'replace')):
        if errors not in supported_errors:
            raise ValueError, "unknown error handling code: " + str(errors)
        m = jis_x_0208.encoding_map
        buffer = []
        charset = US_ASCII
        for c in u"" + data:
            if c < u'\u0080':
                new_charset = US_ASCII
                s = c.encode("ascii", errors)
            elif c == u'\u00A5': # YEN SIGN
                new_charset = JISX0201_1976
                s = '\\'
            elif c == u'\u203E': # OVERLINE
                new_charset = JISX0201_1976
                s = '~'
            elif m.has_key(c):
                new_charset = JISX0208_1983
                s = m[c]
            elif errors == 'replace':
                new_charset = JISX0208_1983
                s = '\x22\x2e' # U+3013 GETA MARK
            elif errors == 'strict':
                raise UnicodeError, "cannot map \\u%04x to ISO-2022-JP" % ord(c)
            else:
                continue
            if charset != new_charset:
                charset = new_charset
                buffer.append(DESIGNATIONS[charset])
            buffer.append(s)
        if charset != US_ASCII:
            buffer.append(DESIGNATIONS[US_ASCII])
        return (''.join(buffer), len(data))
    # character buffer to Unicode
    def decode(self, data, errors='strict',
               supported_errors=('strict', 'ignore', 'replace')):
        if errors not in supported_errors:
            raise ValueError, "unknown error handling code: " + str(errors)
        m = jis_x_0208.decoding_map
        buffer = []
        data = str(data) # character buffer compatible object
        charset = US_ASCII
        end = 0
        while 1:
            pos = data.find("\033", end)
            if pos < 0:
                if charset != US_ASCII:
                    raise UnicodeError, "malformed input"
                buffer.append(unicode(data[end:], "ascii", errors))
                break
            if charset == US_ASCII:
                buffer.append(unicode(data[end:pos], "ascii", errors))
            elif charset == JISX0201_1976:
                buffer.append(unicode(data[end:pos], "japanese.jis-x-0201-roman", errors))
            elif charset in [JISX0208_1978, JISX0208_1983]:
                for i in range(end, pos, 2):
                    s = data[i:i+2]
                    if m.has_key(s):
                        buffer.append(m[s])
                    elif errors == 'replace':
                        buffer.append(u'\uFFFD') # REPLACEMENT CHARACTER
                    elif errors == 'strict':
                        raise UnicodeError, "unexpected byte 0x%02x found" % ord(data[i])
            end = pos + 3
            try:
                s = data[pos:end]
            except IndexError:
                raise UnicodeError, "unexpected end of input"
            try:
                charset = CHARSETS[s]
            except KeyError:
                raise UnicodeError, "unknown designation"
        return (u''.join(buffer), len(data))

class StreamWriter(Codec, codecs.StreamWriter):
    pass

class StreamReader(Codec, codecs.StreamReader):
    def __init__(self, stream, errors='strict'):
        codecs.StreamReader.__init__(self, stream, errors)
        self.data = ''
        self.charset = US_ASCII
    def _read(self, func, size):
        if size == 0:
            return u''
        if size is None or size < 0:
            data = self.data + func()
        else:
            data = self.data + func(max(size, 8) - len(self.data))
        self.data = ''
        if self.charset != US_ASCII:
            data = DESIGNATIONS[self.charset] + data
        pos = data.rfind("\033")
        if pos >= 0 and pos + 3 >= len(data):
            # data ends on the way of an escape sequence
            data, self.data = data[:pos], data[pos:]
            pos = data.rfind("\033")
        if pos >= 0:
            try:
                self.charset = CHARSETS[data[pos:pos+3]]
            except KeyError:
                raise UnicodeError, "unknown designation"
            if self.charset in [JISX0208_1978, JISX0208_1983] and \
               (len(data) - pos - 3) % 2 == 1:
                data, self.data = data[:-1], data[-1]
            if self.charset != US_ASCII:
                data = data + DESIGNATIONS[US_ASCII]
        return self.decode(data, self.errors)[0]
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
