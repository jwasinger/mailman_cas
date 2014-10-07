# Hye-Shik Chang <1 March 2002>

import CodecTestBase

class Shield:
    class TestCP949Base(CodecTestBase.TestStreamReader, CodecTestBase.CodecTestBase):
        encoding = 'cp949'
        textfile_chunk = ('text.cp949', 'text.cp949.utf-8')
        errortests = (
            # invalid bytes
            ("abc\x80\x80\xc1\xc4", "strict",  None),
            ("abc\xc8", "strict",  None),
            ("abc\x80\x80\xc1\xc4", "replace", u"abc\ufffd\uc894"),
            ("abc\x80\x80\xc1\xc4\xc8", "replace", u"abc\ufffd\uc894\ufffd"),
            ("abc\x80\x80\xc1\xc4", "ignore",  u"abc\uc894"),
        )

class TestCP949_CExtension(Shield.TestCP949Base):
    encoding = 'korean.c.cp949'

class TestCP949_PurePython(Shield.TestCP949Base):
    encoding = 'korean.python.cp949'

if __name__ == '__main__':
    CodecTestBase.main()
