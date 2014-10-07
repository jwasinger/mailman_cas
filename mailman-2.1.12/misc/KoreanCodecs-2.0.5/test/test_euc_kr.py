# Hye-Shik Chang <1 March 2002>

import CodecTestBase

class Shield:
    class TestEUCKR_Base(CodecTestBase.TestStreamReader, CodecTestBase.CodecTestBase):
        encoding = 'euc-kr'
        textfile_chunk = ('text.euc-kr', 'text.euc-kr.utf-8')
        errortests = (
            # invalid bytes
            ("abc\x80\x80\xc1\xc4", "strict",  None),
            ("abc\xc8", "strict",  None),
            ("abc\x80\x80\xc1\xc4", "replace", u"abc\ufffd\uc894"),
            ("abc\x80\x80\xc1\xc4\xc8", "replace", u"abc\ufffd\uc894\ufffd"),
            ("abc\x80\x80\xc1\xc4", "ignore",  u"abc\uc894"),
        )

        def test_ksx1001_1998(self):
            self.assertEqual(unicode('\xa2\xe6', self.encoding), u'\u20ac')
            self.assertEqual(unicode('\xa2\xe7', self.encoding), u'\u00ae')
            self.assertEqual(u'\u20ac'.encode(self.encoding), '\xa2\xe6')
            self.assertEqual(u'\u00ae'.encode(self.encoding), '\xa2\xe7')
	

class TestEUCKR_CExtension(Shield.TestEUCKR_Base):
    encoding = 'korean.c.euc-kr'

class TestEUCKR_PurePython(Shield.TestEUCKR_Base):
    encoding = 'korean.python.euc-kr'

if __name__ == '__main__':
    CodecTestBase.main()
