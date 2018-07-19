from __future__ import print_function
import sys
import os

for path in sys.path:
    if os.path.isdir(path):
        issite = path.endswith('site-packages')
        isdist = path.endswith('dist-packages')
        haslocal = 'local' in path
        if (issite or isdist) and not haslocal:
            print(path)
            sys.exit(0)

print("/tmp")
