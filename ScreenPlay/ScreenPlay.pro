TEMPLATE = app
QT += qml quick widgets quickcontrols2 core webengine
CONFIG += c++17
#CONFIG += qtquickcompiler
#DEFINES  += QT_NO_DEBUG_OUTPUT QT_NO_WARNING_OUTPUT

TARGETPATH = ScreenPlay

ICON = favicon.ico

DEFINES += QT_DEPRECATED_WARNINGS

DISTFILES += \
    favicon.ico \

RESOURCES += Resources.qrc

SOURCES += main.cpp \
    src/createimportvideo.cpp \
    src/installedlistmodel.cpp \
    src/monitorlistmodel.cpp \
    src/screenplaywallpaper.cpp \
    src/screenplaywidget.cpp \
    src/settings.cpp \
    src/profilelistmodel.cpp \
    src/installedlistfilter.cpp \
    src/sdkconnector.cpp \
    src/projectsettingslistmodel.cpp \
    src/screenplaymanager.cpp \
    src/qmlutilities.cpp \
    src/create.cpp



TRANSLATIONS =  translations/ScreenPlay_en.ts   \
                translations/ScreenPlay_de.ts

HEADERS += \
    src/createimportvideo.h \
    src/installedlistmodel.h \
    src/monitorlistmodel.h \
    src/screenplaywallpaper.h \
    src/screenplaywidget.h \
    src/settings.h \
    src/profilelistmodel.h \
    src/profile.h \
    src/projectfile.h \
    src/installedlistfilter.h \
    src/sdkconnector.h \
    src/projectsettingslistitem.h \
    src/projectsettingslistmodel.h \
    src/screenplaymanager.h \
    src/qmlutilities.h \
    src/create.h

INCLUDEPATH += \
    $$PWD/ThirdParty/ \
    $$PWD/src/

CONFIG(debug, debug|release) {
    install_it.path = $${OUT_PWD}/debug/
} else {
    install_it.path = $${OUT_PWD}/release/
}

install_it.files += assets/templates/config.json \
                    assets/icons/favicon.ico

INCLUDEPATH += $$PWD/../Common/vcpkg/installed/x64-windows/include
DEPENDPATH += $$PWD/../Common/vcpkg/installed/x64-windows/include

win32 {
    RC_ICONS += favicon.ico


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

