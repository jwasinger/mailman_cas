# Hye-Shik Chang <1 March 2002>

import CodecTestBase

class TestJOHAB(CodecTestBase.TestStreamReader, CodecTestBase.CodecTestBase):
    encoding = 'johab'
    errortests = (
        # invalid bytes
        ("abc\x80\x80\xc1\xc4", "strict",  None),
        ("abc\x80\x80\xc1\xc4", "replace", u"abc\ufffd\ucd27"),
        ("abc\x80\x80\xc1\xc4", "ignore",  u"abc\ucd27"),
    )

if __name__ == '__main__':
    CodecTestBase.main()
