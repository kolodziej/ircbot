from __future__ import print_function
import sys
import os
import re

def PythonVersion(version_string):
    m = re.search('^([23]\.[0-9])', version_string)
    if not m:
        raise Exception('Cannot find version!')
    return m.group(0)

version_string = sys.argv[1] if len(sys.argv) > 1 else sys.version
version = PythonVersion(version_string)

for path in sys.path:
    if os.path.isdir(path):
        issite = path.endswith('site-packages')
        isdist = path.endswith('dist-packages')
        haslocal = 'local' in path
        hasversion = version in path
        if (issite or isdist) and not haslocal and hasversion:
            print(path)
            sys.exit(0)

print("/tmp")
