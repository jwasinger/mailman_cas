# Hye-Shik Chang <1 March 2002>

import CodecTestBase

class TestUNIJOHAB(CodecTestBase.CodecTestBase):
    encoding = 'unijohab'
    errortests = () # error handling is relying UTF-8 codec.

if __name__ == '__main__':
    CodecTestBase.main()
