#!/bin/bash

Msys2Path='C:\\msys64\\mingw64\\bin'
args=' --proto_path=../ --cpp_out=../GenCode ../*.proto'
exec $Msys2Path'\\'protoc.exe $args
pause