#
# This file is part of KoreanCodecs.
#
# Copyright(C) Hye-Shik Chang <perky@FreeBSD.org>, 2002.
#
# KoreanCodecs is free software; you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published
# by the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# KoreanCodecs is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with KoreanCodecs; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#
# $Id: aliases.py,v 1.6 2002/07/19 00:01:52 perky Exp $
#

import encodings.aliases

encodings.aliases.aliases.update({
    'cp949':        'korean.cp949',
    'ms949':        'korean.cp949',
    'uhc':          'korean.cp949',
    'euc_kr':       'korean.euc_kr',
    'euckr':        'korean.euc_kr',
    'ksc5601':      'korean.euc_kr',
    'ksc5601_1987': 'korean.euc_kr',
    'ksc_5601_1987':'korean.euc_kr',
    'ks_c_5601_1987':'korean.euc_kr',
    'ksx1001':      'korean.euc_kr',
    'iso_2022_kr':  'korean.iso_2022_kr',
    'iso2022kr':    'korean.iso_2022_kr',
    'iso2022_kr':   'korean.iso_2022_kr',
    'johab':        'korean.johab',
    'qwerty2bul':   'korean.qwerty2bul',
    'unijohab':     'korean.unijohab',
    'macjohab':     'korean.unijohab',
})
