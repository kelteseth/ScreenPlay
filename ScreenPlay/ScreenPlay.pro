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
    src/settings.cpp \
    src/profilelistmodel.cpp \
    src/profile.cpp \
    src/projectfile.cpp \
    src/installedlistfilter.cpp \
    src/sdkconnector.cpp \
    src/projectsettingslistitem.cpp \
    src/projectsettingslistmodel.cpp \
    src/screenplay.cpp \
    src/qmlutilities.cpp \
    src/create.cpp


TRANSLATIONS =  translations/ScreenPlay_en.ts   \
                translations/ScreenPlay_de.ts

HEADERS += \
    src/createimportvideo.h \
    src/installedlistmodel.h \
    src/monitorlistmodel.h \
    src/settings.h \
    src/profilelistmodel.h \
    src/profile.h \
    src/projectfile.h \
    src/installedlistfilter.h \
    src/sdkconnector.h \
    src/projectsettingslistitem.h \
    src/projectsettingslistmodel.h \
    src/screenplay.h \
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

win32 {
    LIBS += -luser32
    install_it.files += \
                        Common/vcpkg/packages/openssl-windows_x64-windows/bin/libeay32.dll \
                        Common/vcpkg/packages/openssl-windows_x64-windows/bin/ssleay32.dll \
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

