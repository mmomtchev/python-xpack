import sys
print(sys.path)

import lzma
assert(type(lzma.compress) == type(lambda x: x))
