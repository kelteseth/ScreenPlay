TEMPLATE = app
QT += qml quick widgets quickcontrols2 core webengine
CONFIG += c++17

TARGETPATH = ScreenPlay

include($$PWD/../Common/qt-google-analytics/qt-google-analytics.pri)


ICON = favicon.ico

DEFINES += QT_DEPRECATED_WARNINGS

GIT_VERSION = $$system(git describe --always)
DEFINES += GIT_VERSION=\\\"$$GIT_VERSION\\\"

DISTFILES += \
    $$PWD/favicon.ico \

RESOURCES += \
    $$PWD/Resources.qrc

SOURCES +=  \
    $$PWD/main.cpp \
    $$PWD/app.cpp \
    $$PWD/src/createimportvideo.cpp \
    $$PWD/src/installedlistmodel.cpp \
    $$PWD/src/monitorlistmodel.cpp \
    $$PWD/src/screenplaywallpaper.cpp \
    $$PWD/src/screenplaywidget.cpp \
    $$PWD/src/settings.cpp \
    $$PWD/src/profilelistmodel.cpp \
    $$PWD/src/installedlistfilter.cpp \
    $$PWD/src/sdkconnector.cpp \
    $$PWD/src/projectsettingslistmodel.cpp \
    $$PWD/src/screenplaymanager.cpp \
    $$PWD/src/util.cpp \
    $$PWD/src/create.cpp

TRANSLATIONS = \
    $$PWD/translations/ScreenPlay_en.ts   \
    $$PWD/translations/ScreenPlay_de.ts   \
    $$PWD/translations/ScreenPlay_ru.ts   \
    $$PWD/translations/ScreenPlay_es.ts   \
    $$PWD/translations/ScreenPlay_fr.ts   \
    $$PWD/translations/ScreenPlay_ko.ts   \
    $$PWD/translations/ScreenPlay_vi.ts   \

HEADERS += \
    $$PWD/app.h \
    $$PWD/src/globalvariables.h \
    $$PWD/src/createimportvideo.h \
    $$PWD/src/installedlistmodel.h \
    $$PWD/src/monitorlistmodel.h \
    $$PWD/src/screenplaywallpaper.h \
    $$PWD/src/screenplaywidget.h \
    $$PWD/src/settings.h \
    $$PWD/src/profilelistmodel.h \
    $$PWD/src/profile.h \
    $$PWD/src/projectfile.h \
    $$PWD/src/installedlistfilter.h \
    $$PWD/src/sdkconnector.h \
    $$PWD/src/projectsettingslistitem.h \
    $$PWD/src/projectsettingslistmodel.h \
    $$PWD/src/screenplaymanager.h \
    $$PWD/src/util.h \
    $$PWD/src/create.h

INCLUDEPATH += \
    $$PWD/src/\

CONFIG(debug, debug|release) {
    install_assets.path = $${OUT_PWD}/assets/fonts
} else {
    install_assets.path = $${OUT_PWD}/assets/fonts
}

install_assets.files += $$PWD/assets/fonts/NotoSansCJKkr-Regular.otf

win32 {
    RC_ICONS += favicon.ico
    CONFIG(debug, debug|release) {
        install_it.path = $${OUT_PWD}/debug/
    } else {
        install_it.path = $${OUT_PWD}/release/
    }

    INCLUDEPATH += $$PWD/../Common/vcpkg/installed/x64-windows/include
    DEPENDPATH  += $$PWD/../Common/vcpkg/installed/x64-windows/include

    LIBS += -luser32
    LIBS += -L$$PWD/../Common/vcpkg/installed/x64-windows/lib/ -llibzippp

    CONFIG(debug, debug|release) {
        install_it.files += $$PWD/../Common/vcpkg/installed/x64-windows/debug/bin/zip.dll \
                            $$PWD/../Common/vcpkg/installed/x64-windows/debug/bin/zlibd1.dll \
                            $$PWD/../Common/vcpkg/installed/x64-windows/debug/bin/libzippp.dll \
                            $$PWD/../Common/vcpkg/installed/x64-windows/debug/bin/bz2d.dll \
                            $$PWD/../Common/vcpkg/installed/x64-windows/debug/bin/libcrypto-1_1-x64.dll \
                            $$PWD/../Common/vcpkg/installed/x64-windows/debug/bin/libssl-1_1-x64.dll \

    } else {
        install_it.files += $$PWD/../Common/vcpkg/installed/x64-windows/bin/zip.dll \
                            $$PWD/../Common/vcpkg/installed/x64-windows/bin/zlib1.dll \
                            $$PWD/../Common/vcpkg/installed/x64-windows/bin/libzippp.dll \
                            $$PWD/../Common/vcpkg/installed/x64-windows/bin/bz2.dll \
                            $$PWD/../Common/vcpkg/installed/x64-windows/bin/libcrypto-1_1-x64.dll \
                            $$PWD/../Common/vcpkg/installed/x64-windows/bin/libssl-1_1-x64.dll \
    }

}



macx {
    LIBS += -L$$PWD/ThirdParty/steam/redistributable_bin/osx32/ -lsteam_api
    DEPENDPATH += $$PWD/ThirdParty/steam/redistributable_bin/osx32

    INCLUDEPATH += $$PWD/ThirdParty/steam/
    DEPENDPATH += $$PWD/ThirdParty/steam/

    LIBS += -L$$PWD/ThirdParty/steam/lib/osx32/ -lsdkencryptedappticket

    steam_data.files += steam_appid.txt
    steam_data.path = Contents/MacOS
    steam_data_lib.files += $$PWD/ThirdParty/steam/redistributable_bin/osx32/libsteam_api.dylib
    steam_data_lib.path = Contents/MacOS/
    QMAKE_BUNDLE_DATA += steam_data
    QMAKE_BUNDLE_DATA += steam_data_lib

}

unix {
    INCLUDEPATH += $$PWD/../Common/vcpkg/installed/x64-linux/include
    DEPENDPATH  += $$PWD/../Common/vcpkg/installed/x64-linux/include


    install_it.path = $${OUT_PWD}

    CONFIG(debug, debug|release) {
        LIBS += -L$$PWD/../Common/vcpkg/installed/x64-linux/debug/lib/  -llibzippp -lzip -lbz2d -lz -lcrypto -lssl -ldl

        install_it.files += $$PWD/../Common/vcpkg/installed/x64-linux/debug/bin/libbz2d.so \
                            $$PWD/../Common/vcpkg/installed/x64-linux/debug/bin/libcrypto.so \
                            $$PWD/../Common/vcpkg/installed/x64-linux/debug/bin/liblibzippp.so \
                            $$PWD/../Common/vcpkg/installed/x64-linux/debug/bin/libssl.so \
                            $$PWD/../Common/vcpkg/installed/x64-linux/debug/bin/libz.so \
                            $$PWD/../Common/vcpkg/installed/x64-linux/debug/bin/libzip.so \

    } else {
        LIBS += -L$$PWD/../Common/vcpkg/installed/x64-linux/debug/lib/  -llibzippp -lzip -lbz2d -lz -lcrypto -lssl -ldl

        install_it.files += $$PWD/../Common/vcpkg/installed/x64-linux/bin/libbz2d.so \
                            $$PWD/../Common/vcpkg/installed/x64-linux/bin/libcrypto.so \
                            $$PWD/../Common/vcpkg/installed/x64-linux/bin/liblibzippp.so \
                            $$PWD/../Common/vcpkg/installed/x64-linux/bin/libssl.so \
                            $$PWD/../Common/vcpkg/installed/x64-linux/bin/libz.so \
                            $$PWD/../Common/vcpkg/installed/x64-linux/bin/libzip.so \
    }
}


INSTALLS += install_it install_assets

