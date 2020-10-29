import fnmatch
import os
import sys
import subprocess

executable = "clang-format"
if os.name == 'nt':
    executable += ".exe"

for root, dirnames, files in os.walk('../'):
      for filename in files:
        if filename.endswith(('.cpp', '.h')):
            print(executable, root+"/"+filename)
            os.system(" %s -style=file -i %s" % (executable, (root + "/" + filename)))

# Check if all files are formatter
output = subprocess.check_output("git diff", shell=True)

if output:
    print("Git diff is not empty. This means your code was not formatted via %s!" % executable)
    sys.exit(1)
