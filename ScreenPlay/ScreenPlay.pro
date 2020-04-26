TEMPLATE = app
QT += qml quick widgets quickcontrols2 core webengine
CONFIG += c++17

TARGETPATH = ScreenPlay

include($$PWD/../Common/qt-google-analytics/qt-google-analytics.pri)

!unix {
    include($$PWD/../Common/qt-breakpad/qt-breakpad.pri)
}

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
    $$PWD/src/globalvariables.cpp \
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

win32: ARCH_OS = x64-windows
unix:!macx { ARCH_OS = x64-linux}
macx: ARCH_OS = x64-osx

CONFIG(debug, debug|release){
    ARCH_OS_BUILD = $$ARCH_OS/debug
} else {
    ARCH_OS_BUILD = $$ARCH_OS
}

INCLUDEPATH += $$PWD/../Common/vcpkg/installed/$$ARCH_OS/include
DEPENDPATH  += $$PWD/../Common/vcpkg/installed/$$ARCH_OS/include


win32 {
    RC_ICONS += favicon.ico
    CONFIG(debug, debug|release) {
        install_it.path = $${OUT_PWD}/debug/
    } else {
        install_it.path = $${OUT_PWD}/release/
    }


    LIBS += -luser32
	LIBS += -L$$PWD/../Common/vcpkg/installed/x64-windows/lib/ -llibzippp


    install_it.files += $$PWD/../Common/vcpkg/installed/$$ARCH_OS_BUILD/bin/zip.dll \
	                    $$PWD/../Common/vcpkg/installed/$$ARCH_OS_BUILD/bin/zlibd1.dll \
						$$PWD/../Common/vcpkg/installed/$$ARCH_OS_BUILD/bin/libzippp.dll \
						$$PWD/../Common/vcpkg/installed/$$ARCH_OS_BUILD/bin/bz2d.dll \
						$$PWD/../Common/vcpkg/installed/$$ARCH_OS_BUILD/bin/libcrypto-1_1-x64.dll \
						$$PWD/../Common/vcpkg/installed/$$ARCH_OS_BUILD/bin/libssl-1_1-x64.dll \

}

unix {

    CONFIG(debug, debug|release){
        #lbz2d uses d
        LIBS += -L$$PWD/../Common/vcpkg/installed/$$ARCH_OS_BUILD/lib/  -llibzippp -lzip -lbz2d -lz -lcrypto -lssl -ldl
    } else {
        LIBS += -L$$PWD/../Common/vcpkg/installed/$$ARCH_OS_BUILD/lib/  -llibzippp -lzip -lbz2 -lz -lcrypto -lssl -ldl
    }


}


INSTALLS += install_it install_assets

