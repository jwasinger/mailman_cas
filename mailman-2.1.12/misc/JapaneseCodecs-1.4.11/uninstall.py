# Tamito KAJIYAMA <12 December 2000>

import os
import sys

def remove_module(dir, name):
    for ext in [".py", ".pyc", ".pyo"]:
        path = os.path.join(dir, name + ext)
        if os.path.exists(path):
            os.remove(path)

def main():
    libdir = os.path.join(sys.prefix, 'lib', 'python'+sys.version[:3],
                          'encodings')
    for module in ["euc_jp", "shift_jis", "jis_7",
                   os.path.join("japanese", "__init__"),
                   os.path.join("japanese", "euc2utf"),
                   os.path.join("japanese", "utf2euc"),
                   os.path.join("japanese", "sjis2utf"),
                   os.path.join("japanese", "utf2sjis")]:
        remove_module(libdir, module)
    os.rmdir(os.path.join(libdir, "japanese"))

try:
    main()
except OSError, e:
    sys.stderr.write(str(e) + "\n")
