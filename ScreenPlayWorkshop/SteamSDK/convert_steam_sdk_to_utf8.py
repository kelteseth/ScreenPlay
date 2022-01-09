import json
import os

# open all files in subfolder public and convert to utf8
for root, dirs, files in os.walk("public/steam"):
    for file in files:    
        if file.endswith(".h"):
            path = os.path.join(root, file)
            data = ""
            try:
                with open(path, 'r', encoding='utf-8', errors='strict') as f:
                    data = f.read()
            except UnicodeDecodeError:
                print("Converting " + file)
                with open(path, 'r', encoding='cp1252') as infile:
                    data = infile.read()
                with open(path, 'w', encoding='utf-8') as outfile:
                    outfile.write(data)
