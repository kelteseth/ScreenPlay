import fnmatch
import os
import sys
import subprocess

executable = "cmake-format"
if os.name == 'nt':
    executable += ".exe"

for root, dirnames, filenames in os.walk('../'):
    for filename in fnmatch.filter(filenames, 'CMakeLists.txt'):
        print(executable, root+"/"+filename)
        os.system(" %s -c ../.cmake-format.py -i %s" % (executable, (root + "/" + filename)))

# Check if all files are formatter
output = subprocess.check_output("git diff", shell=True)

if output:
    print("Git diff is not empty. This means your CMakeLists.txt file was not formatted via %s!" % executable)
    sys.exit(1)
