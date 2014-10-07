import CodecTestBase

from test_cp949 import TestCP949_CExtension, TestCP949_PurePython
from test_euc_kr import TestEUCKR_CExtension, TestEUCKR_PurePython
from test_iso_2022_kr import TestISO_2022_KR
from test_johab import TestJOHAB
from test_qwerty2bul import TestQWERTY2BUL
from test_unijohab import TestUNIJOHAB

from test_hangul import TestHangul_CExtension, TestHangul_PurePython

if __name__ == '__main__':
    CodecTestBase.main()
