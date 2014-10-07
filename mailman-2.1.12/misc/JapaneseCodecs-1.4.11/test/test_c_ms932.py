# Tamito KAJIYAMA <2 March 2002>

import basetests

def test(encoding):
    file = "text.shift_jis"
    basetests.roundrobin_tests(file, encoding)
    basetests.test_backslash_tilde(encoding)
    basetests.test_stream(file, encoding)
    basetests.test_encode(encoding)
    basetests.test_errors(encoding)
    basetests.test_error_handling(encoding, (
        # invalid bytes
        ("abc\x80\x80\x82\xa0", "strict",  None),
        ("abc\x80\x80\x82\xa0", "replace", u"abc\ufffd\u3042"),
        ("abc\x80\x80\x82\xa0", "ignore",  u"abc\u3042"),
        # a character that has no corresponding character in Shift_JIS
        (u"abc\ufffd\u3042", "strict",  None),
        (u"abc\ufffd\u3042", "replace", "abc\x81\xac\x82\xa0"),
        (u"abc\ufffd\u3042", "ignore",  "abc\x82\xa0"),
        ))
    test_non_round_trip_mapping(encoding)

# Q170559 - PRB: Conversion Problem Between Shift-JIS and Unicode
# http://support.microsoft.com/default.aspx?scid=kb;en-us;Q170559
NON_ROUND_TRIP_MAPPING = [
    ["\x87\x9c", u"\u222a", "\x81\xbe"],
    ["\x87\x9b", u"\u2229", "\x81\xbf"],
    ["\xee\xf9", u"\uffe2", "\x81\xca"],
    ["\xfa\x54", u"\uffe2", "\x81\xca"],
    ["\x87\x97", u"\u2220", "\x81\xda"],
    ["\x87\x96", u"\u22a5", "\x81\xdb"],
    ["\x87\x91", u"\u2261", "\x81\xdf"],
    ["\x87\x90", u"\u2252", "\x81\xe0"],
    ["\x87\x95", u"\u221a", "\x81\xe3"],
    ["\x87\x9a", u"\u2235", "\x81\xe6"],
    ["\xfa\x5b", u"\u2235", "\x81\xe6"],
    ["\x87\x92", u"\u222b", "\x81\xe7"],
    ["\xfa\x4a", u"\u2160", "\x87\x54"],
    ["\xfa\x4b", u"\u2161", "\x87\x55"],
    ["\xfa\x4c", u"\u2162", "\x87\x56"],
    ["\xfa\x4d", u"\u2163", "\x87\x57"],
    ["\xfa\x4e", u"\u2164", "\x87\x58"],
    ["\xfa\x4f", u"\u2165", "\x87\x59"],
    ["\xfa\x50", u"\u2166", "\x87\x5a"],
    ["\xfa\x51", u"\u2167", "\x87\x5b"],
    ["\xfa\x52", u"\u2168", "\x87\x5c"],
    ["\xfa\x53", u"\u2169", "\x87\x5d"],
    ["\xfa\x59", u"\u2116", "\x87\x82"],
    ["\xfa\x5a", u"\u2121", "\x87\x84"],
    ["\xfa\x58", u"\u3231", "\x87\x8a"],
    ["\xee\xef", u"\u2170", "\xfa\x40"],
    ["\xee\xf0", u"\u2171", "\xfa\x41"],
    ["\xee\xf1", u"\u2172", "\xfa\x42"],
    ["\xee\xf2", u"\u2173", "\xfa\x43"],
    ["\xee\xf3", u"\u2174", "\xfa\x44"],
    ["\xee\xf4", u"\u2175", "\xfa\x45"],
    ["\xee\xf5", u"\u2176", "\xfa\x46"],
    ["\xee\xf6", u"\u2177", "\xfa\x47"],
    ["\xee\xf7", u"\u2178", "\xfa\x48"],
    ["\xee\xf8", u"\u2179", "\xfa\x49"],
    ["\xee\xfa", u"\uffe4", "\xfa\x55"],
    ["\xee\xfb", u"\uff07", "\xfa\x56"],
    ["\xee\xfc", u"\uff02", "\xfa\x57"],
    ["\xed\x40", u"\u7e8a", "\xfa\x5c"],
    ["\xed\x41", u"\u891c", "\xfa\x5d"],
    ["\xed\x42", u"\u9348", "\xfa\x5e"],
    ["\xed\x43", u"\u9288", "\xfa\x5f"],
    ["\xed\x44", u"\u84dc", "\xfa\x60"],
    ["\xed\x45", u"\u4fc9", "\xfa\x61"],
    ["\xed\x46", u"\u70bb", "\xfa\x62"],
    ["\xed\x47", u"\u6631", "\xfa\x63"],
    ["\xed\x48", u"\u68c8", "\xfa\x64"],
    ["\xed\x49", u"\u92f9", "\xfa\x65"],
    ["\xed\x4a", u"\u66fb", "\xfa\x66"],
    ["\xed\x4b", u"\u5f45", "\xfa\x67"],
    ["\xed\x4c", u"\u4e28", "\xfa\x68"],
    ["\xed\x4d", u"\u4ee1", "\xfa\x69"],
    ["\xed\x4e", u"\u4efc", "\xfa\x6a"],
    ["\xed\x4f", u"\u4f00", "\xfa\x6b"],
    ["\xed\x50", u"\u4f03", "\xfa\x6c"],
    ["\xed\x51", u"\u4f39", "\xfa\x6d"],
    ["\xed\x52", u"\u4f56", "\xfa\x6e"],
    ["\xed\x53", u"\u4f92", "\xfa\x6f"],
    ["\xed\x54", u"\u4f8a", "\xfa\x70"],
    ["\xed\x55", u"\u4f9a", "\xfa\x71"],
    ["\xed\x56", u"\u4f94", "\xfa\x72"],
    ["\xed\x57", u"\u4fcd", "\xfa\x73"],
    ["\xed\x58", u"\u5040", "\xfa\x74"],
    ["\xed\x59", u"\u5022", "\xfa\x75"],
    ["\xed\x5a", u"\u4fff", "\xfa\x76"],
    ["\xed\x5b", u"\u501e", "\xfa\x77"],
    ["\xed\x5c", u"\u5046", "\xfa\x78"],
    ["\xed\x5d", u"\u5070", "\xfa\x79"],
    ["\xed\x5e", u"\u5042", "\xfa\x7a"],
    ["\xed\x5f", u"\u5094", "\xfa\x7b"],
    ["\xed\x60", u"\u50f4", "\xfa\x7c"],
    ["\xed\x61", u"\u50d8", "\xfa\x7d"],
    ["\xed\x62", u"\u514a", "\xfa\x7e"],
    ["\xed\x63", u"\u5164", "\xfa\x80"],
    ["\xed\x64", u"\u519d", "\xfa\x81"],
    ["\xed\x65", u"\u51be", "\xfa\x82"],
    ["\xed\x66", u"\u51ec", "\xfa\x83"],
    ["\xed\x67", u"\u5215", "\xfa\x84"],
    ["\xed\x68", u"\u529c", "\xfa\x85"],
    ["\xed\x69", u"\u52a6", "\xfa\x86"],
    ["\xed\x6a", u"\u52c0", "\xfa\x87"],
    ["\xed\x6b", u"\u52db", "\xfa\x88"],
    ["\xed\x6c", u"\u5300", "\xfa\x89"],
    ["\xed\x6d", u"\u5307", "\xfa\x8a"],
    ["\xed\x6e", u"\u5324", "\xfa\x8b"],
    ["\xed\x6f", u"\u5372", "\xfa\x8c"],
    ["\xed\x70", u"\u5393", "\xfa\x8d"],
    ["\xed\x71", u"\u53b2", "\xfa\x8e"],
    ["\xed\x72", u"\u53dd", "\xfa\x8f"],
    ["\xed\x73", u"\ufa0e", "\xfa\x90"],
    ["\xed\x74", u"\u549c", "\xfa\x91"],
    ["\xed\x75", u"\u548a", "\xfa\x92"],
    ["\xed\x76", u"\u54a9", "\xfa\x93"],
    ["\xed\x77", u"\u54ff", "\xfa\x94"],
    ["\xed\x78", u"\u5586", "\xfa\x95"],
    ["\xed\x79", u"\u5759", "\xfa\x96"],
    ["\xed\x7a", u"\u5765", "\xfa\x97"],
    ["\xed\x7b", u"\u57ac", "\xfa\x98"],
    ["\xed\x7c", u"\u57c8", "\xfa\x99"],
    ["\xed\x7d", u"\u57c7", "\xfa\x9a"],
    ["\xed\x7e", u"\ufa0f", "\xfa\x9b"],
    ["\xed\x80", u"\ufa10", "\xfa\x9c"],
    ["\xed\x81", u"\u589e", "\xfa\x9d"],
    ["\xed\x82", u"\u58b2", "\xfa\x9e"],
    ["\xed\x83", u"\u590b", "\xfa\x9f"],
    ["\xed\x84", u"\u5953", "\xfa\xa0"],
    ["\xed\x85", u"\u595b", "\xfa\xa1"],
    ["\xed\x86", u"\u595d", "\xfa\xa2"],
    ["\xed\x87", u"\u5963", "\xfa\xa3"],
    ["\xed\x88", u"\u59a4", "\xfa\xa4"],
    ["\xed\x89", u"\u59ba", "\xfa\xa5"],
    ["\xed\x8a", u"\u5b56", "\xfa\xa6"],
    ["\xed\x8b", u"\u5bc0", "\xfa\xa7"],
    ["\xed\x8c", u"\u752f", "\xfa\xa8"],
    ["\xed\x8d", u"\u5bd8", "\xfa\xa9"],
    ["\xed\x8e", u"\u5bec", "\xfa\xaa"],
    ["\xed\x8f", u"\u5c1e", "\xfa\xab"],
    ["\xed\x90", u"\u5ca6", "\xfa\xac"],
    ["\xed\x91", u"\u5cba", "\xfa\xad"],
    ["\xed\x92", u"\u5cf5", "\xfa\xae"],
    ["\xed\x93", u"\u5d27", "\xfa\xaf"],
    ["\xed\x94", u"\u5d53", "\xfa\xb0"],
    ["\xed\x95", u"\ufa11", "\xfa\xb1"],
    ["\xed\x96", u"\u5d42", "\xfa\xb2"],
    ["\xed\x97", u"\u5d6d", "\xfa\xb3"],
    ["\xed\x98", u"\u5db8", "\xfa\xb4"],
    ["\xed\x99", u"\u5db9", "\xfa\xb5"],
    ["\xed\x9a", u"\u5dd0", "\xfa\xb6"],
    ["\xed\x9b", u"\u5f21", "\xfa\xb7"],
    ["\xed\x9c", u"\u5f34", "\xfa\xb8"],
    ["\xed\x9d", u"\u5f67", "\xfa\xb9"],
    ["\xed\x9e", u"\u5fb7", "\xfa\xba"],
    ["\xed\x9f", u"\u5fde", "\xfa\xbb"],
    ["\xed\xa0", u"\u605d", "\xfa\xbc"],
    ["\xed\xa1", u"\u6085", "\xfa\xbd"],
    ["\xed\xa2", u"\u608a", "\xfa\xbe"],
    ["\xed\xa3", u"\u60de", "\xfa\xbf"],
    ["\xed\xa4", u"\u60d5", "\xfa\xc0"],
    ["\xed\xa5", u"\u6120", "\xfa\xc1"],
    ["\xed\xa6", u"\u60f2", "\xfa\xc2"],
    ["\xed\xa7", u"\u6111", "\xfa\xc3"],
    ["\xed\xa8", u"\u6137", "\xfa\xc4"],
    ["\xed\xa9", u"\u6130", "\xfa\xc5"],
    ["\xed\xaa", u"\u6198", "\xfa\xc6"],
    ["\xed\xab", u"\u6213", "\xfa\xc7"],
    ["\xed\xac", u"\u62a6", "\xfa\xc8"],
    ["\xed\xad", u"\u63f5", "\xfa\xc9"],
    ["\xed\xae", u"\u6460", "\xfa\xca"],
    ["\xed\xaf", u"\u649d", "\xfa\xcb"],
    ["\xed\xb0", u"\u64ce", "\xfa\xcc"],
    ["\xed\xb1", u"\u654e", "\xfa\xcd"],
    ["\xed\xb2", u"\u6600", "\xfa\xce"],
    ["\xed\xb3", u"\u6615", "\xfa\xcf"],
    ["\xed\xb4", u"\u663b", "\xfa\xd0"],
    ["\xed\xb5", u"\u6609", "\xfa\xd1"],
    ["\xed\xb6", u"\u662e", "\xfa\xd2"],
    ["\xed\xb7", u"\u661e", "\xfa\xd3"],
    ["\xed\xb8", u"\u6624", "\xfa\xd4"],
    ["\xed\xb9", u"\u6665", "\xfa\xd5"],
    ["\xed\xba", u"\u6657", "\xfa\xd6"],
    ["\xed\xbb", u"\u6659", "\xfa\xd7"],
    ["\xed\xbc", u"\ufa12", "\xfa\xd8"],
    ["\xed\xbd", u"\u6673", "\xfa\xd9"],
    ["\xed\xbe", u"\u6699", "\xfa\xda"],
    ["\xed\xbf", u"\u66a0", "\xfa\xdb"],
    ["\xed\xc0", u"\u66b2", "\xfa\xdc"],
    ["\xed\xc1", u"\u66bf", "\xfa\xdd"],
    ["\xed\xc2", u"\u66fa", "\xfa\xde"],
    ["\xed\xc3", u"\u670e", "\xfa\xdf"],
    ["\xed\xc4", u"\uf929", "\xfa\xe0"],
    ["\xed\xc5", u"\u6766", "\xfa\xe1"],
    ["\xed\xc6", u"\u67bb", "\xfa\xe2"],
    ["\xed\xc7", u"\u6852", "\xfa\xe3"],
    ["\xed\xc8", u"\u67c0", "\xfa\xe4"],
    ["\xed\xc9", u"\u6801", "\xfa\xe5"],
    ["\xed\xca", u"\u6844", "\xfa\xe6"],
    ["\xed\xcb", u"\u68cf", "\xfa\xe7"],
    ["\xed\xcc", u"\ufa13", "\xfa\xe8"],
    ["\xed\xcd", u"\u6968", "\xfa\xe9"],
    ["\xed\xce", u"\ufa14", "\xfa\xea"],
    ["\xed\xcf", u"\u6998", "\xfa\xeb"],
    ["\xed\xd0", u"\u69e2", "\xfa\xec"],
    ["\xed\xd1", u"\u6a30", "\xfa\xed"],
    ["\xed\xd2", u"\u6a6b", "\xfa\xee"],
    ["\xed\xd3", u"\u6a46", "\xfa\xef"],
    ["\xed\xd4", u"\u6a73", "\xfa\xf0"],
    ["\xed\xd5", u"\u6a7e", "\xfa\xf1"],
    ["\xed\xd6", u"\u6ae2", "\xfa\xf2"],
    ["\xed\xd7", u"\u6ae4", "\xfa\xf3"],
    ["\xed\xd8", u"\u6bd6", "\xfa\xf4"],
    ["\xed\xd9", u"\u6c3f", "\xfa\xf5"],
    ["\xed\xda", u"\u6c5c", "\xfa\xf6"],
    ["\xed\xdb", u"\u6c86", "\xfa\xf7"],
    ["\xed\xdc", u"\u6c6f", "\xfa\xf8"],
    ["\xed\xdd", u"\u6cda", "\xfa\xf9"],
    ["\xed\xde", u"\u6d04", "\xfa\xfa"],
    ["\xed\xdf", u"\u6d87", "\xfa\xfb"],
    ["\xed\xe0", u"\u6d6f", "\xfa\xfc"],
    ["\xed\xe1", u"\u6d96", "\xfb\x40"],
    ["\xed\xe2", u"\u6dac", "\xfb\x41"],
    ["\xed\xe3", u"\u6dcf", "\xfb\x42"],
    ["\xed\xe4", u"\u6df8", "\xfb\x43"],
    ["\xed\xe5", u"\u6df2", "\xfb\x44"],
    ["\xed\xe6", u"\u6dfc", "\xfb\x45"],
    ["\xed\xe7", u"\u6e39", "\xfb\x46"],
    ["\xed\xe8", u"\u6e5c", "\xfb\x47"],
    ["\xed\xe9", u"\u6e27", "\xfb\x48"],
    ["\xed\xea", u"\u6e3c", "\xfb\x49"],
    ["\xed\xeb", u"\u6ebf", "\xfb\x4a"],
    ["\xed\xec", u"\u6f88", "\xfb\x4b"],
    ["\xed\xed", u"\u6fb5", "\xfb\x4c"],
    ["\xed\xee", u"\u6ff5", "\xfb\x4d"],
    ["\xed\xef", u"\u7005", "\xfb\x4e"],
    ["\xed\xf0", u"\u7007", "\xfb\x4f"],
    ["\xed\xf1", u"\u7028", "\xfb\x50"],
    ["\xed\xf2", u"\u7085", "\xfb\x51"],
    ["\xed\xf3", u"\u70ab", "\xfb\x52"],
    ["\xed\xf4", u"\u710f", "\xfb\x53"],
    ["\xed\xf5", u"\u7104", "\xfb\x54"],
    ["\xed\xf6", u"\u715c", "\xfb\x55"],
    ["\xed\xf7", u"\u7146", "\xfb\x56"],
    ["\xed\xf8", u"\u7147", "\xfb\x57"],
    ["\xed\xf9", u"\ufa15", "\xfb\x58"],
    ["\xed\xfa", u"\u71c1", "\xfb\x59"],
    ["\xed\xfb", u"\u71fe", "\xfb\x5a"],
    ["\xed\xfc", u"\u72b1", "\xfb\x5b"],
    ["\xee\x40", u"\u72be", "\xfb\x5c"],
    ["\xee\x41", u"\u7324", "\xfb\x5d"],
    ["\xee\x42", u"\ufa16", "\xfb\x5e"],
    ["\xee\x43", u"\u7377", "\xfb\x5f"],
    ["\xee\x44", u"\u73bd", "\xfb\x60"],
    ["\xee\x45", u"\u73c9", "\xfb\x61"],
    ["\xee\x46", u"\u73d6", "\xfb\x62"],
    ["\xee\x47", u"\u73e3", "\xfb\x63"],
    ["\xee\x48", u"\u73d2", "\xfb\x64"],
    ["\xee\x49", u"\u7407", "\xfb\x65"],
    ["\xee\x4a", u"\u73f5", "\xfb\x66"],
    ["\xee\x4b", u"\u7426", "\xfb\x67"],
    ["\xee\x4c", u"\u742a", "\xfb\x68"],
    ["\xee\x4d", u"\u7429", "\xfb\x69"],
    ["\xee\x4e", u"\u742e", "\xfb\x6a"],
    ["\xee\x4f", u"\u7462", "\xfb\x6b"],
    ["\xee\x50", u"\u7489", "\xfb\x6c"],
    ["\xee\x51", u"\u749f", "\xfb\x6d"],
    ["\xee\x52", u"\u7501", "\xfb\x6e"],
    ["\xee\x53", u"\u756f", "\xfb\x6f"],
    ["\xee\x54", u"\u7682", "\xfb\x70"],
    ["\xee\x55", u"\u769c", "\xfb\x71"],
    ["\xee\x56", u"\u769e", "\xfb\x72"],
    ["\xee\x57", u"\u769b", "\xfb\x73"],
    ["\xee\x58", u"\u76a6", "\xfb\x74"],
    ["\xee\x59", u"\ufa17", "\xfb\x75"],
    ["\xee\x5a", u"\u7746", "\xfb\x76"],
    ["\xee\x5b", u"\u52af", "\xfb\x77"],
    ["\xee\x5c", u"\u7821", "\xfb\x78"],
    ["\xee\x5d", u"\u784e", "\xfb\x79"],
    ["\xee\x5e", u"\u7864", "\xfb\x7a"],
    ["\xee\x5f", u"\u787a", "\xfb\x7b"],
    ["\xee\x60", u"\u7930", "\xfb\x7c"],
    ["\xee\x61", u"\ufa18", "\xfb\x7d"],
    ["\xee\x62", u"\ufa19", "\xfb\x7e"],
    ["\xee\x63", u"\ufa1a", "\xfb\x80"],
    ["\xee\x64", u"\u7994", "\xfb\x81"],
    ["\xee\x65", u"\ufa1b", "\xfb\x82"],
    ["\xee\x66", u"\u799b", "\xfb\x83"],
    ["\xee\x67", u"\u7ad1", "\xfb\x84"],
    ["\xee\x68", u"\u7ae7", "\xfb\x85"],
    ["\xee\x69", u"\ufa1c", "\xfb\x86"],
    ["\xee\x6a", u"\u7aeb", "\xfb\x87"],
    ["\xee\x6b", u"\u7b9e", "\xfb\x88"],
    ["\xee\x6c", u"\ufa1d", "\xfb\x89"],
    ["\xee\x6d", u"\u7d48", "\xfb\x8a"],
    ["\xee\x6e", u"\u7d5c", "\xfb\x8b"],
    ["\xee\x6f", u"\u7db7", "\xfb\x8c"],
    ["\xee\x70", u"\u7da0", "\xfb\x8d"],
    ["\xee\x71", u"\u7dd6", "\xfb\x8e"],
    ["\xee\x72", u"\u7e52", "\xfb\x8f"],
    ["\xee\x73", u"\u7f47", "\xfb\x90"],
    ["\xee\x74", u"\u7fa1", "\xfb\x91"],
    ["\xee\x75", u"\ufa1e", "\xfb\x92"],
    ["\xee\x76", u"\u8301", "\xfb\x93"],
    ["\xee\x77", u"\u8362", "\xfb\x94"],
    ["\xee\x78", u"\u837f", "\xfb\x95"],
    ["\xee\x79", u"\u83c7", "\xfb\x96"],
    ["\xee\x7a", u"\u83f6", "\xfb\x97"],
    ["\xee\x7b", u"\u8448", "\xfb\x98"],
    ["\xee\x7c", u"\u84b4", "\xfb\x99"],
    ["\xee\x7d", u"\u8553", "\xfb\x9a"],
    ["\xee\x7e", u"\u8559", "\xfb\x9b"],
    ["\xee\x80", u"\u856b", "\xfb\x9c"],
    ["\xee\x81", u"\ufa1f", "\xfb\x9d"],
    ["\xee\x82", u"\u85b0", "\xfb\x9e"],
    ["\xee\x83", u"\ufa20", "\xfb\x9f"],
    ["\xee\x84", u"\ufa21", "\xfb\xa0"],
    ["\xee\x85", u"\u8807", "\xfb\xa1"],
    ["\xee\x86", u"\u88f5", "\xfb\xa2"],
    ["\xee\x87", u"\u8a12", "\xfb\xa3"],
    ["\xee\x88", u"\u8a37", "\xfb\xa4"],
    ["\xee\x89", u"\u8a79", "\xfb\xa5"],
    ["\xee\x8a", u"\u8aa7", "\xfb\xa6"],
    ["\xee\x8b", u"\u8abe", "\xfb\xa7"],
    ["\xee\x8c", u"\u8adf", "\xfb\xa8"],
    ["\xee\x8d", u"\ufa22", "\xfb\xa9"],
    ["\xee\x8e", u"\u8af6", "\xfb\xaa"],
    ["\xee\x8f", u"\u8b53", "\xfb\xab"],
    ["\xee\x90", u"\u8b7f", "\xfb\xac"],
    ["\xee\x91", u"\u8cf0", "\xfb\xad"],
    ["\xee\x92", u"\u8cf4", "\xfb\xae"],
    ["\xee\x93", u"\u8d12", "\xfb\xaf"],
    ["\xee\x94", u"\u8d76", "\xfb\xb0"],
    ["\xee\x95", u"\ufa23", "\xfb\xb1"],
    ["\xee\x96", u"\u8ecf", "\xfb\xb2"],
    ["\xee\x97", u"\ufa24", "\xfb\xb3"],
    ["\xee\x98", u"\ufa25", "\xfb\xb4"],
    ["\xee\x99", u"\u9067", "\xfb\xb5"],
    ["\xee\x9a", u"\u90de", "\xfb\xb6"],
    ["\xee\x9b", u"\ufa26", "\xfb\xb7"],
    ["\xee\x9c", u"\u9115", "\xfb\xb8"],
    ["\xee\x9d", u"\u9127", "\xfb\xb9"],
    ["\xee\x9e", u"\u91da", "\xfb\xba"],
    ["\xee\x9f", u"\u91d7", "\xfb\xbb"],
    ["\xee\xa0", u"\u91de", "\xfb\xbc"],
    ["\xee\xa1", u"\u91ed", "\xfb\xbd"],
    ["\xee\xa2", u"\u91ee", "\xfb\xbe"],
    ["\xee\xa3", u"\u91e4", "\xfb\xbf"],
    ["\xee\xa4", u"\u91e5", "\xfb\xc0"],
    ["\xee\xa5", u"\u9206", "\xfb\xc1"],
    ["\xee\xa6", u"\u9210", "\xfb\xc2"],
    ["\xee\xa7", u"\u920a", "\xfb\xc3"],
    ["\xee\xa8", u"\u923a", "\xfb\xc4"],
    ["\xee\xa9", u"\u9240", "\xfb\xc5"],
    ["\xee\xaa", u"\u923c", "\xfb\xc6"],
    ["\xee\xab", u"\u924e", "\xfb\xc7"],
    ["\xee\xac", u"\u9259", "\xfb\xc8"],
    ["\xee\xad", u"\u9251", "\xfb\xc9"],
    ["\xee\xae", u"\u9239", "\xfb\xca"],
    ["\xee\xaf", u"\u9267", "\xfb\xcb"],
    ["\xee\xb0", u"\u92a7", "\xfb\xcc"],
    ["\xee\xb1", u"\u9277", "\xfb\xcd"],
    ["\xee\xb2", u"\u9278", "\xfb\xce"],
    ["\xee\xb3", u"\u92e7", "\xfb\xcf"],
    ["\xee\xb4", u"\u92d7", "\xfb\xd0"],
    ["\xee\xb5", u"\u92d9", "\xfb\xd1"],
    ["\xee\xb6", u"\u92d0", "\xfb\xd2"],
    ["\xee\xb7", u"\ufa27", "\xfb\xd3"],
    ["\xee\xb8", u"\u92d5", "\xfb\xd4"],
    ["\xee\xb9", u"\u92e0", "\xfb\xd5"],
    ["\xee\xba", u"\u92d3", "\xfb\xd6"],
    ["\xee\xbb", u"\u9325", "\xfb\xd7"],
    ["\xee\xbc", u"\u9321", "\xfb\xd8"],
    ["\xee\xbd", u"\u92fb", "\xfb\xd9"],
    ["\xee\xbe", u"\ufa28", "\xfb\xda"],
    ["\xee\xbf", u"\u931e", "\xfb\xdb"],
    ["\xee\xc0", u"\u92ff", "\xfb\xdc"],
    ["\xee\xc1", u"\u931d", "\xfb\xdd"],
    ["\xee\xc2", u"\u9302", "\xfb\xde"],
    ["\xee\xc3", u"\u9370", "\xfb\xdf"],
    ["\xee\xc4", u"\u9357", "\xfb\xe0"],
    ["\xee\xc5", u"\u93a4", "\xfb\xe1"],
    ["\xee\xc6", u"\u93c6", "\xfb\xe2"],
    ["\xee\xc7", u"\u93de", "\xfb\xe3"],
    ["\xee\xc8", u"\u93f8", "\xfb\xe4"],
    ["\xee\xc9", u"\u9431", "\xfb\xe5"],
    ["\xee\xca", u"\u9445", "\xfb\xe6"],
    ["\xee\xcb", u"\u9448", "\xfb\xe7"],
    ["\xee\xcc", u"\u9592", "\xfb\xe8"],
    ["\xee\xcd", u"\uf9dc", "\xfb\xe9"],
    ["\xee\xce", u"\ufa29", "\xfb\xea"],
    ["\xee\xcf", u"\u969d", "\xfb\xeb"],
    ["\xee\xd0", u"\u96af", "\xfb\xec"],
    ["\xee\xd1", u"\u9733", "\xfb\xed"],
    ["\xee\xd2", u"\u973b", "\xfb\xee"],
    ["\xee\xd3", u"\u9743", "\xfb\xef"],
    ["\xee\xd4", u"\u974d", "\xfb\xf0"],
    ["\xee\xd5", u"\u974f", "\xfb\xf1"],
    ["\xee\xd6", u"\u9751", "\xfb\xf2"],
    ["\xee\xd7", u"\u9755", "\xfb\xf3"],
    ["\xee\xd8", u"\u9857", "\xfb\xf4"],
    ["\xee\xd9", u"\u9865", "\xfb\xf5"],
    ["\xee\xda", u"\ufa2a", "\xfb\xf6"],
    ["\xee\xdb", u"\ufa2b", "\xfb\xf7"],
    ["\xee\xdc", u"\u9927", "\xfb\xf8"],
    ["\xee\xdd", u"\ufa2c", "\xfb\xf9"],
    ["\xee\xde", u"\u999e", "\xfb\xfa"],
    ["\xee\xdf", u"\u9a4e", "\xfb\xfb"],
    ["\xee\xe0", u"\u9ad9", "\xfb\xfc"],
    ["\xee\xe1", u"\u9adc", "\xfc\x40"],
    ["\xee\xe2", u"\u9b75", "\xfc\x41"],
    ["\xee\xe3", u"\u9b72", "\xfc\x42"],
    ["\xee\xe4", u"\u9b8f", "\xfc\x43"],
    ["\xee\xe5", u"\u9bb1", "\xfc\x44"],
    ["\xee\xe6", u"\u9bbb", "\xfc\x45"],
    ["\xee\xe7", u"\u9c00", "\xfc\x46"],
    ["\xee\xe8", u"\u9d70", "\xfc\x47"],
    ["\xee\xe9", u"\u9d6b", "\xfc\x48"],
    ["\xee\xea", u"\ufa2d", "\xfc\x49"],
    ["\xee\xeb", u"\u9e19", "\xfc\x4a"],
    ["\xee\xec", u"\u9ed1", "\xfc\x4b"]]

def test_non_round_trip_mapping(encoding):
    for f, u, t in NON_ROUND_TRIP_MAPPING:
        assert u == unicode(f, encoding)
        assert t == unicode(f, encoding).encode(encoding)

def sanity():
    for h in range(0x81, 0xa0):
        for l in range(0x40, 0xed):
            if l in (0x7e, 0x7f):
                continue

            u1=u2="***"
            c = chr(h) + chr(l)
            u1 = unicode(c, "mbcs", "strict")
            try:
                u2 = unicode(c, "japanese.ms932", "strict")
            except UnicodeError:
                if ord(u1) <> 0x30fb:
                    raise
                else:
                    continue
            assert u1 == u2

            u1=u2="***"
            c = chr(h) + chr(l)
            u1 = unicode(c, "mbcs", "strict")
            try:
                u2 = unicode(c, "japanese.ms932", "strict")
            except UnicodeError:
                if ord(u1) <> 0x30fb:
                    raise
                else:
                    continue
            assert u1 == u2

    for h in range(0xe0, 0xfd):
        for l in range(0x40, 0xed):
            if l in (0x7e, 0x7f):
                continue
            u1=u2="***"
            c = chr(h) + chr(l)
            u1 = unicode(c, "mbcs", "strict")
            try:
                u2 = unicode(c, "japanese.ms932", "strict")
            except UnicodeError:
                if ord(u1) <> 0x30fb:
                    raise
                else:
                    continue
            assert u1 == u2

            u1=u2="***"
            c = chr(h) + chr(l)
            u1 = unicode(c, "mbcs", "strict")
            try:
                u2 = unicode(c, "japanese.ms932", "strict")
            except UnicodeError:
                if ord(u1) <> 0x30fb:
                    raise
                else:
                    continue
            assert u1 == u2

def sanity2():
    for i in range(65536):
        try:
            u1 = unichr(i).encode("mbcs")
        except UnicodeError:
            u1 = '____'
        
        try:
            u2 = unichr(i).encode("japanese.ms932")
        except UnicodeError:
            u2 = '____'
        
        if u1 != '?':
            assert u1 == u2

test("japanese.c.ms932")
#import sys
#if sys.platform == "win32":
#    sanity()
#    sanity2()
