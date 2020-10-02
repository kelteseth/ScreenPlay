setlocal EnableExtensions

set PATH=%PATH%;C:\Qt\Tools\QtCreator\bin
set PATH=%PATH%;C:\Qt\5.15.1\msvc2019_64\bin
set root=%cd%

cd translations

lupdate.exe -extensions qml "../qml" -ts ^
ScreenPlay_ru.ts ^
ScreenPlay_de.ts ^
ScreenPlay_en.ts ^
ScreenPlay_fr.ts ^
ScreenPlay_ko.ts ^
ScreenPlay_vi.ts ^
ScreenPlay_es.ts ^
ScreenPlay_zh-cn.ts ^
 
pause


