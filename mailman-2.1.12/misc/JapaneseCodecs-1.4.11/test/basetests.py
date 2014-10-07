# Tamito KAJIYAMA <19 December 2000>

import StringIO
import string
import codecs
import sys

def roundrobin_tests(file, encoding):
    assert unicode(u"\u005C".encode(encoding), encoding) == u"\u005C"
    assert unicode(u"\uFF3C".encode(encoding), encoding) == u"\uFF3C"
    for line in open(file).readlines():
        u = unicode(line, encoding)
        sys.stdout.write(u.encode("utf-8"))
        assert line == u.encode(encoding)

def test_stream(file, encoding, compare_unicode=0):
    encode, decode, Reader, Writer = codecs.lookup(encoding)
    text = open(file).read()
    if compare_unicode:
        test = unicode(text, encoding)
    else:
        test = text
    for name in ["read", "readline", "readlines"]:
        for sizehint in [None, -1] + range(1, 33) + [64, 128, 256, 512, 1024]:
            ###print name, sizehint
            istream = Reader(StringIO.StringIO(text))
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
            if compare_unicode:
                assert test == unicode(ostream.getvalue(), encoding)
            else:
                assert test == ostream.getvalue()

def test_backslash_tilde(encoding):
    assert u"\u00a5\u203e\\~".encode(encoding) == "\\~\\~"

def test_jis_x_0201_roman(encoding):
    # backslash and tilde
    jis = "\033(J\\~\033(B\\~"
    ucs = u"\u00a5\u203e\\~"
    assert unicode(jis, encoding) == ucs
    assert jis == ucs.encode(encoding)
    # control characters
    jis = "\033(J\r\n\033(B\r\n"
    ucs = u"\r\n\r\n"
    assert unicode(jis, encoding) == ucs

def test_encode(encoding):
    encode, decode, Reader, Writer = codecs.lookup(encoding)
    assert encode("abc") == ("abc", 3)
    for obj in [None, 0, [], (), {}]:
        try:
            encode(obj)
        except TypeError:
            pass
        else:
            raise AssertionError, "a TypeError expected"

def test_errors(encoding):
    encode, decode, Reader, Writer = codecs.lookup(encoding)
    for scheme in ["strict", "ignore", "replace", "unknown"]:
        for func, value in [(encode, u""), (decode, "")]:
            try:
                func(value, scheme)
            except ValueError:
                if scheme != "unknown":
                    raise AssertionError, "no error expected"
            else:
                if scheme == "unknown":
                    raise AssertionError, "a ValueError expected"

def test_error_handling(encoding, tests):
    encode, decode, Reader, Writer = codecs.lookup(encoding)
    for source, scheme, expected in tests:
        if type(source) == type(''):
            func = decode
        else:
            func = encode
        if expected:
            result = func(source, scheme)[0]
            assert result == expected
        else:
            try:
                result = func(source, scheme)[0]
            except UnicodeError:
                continue
            raise AssertionError, "a UnicodeError expected"
        if type(result) == type(''):
            sys.stdout.write(escape(result))
        else:
            sys.stdout.write(result.encode("utf-8"))
        sys.stdout.write('\n')
    for source, scheme, expected in tests:
        if type(source) == type(''):
            stream = Reader(StringIO.StringIO(source), scheme)
            if expected:
                assert stream.read() == expected
            else:
                try:
                    stream.read()
                except UnicodeError:
                    continue
                raise AssertionError, "a UnicodeError expected"

def escape(s):
    buffer = []
    for c in map(ord, s):
        if c < 0x20 or c > 0x7f:
            buffer.append("\\%03o" % c)
        else:
            buffer.append(chr(c))
    return "'" + string.join(buffer, '') + "'"
