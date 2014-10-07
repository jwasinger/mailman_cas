# Hye-Shik Chang <1 March 2002>

import CodecTestBase

class TestQWERTY2BUL(CodecTestBase.CodecTestBase):
    encoding = 'qwerty2bul'
    errortests = (
        # invalid bytes
        ("123\x80\x80whkf", "strict",  None),
        ("123\x80\x80whkf", "replace", u"123\ufffd\uc894"),
        ("123\x80\x80whkf", "ignore",  u"123\uc894"),
    )

if __name__ == '__main__':
    CodecTestBase.main()
