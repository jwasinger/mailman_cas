# Hye-Shik Chang <1 March 2002>

import CodecTestBase

class TestISO_2022_KR(CodecTestBase.CodecTestBase):
    encoding = 'iso-2022-kr'
    textfile_chunk  = ('text.%s.roundrobin' % encoding, 'text.%s.utf-8' % encoding)
    textfile_stream = ('text.%s.stream' % encoding, 'text.%s.utf-8' % encoding)

    errortests = (
        # invalid bytes
        ("abc\x1b$)C\x0e\x00\x00AD\x0f\x1b$(B", "strict",  None),
        ("abc\x1b$)C\x0e\x00\x00AD\x0f\x1b$(B", "replace", u"abc\ufffd\uc894"),
        ("abc\x1b$)C\x0e\x00\x00AD\x0f\x1b$(B", "ignore",  u"abc\uc894"),
    )

if __name__ == '__main__':
    CodecTestBase.main()
