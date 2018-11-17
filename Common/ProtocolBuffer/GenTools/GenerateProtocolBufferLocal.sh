#!/bin/bash

Msys2Path='C:\\msys64\\mingw64\\bin'
args=' --proto_path=../ --cpp_out=../GenCode ../*.proto'
exec ../GenTools/protoc.exe $args #// for local compile on windows
pause