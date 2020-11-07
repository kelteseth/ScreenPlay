import fnmatch
import os
import sys
import subprocess

executable = "cmake-lint"
if os.name == 'nt':
    executable += ".exe"

for root, dirnames, filenames in os.walk('../'):
    for filename in fnmatch.filter(filenames, 'CMakeLists.txt'):
        print(executable, root + "/" + filename)
        os.system(" %s %s" % (executable, (root + "/" + filename)))
