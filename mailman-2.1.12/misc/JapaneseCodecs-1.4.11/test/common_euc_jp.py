# Tamito KAJIYAMA <26 September 2001>

import basetests

def test(encoding):
    file = "text.euc-jp"
    basetests.roundrobin_tests(file, encoding)
    basetests.test_backslash_tilde(encoding)
    basetests.test_stream(file, encoding)
    basetests.test_encode(encoding)
    basetests.test_errors(encoding)
    basetests.test_error_handling(encoding, (
        # invalid bytes in JIS X 0208
        ("abc\x80\x80\xa4\xa2", "strict",  None),
        ("abc\x80\x80\xa4\xa2", "replace", u"abc\ufffd\u3042"),
        ("abc\x80\x80\xa4\xa2", "ignore",  u"abc\u3042"),
        # invalid byte in JIS X 0201 Katakana
        ("abc\x8e\x00\x8e\xb1", "strict",  None),
        ("abc\x8e\x00\x8e\xb1", "replace", u"abc\ufffd\uff71"),
        ("abc\x8e\x00\x8e\xb1", "ignore",  u"abc\uff71"),
        # invalid bytes in JIS X 0212
        ("abc\x8f\x80\x80\x8f\xb0\xa1", "strict",  None),
        ("abc\x8f\x80\x80\x8f\xb0\xa1", "replace", u"abc\ufffd\u4e02"),
        ("abc\x8f\x80\x80\x8f\xb0\xa1", "ignore",  u"abc\u4e02"),
        # a character that has no corresponding character in EUC-JP
        (u"abc\ufffd\u3042", "strict",  None),
        (u"abc\ufffd\u3042", "replace", "abc\xa2\xae\xa4\xa2"),
        (u"abc\ufffd\u3042", "ignore",  "abc\xa4\xa2"),
        ))
    file = "supl.euc-jp"
    basetests.roundrobin_tests(file, encoding)
