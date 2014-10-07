# Tamito KAJIYAMA <26 September 2001>

import basetests

def test(encoding):
    file = "text.shift_jis"
    basetests.roundrobin_tests(file, encoding)
    basetests.test_backslash_tilde(encoding)
    basetests.test_stream(file, encoding)
    basetests.test_encode(encoding)
    basetests.test_errors(encoding)
    basetests.test_error_handling(encoding, (
        # invalid bytes
        ("abc\x80\x80\x82\xa0", "strict",  None),
        ("abc\x80\x80\x82\xa0", "replace", u"abc\ufffd\u3042"),
        ("abc\x80\x80\x82\xa0", "ignore",  u"abc\u3042"),
        # a character that has no corresponding character in Shift_JIS
        (u"abc\ufffd\u3042", "strict",  None),
        (u"abc\ufffd\u3042", "replace", "abc\x81\xac\x82\xa0"),
        (u"abc\ufffd\u3042", "ignore",  "abc\x82\xa0"),
        ))
