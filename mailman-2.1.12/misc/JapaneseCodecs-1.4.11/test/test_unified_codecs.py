# Tamito KAJIYAMA <26 September 2001>

assert(u"test" == unicode("test", "japanese.euc-jp"))
assert(u"test" == unicode("test", "japanese.shift_jis"))
assert(u"test" == unicode("test", "japanese.iso-2022-jp"))
assert(u"test" == unicode("test", "japanese.iso-2022-jp-1"))
assert(u"test" == unicode("test", "japanese.iso-2022-jp-ext"))
assert(u"test" == unicode("test", "japanese.jis-x-0201-roman"))
assert(u"\uFF83\uFF7D\uFF84" == unicode("C=D", "japanese.jis-x-0201-katakana"))
