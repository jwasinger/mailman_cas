# Hye-Shik Chang <1 March 2002>

import StringIO
import sys, codecs
import unittest
from korean import aliases
del aliases

def escape(s):
    buffer = []
    for c in map(ord, s):
        if c < 0x20 or c > 0x7f:
            buffer.append("\\%03o" % c)
        else:
            buffer.append(chr(c))
    return "'" + ''.join(buffer) + "'"


class CodecTestBase(unittest.TestCase):

    encoding        = ''   # codec name
    textfile_chunk  = None # (native, utf-8) file name tuple
    textfile_stream = None # (native, utf-8)
    
    errortests      = None # must set. error test tuple

    def setUp(self):
        if not self.textfile_chunk:
            self.textfile_chunk = ('text.' + self.encoding, 
                                'text.%s.utf-8' % self.encoding) or self.textfile_stream
        if not self.textfile_stream:
            self.textfile_stream = self.textfile_chunk # checked above. :)

    def test_ChunkCoding(self):
        for native, utf8 in zip(*[open(f).readlines() for f in self.textfile_chunk]):
            u = unicode(native, self.encoding)
            self.assertEqual(u, unicode(utf8, 'utf-8'))
            self.assertEqual(native, u.encode(self.encoding))

    def test_ErrorHandling(self):
        encode, decode, Reader, Writer = codecs.lookup(self.encoding)
        for source, scheme, expected in self.errortests:
            if type(source) == type(''):
                func = decode
            else:
                func = encode
            if expected:
                result = func(source, scheme)[0]
                self.assertEqual(result, expected)
            else:
                try:
                    result = func(source, scheme)[0]
                except UnicodeError:
                    continue
                self.fail('UnicodeError expected')


class TestStreamReader:

    # stream test codes has taken from KAJIYAMA's JapaneseCodecs

    def test_StreamReader(self):
        Reader     = codecs.lookup(self.encoding)[2]
        UTF8Writer = codecs.lookup('utf-8')[3]
        textnative = open(self.textfile_stream[0]).read()
        textuni    = open(self.textfile_stream[1]).read()

        for name in ["read", "readline", "readlines"]:
            for sizehint in [None, -1] + range(1, 33) + [64, 128, 256, 512, 1024]:
                istream = Reader(StringIO.StringIO(textnative))
                ostream = UTF8Writer(StringIO.StringIO())
                func = getattr(istream, name)
                while 1:
                    data = func(sizehint)
                    if not data:
                        break
                    if name == "readlines":
                        ostream.writelines(data)
                    else:
                        ostream.write(data)

                self.assertEqual(ostream.getvalue(), textuni)


class TestStreamWriter:

    def test_StreamWriter(self):
        UTF8Reader = codecs.lookup('utf-8')[2]
        Writer     = codecs.lookup(self.encoding)[3]
        textnative = open(self.textfile_stream[0]).read()
        textuni    = open(self.textfile_stream[1]).read()

        for name in ["read", "readline", "readlines"]:
            for sizehint in [None, -1] + range(1, 33) + [64, 128, 256, 512, 1024]:
                istream = UTF8Reader(StringIO.StringIO(textuni))
                ostream = Writer(StringIO.StringIO())
                func = getattr(istream, name)
                while 1:
                    data = func(sizehint)
                    if not data:
                        break
                    if name == "readlines":
                        ostream.writelines(data)
                    else:
                        ostream.write(data)

                self.assertEqual(ostream.getvalue(), textnative)

def main():
    sys.argv.insert(1, '-v')
    unittest.main(argv=sys.argv)
