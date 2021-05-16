import subprocess

# Python program to print
# colored text and background
def printRed(skk): print("\033[91m {}\033[0;37;40m" .format(skk))
def printGreen(skk): print("\033[92m {}\033[0;37;40m" .format(skk))
def printYellow(skk): print("\033[93m {}\033[0;37;40m" .format(skk))
def printLightPurple(skk): print("\033[94m {}\033[0;37;40m" .format(skk))
def printPurple(skk): print("\033[95m {}\033[0;37;40m" .format(skk))
def printCyan(skk): print("\033[96m {}\033[0;37;40m" .format(skk))
def printLightGray(skk): print("\033[97m {}\033[0;37;40m" .format(skk))
def printBlack(skk): print("\033[98m {}\033[0;37;40m" .format(skk))

def execute(command, workingDir=".", ignore_error=False, use_shell=True):
    process = subprocess.Popen(
        command, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, shell=use_shell, cwd=workingDir)

    while True:
        if process.poll() is not None:
            break
        byte_line = process.stdout.readline()
        text_line = byte_line.decode('utf8', errors='ignore')
        if text_line:
            if ' warning' in text_line.lower():
                printYellow(text_line.strip())
            elif ' error' in text_line.lower():
                printRed(text_line.strip())
            else:
                print(text_line.strip())

    process.communicate()
    exitCode = process.returncode
    if exitCode and ignore_error:
        printYellow("Ignore error ")
        return

    if exitCode:
        raise subprocess.CalledProcessError(exitCode, command)
