import fnmatch
import os
import sys
import subprocess

for root, dirnames, filenames in os.walk('..'):
    for filename in fnmatch.filter(filenames, 'CMakeLists.txt'):
        print("cmake-format  -c ../.cmake-format.py -i " + root + "/" + filename)
        os.system("cmake-format -c ../.cmake-format.py -i %s" % ((root + "/" + filename)))

# Check if all files are formatter
output = subprocess.check_output("git diff", shell=True)

if output:
    print("Git diff is not empty. This means your CMakeLists.txt file was not formatted!")
    #print(output)
    sys.exit(1)
