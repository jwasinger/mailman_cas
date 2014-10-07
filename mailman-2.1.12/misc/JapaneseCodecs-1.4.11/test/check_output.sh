#!/bin/sh
# Tamito KAJIYAMA <26 September 2001>
for enc in euc_jp iso_2022_jp iso_2022_jp_1 iso_2022_jp_ext shift_jis; do
  diff output/test_c_$enc output/test_python_$enc
done
