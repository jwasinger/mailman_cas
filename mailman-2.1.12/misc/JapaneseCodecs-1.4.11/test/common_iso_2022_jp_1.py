# Tamito KAJIYAMA <26 September 2001>

import basetests

def test(encoding):
    file = "text.iso-2022-jp-1"
    basetests.roundrobin_tests(file, encoding)
    basetests.test_jis_x_0201_roman(encoding)
    basetests.test_stream(file, encoding, 1)
    basetests.test_encode(encoding)
    basetests.test_errors(encoding)
    basetests.test_error_handling(encoding, (
        # invalid bytes in JIS X 0208
        ("abc\033$B\x00\x00\x30\x21\033(B", "strict",  None),
        ("abc\033$B\x00\x00\x30\x21\033(B", "replace", u"abc\ufffd\u4e9c"),
        ("abc\033$B\x00\x00\x30\x21\033(B", "ignore",  u"abc\u4e9c"),
        # invalid byte in JIS X 0201 Roman
        ("abc\033(J\x80xyz\033(B", "strict",  None),
        ("abc\033(J\x80xyz\033(B", "replace", u"abc\ufffdxyz"),
        ("abc\033(J\x80xyz\033(B", "ignore",  u"abcxyz"),
        # invalid bytes in JIS X 0212
        ("abc\033$(D\x00\x00\x30\x21\033(B", "strict",  None),
        ("abc\033$(D\x00\x00\x30\x21\033(B", "replace", u"abc\ufffd\u4e02"),
        ("abc\033$(D\x00\x00\x30\x21\033(B", "ignore",  u"abc\u4e02"),
        # a character that has no corresponding character in ISO-2022-JP-1
        (u"abc\ufffd\u4e9c", "strict",  None),
        (u"abc\ufffd\u4e9c", "replace", "abc\033$B\x22\x2e\x30\x21\033(B"),
        (u"abc\ufffd\u4e9c", "ignore",  "abc\033$B\x30\x21\033(B"),
        # unknown designation (GB 2312)
        ("abc\033$A\x30\x21\033(B", "strict",  None),
        ("abc\033$A\x30\x21\033(B", "replace", None),
        ("abc\033$A\x30\x21\033(B", "ignore",  None),
        ))
    file = "supl.iso-2022-jp-1"
    basetests.roundrobin_tests(file, encoding)
