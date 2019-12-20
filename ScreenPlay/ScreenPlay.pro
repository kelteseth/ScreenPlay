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
    $$PWD/translations/ScreenPlay_fr.ts

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
    install_it.path = $${OUT_PWD}/debug/
} else {
    install_it.path = $${OUT_PWD}/release/
}



win32 {
    RC_ICONS += favicon.ico

    INCLUDEPATH += $$PWD/../Common/vcpkg/installed/x64-windows/include
    DEPENDPATH += $$PWD/../Common/vcpkg/installed/x64-windows/include

    LIBS += -L$$PWD/../Common/vcpkg/installed/x64-windows/lib/ -llibzippp
CONFIG(debug, debug|release) {
    install_it.files += $$PWD/../Common/vcpkg/installed/x64-windows/debug/bin/zip.dll \
                        $$PWD/../Common/vcpkg/installed/x64-windows/debug/bin/zlibd1.dll \
                        $$PWD/../Common/vcpkg/installed/x64-windows/debug/bin/libzippp.dll \
                        $$PWD/../Common/vcpkg/installed/x64-windows/debug/bin/bz2d.dll \

} else {
    install_it.files += $$PWD/../Common/vcpkg/installed/x64-windows/bin/zip.dll \
                        $$PWD/../Common/vcpkg/installed/x64-windows/bin/zlib1.dll \
                        $$PWD/../Common/vcpkg/installed/x64-windows/bin/libzippp.dll \
                        $$PWD/../Common/vcpkg/installed/x64-windows/bin/bz2.dll \
}



    LIBS += -luser32
    install_it.files += \
                        $$PWD/ThirdParty/OpenSSL/libcrypto-1_1-x64.dll \
                        $$PWD/ThirdParty/OpenSSL/libssl-1_1-x64.dll \
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

INSTALLS += install_it

