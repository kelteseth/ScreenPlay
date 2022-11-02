import subprocess
import os
import shutil
from sys import stdout

stdout.reconfigure(encoding='utf-8')

def main():
    print("\nRuns qdoc with the settings from Docs/config.qdocconf. For this you need to have Qt 6.1 installed!")
    
    shutil.rmtree('../Docs/html/')
    os.mkdir('../Docs/html/')
    if os.name == "nt":
        process = subprocess.Popen("C:\\Qt\\6.1.0\\msvc2019_64\\bin\\qdoc.exe ../Docs/config.qdocconf", stdout=subprocess.PIPE)
    else:
        process = subprocess.Popen("qdoc ../Docs/configCI.qdocconf", stdout=subprocess.PIPE)

if __name__ == "__main__":
    main()
