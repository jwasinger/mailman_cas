# Hye-Shik Chang <1 March 2002>

import unittest

class Shield:
  class TestHangul(unittest.TestCase):
    def test_joinsplit(self):
        self.assertEqual(self.h.join([self.h.J, self.h.WA, self.h.L]), u'\uc894')
        self.assertEqual(self.h.join([self.h.JJ, self.h.Null, self.h.Null]), u'\u3149')
        self.assertEqual(self.h.join((self.h.Null, self.h.YI, self.h.Null)), u'\u3162')

        self.assertEqual(self.h.split(u'\uc894'), (self.h.J, self.h.WA, self.h.L))
        self.assertEqual(self.h.split(u'\u3149'), (self.h.JJ, self.h.Null, self.h.Null))
        self.assertEqual(self.h.split(u'\u3162'), (self.h.Null, self.h.YI, self.h.Null))

    def test_basicspec(self):
        self.assertEqual(self.h.isJaeum(self.h.J), 1)
        self.assertEqual(self.h.isJaeum(self.h.E), 0)
        self.assertEqual(self.h.isMoeum(self.h.L), 0)
        self.assertEqual(self.h.isMoeum(self.h.O), 1)
        self.assertEqual(self.h.ishangul(u'\uc870'), 1)
        self.assertEqual(self.h.ishangul(u'\u382c'), 0)

    def test_testlong(self):
        self.assertEqual(self.h.isJaeum(u'\u3131\u3134\u3137\u3139'), 1)
        self.assertEqual(self.h.isJaeum(u'\u3131\u314f\u3134\u314f'), 0)
        self.assertEqual(self.h.isJaeum(u''), 0)

        self.assertEqual(self.h.isMoeum(u'\u314f\u3151\u3153\u3155'), 1)
        self.assertEqual(self.h.isMoeum(u'\u3131\u314f\u3134\u314f'), 0)
        self.assertEqual(self.h.isMoeum(u''), 0)

        self.assertEqual(self.h.ishangul(u'\ud2f0\ud2f0\ub9c8\uc18c\uc774'), 1)
        self.assertEqual(self.h.ishangul(u'\ud2f0\ud2f0\ub9c8 \uc18c\uc774'), 0)
        self.assertEqual(self.h.ishangul(u''), 0)

    def test_format_altsuffix(self):
        fmt = u'%s\ub294 %s\ub97c %s\ud55c\ub2e4.'
        obj1, obj2 = u'\ud61c\uc2dd', u'\uc544\ub77c'
        self.assertEqual(self.h.format(fmt, obj1, obj2, u'\u2661'),
                  u'\ud61c\uc2dd\uc740 \uc544\ub77c\ub97c \u2661\ud55c\ub2e4.')
        self.assertEqual(self.h.format(fmt, obj2, obj1, u'\uc2eb\uc5b4'),
                  u'\uc544\ub77c\ub294 \ud61c\uc2dd\uc744 \uc2eb\uc5b4\ud55c\ub2e4.')

        fmt = u'\ud0dc\ucd08\uc5d0 %s\uc640 %s\uac00 \uc788\uc5c8\ub2e4.'
        self.assertEqual(self.h.format(fmt, obj1, obj2),
               u'\ud0dc\ucd08\uc5d0 \ud61c\uc2dd\uacfc \uc544\ub77c\uac00'
               u' \uc788\uc5c8\ub2e4.')
        self.assertEqual(self.h.format(fmt, obj2, obj1),
               u'\ud0dc\ucd08\uc5d0 \uc544\ub77c\uc640 \ud61c\uc2dd\uc774'
               u' \uc788\uc5c8\ub2e4.')

        obj1, obj2 = u'Julian', u'Julie'
        self.assertEqual(self.h.format(fmt, obj1, obj2),
               u'\ud0dc\ucd08\uc5d0 Julian\uacfc Julie\uac00 \uc788\uc5c8\ub2e4.')
        self.assertEqual(self.h.format(fmt, obj2, obj1),
               u'\ud0dc\ucd08\uc5d0 Julie\uc640 Julian\uc774 \uc788\uc5c8\ub2e4.')

    def test_format_idasuffix(self):
        fmt = u'%s(\uc785)\ub2c8\ub2e4, %s(\uc778)\ub370, %s(\uc774)\ub2e4'
        self.assertEqual(self.h.format(fmt, *(u'\uc18c\uc774',)*3), 
                u'\uc18c\uc785\ub2c8\ub2e4, \uc18c\uc778\ub370, \uc18c\uc774\ub2e4')
        self.assertEqual(self.h.format(fmt, *(u'\ub2e4\ub155',)*3), 
                u'\ub2e4\ub155\uc785\ub2c8\ub2e4, \ub2e4\ub155\uc778\ub370,'
                u' \ub2e4\ub155\uc774\ub2e4')

    def test_format_argtypes(self):
        fmt = u'%(int)d(\uc785)\ub2c8\ub2e4. %(str)s\uc740 %(str)s\uc5d0' \
              u'%(float).2f\uc640'
        self.assertEqual(self.h.format(fmt, int=1, str=u'hmm', float=3.14),
                u'1\uc785\ub2c8\ub2e4. hmm\uc740 hmm\uc5d03.14\uc640')

    def test_conjoin(self):
        self.assertEqual(self.h.conjoin(u'\u1112\u1161\u11ab\u1100\u1173\u11af\u110b\u1175'
                        u' \u110c\u1169\u11c2\u110b\u1161\u110b\u116d.'),
                        u'\ud55c\uae00\uc774 \uc88b\uc544\uc694.')

    def test_disjoint(self):
        self.assertEqual(self.h.disjoint(u'\ub9c8\ub140\ubc30\ub2ec\ubd80 \ud0a4\ud0a4'),
                        u'\u1106\u1161\u1102\u1167\u1107\u1162\u1103\u1161\u11af\u1107\u116e'
                        u' \u110f\u1175\u110f\u1175')

class TestHangul_CExtension(Shield.TestHangul):
    from korean.c import hangul as h

class TestHangul_PurePython(Shield.TestHangul):
    from korean.python import hangul as h

if __name__ == '__main__':
    import sys
    sys.argv.insert(1, '-v')
    unittest.main(argv=sys.argv)
