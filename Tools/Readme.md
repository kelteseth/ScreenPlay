## Tools

This folder contains serveral python tools to help with development. Depending on your os you have to change the python command to ```python```, ```python3``` or ```python3.9```

#### setup.py
- Installs third party c++ dependencies for all platforms
    - ```python3 setup.py -u=xxx -p=xxx```
#### build.py
- Build ScreenPlay locally
    - ```python3 build.py -t=release```
#### clang_format.py
- Invokers clang-format for all .cpp and .h files
    - ```python3 clang_format.py ```
#### qml_format.py
- Calls qmlformat for all qml files
    - ```python3 qml_format.py```
#### qdoc.py
- Builds documentation for all projects into Docs/html
    - ```python3 qdoc.py```
#### download_ffmpeg.py
- Donwload ffmpeg for macos only for now
    - ```python3 download_ffmpeg.py```

#### steam_publish.py
- Upload to steam. This is needed to add some much needed output like git commit to the steam ui for selecting branches
    - ```python3 steam_publish.py -u xxx -p xxx```

