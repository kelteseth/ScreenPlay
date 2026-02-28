#!/usr/bin/python3
# SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
import subprocess
from sys import stdout
from rich_console import console

stdout.reconfigure(encoding='utf-8')


def execute(command, workingDir=".", ignore_error=False, use_shell=True, print_command=True) -> bool:

    if print_command:
        console.print(f"[bold green] Executing:[/] {command}")

    process = subprocess.Popen(
        command, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, shell=use_shell, cwd=workingDir)

    while True:
        if process.poll() is not None:
            break
        byte_line = process.stdout.readline()
        text_line = byte_line.decode('utf8', errors='ignore')
        if text_line:
            line = text_line.strip()
            if ' warning' in text_line.lower():
                console.print(f"[yellow]{line}[/]")
            elif ' error' in text_line.lower():
                console.print(f"[red]{line}[/]")
            else:
                console.print(line)

    process.communicate()
    exitCode = process.returncode
    if exitCode:
        if ignore_error:
            console.print(f"[yellow]Ignore error {exitCode}[/]")
        else:
            return exitCode