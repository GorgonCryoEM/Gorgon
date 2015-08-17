#!/usr/bin/env python

import os, sys

dirs = os.walk(os.path.curdir)

for d in dirs:
#     print d
    
    package = d[0]
    files   = d[2]
    n = len(files)

    if n > 1:
        dir = "from %s import " %os.path.basename(package)
        imports = [os.path.splitext(files[m])[0] for m in range(1,n)]
#         Print per subpackage
#         imports = ', '.join(imports)
#         print "%s %s" % (dir, imports)
#       Print per module
        for i in imports:
            print dir+i
