#!/bin/bash

read -p 'Type your ABSOLUTE Qt "bin" directory (default is /usr/bin): ' path

if [ o$path == "o" ]
then
    echo "Using default directory..."
    path="/usr/bin"
fi

echo "Testing directory..."
if [[ -f "$path/lupdate" ]] && [[ -f "$path/lrelease" ]]
then
    echo "Begin update translations..."
    for f in ls *.ts
	do
        echo "Processing $f (1/2)"
        $path/lupdate -extensions qml ../qml -ts $f
        echo "Processing $f (2/2)"
        $path/lrelease $f
        echo "Done"
	done
    echo "Updated translations."
else
    echo "lupdate or lrelease dosen't exist, failed to update translations..."
fi
echo Enter to exit.
read
