TEMPLATE = app
QT += qml quick quickcontrols2 widgets core webengine
CONFIG += c++17

DEFINES += QT_DEPRECATED_WARNINGS

TARGETPATH = ScreenPlayWallpaper


RESOURCES += \
    SPWResources.qrc


SOURCES += \
    main.cpp \
    src/basewindow.cpp \

HEADERS += \
    src/basewindow.h \

unix{
    SOURCES += \
        src/linuxwindow.cpp

    HEADERS += \
        src/linuxwindow.h
}

win32 {
    LIBS += -luser32
    SOURCES += \
        src/windowsdesktopproperties.cpp \
        src/winwindow.cpp

    HEADERS += \
        src/windowsdesktopproperties.h \
        src/winwindow.h

    CONFIG(debug, debug|release) {
        install_it.path   = $${OUT_PWD}/debug/
        install_it.files +=  \
                            $$PWD/../Common/vcpkg/installed/x64-windows/debug/bin/libcrypto-1_1-x64.dll \
                            $$PWD/../Common/vcpkg/installed/x64-windows/debug/bin/libssl-1_1-x64.dll \

    } else {
        install_it.path   = $${OUT_PWD}/release/
        install_it.files +=  \
                            $$PWD/../Common/vcpkg/installed/x64-windows/bin/libcrypto-1_1-x64.dll \
                            $$PWD/../Common/vcpkg/installed/x64-windows/bin/libssl-1_1-x64.dll \
    }
}

include(../ScreenPlaySDK/ScreenPlaySDK.pri)

macx {
    QMAKE_LIBDIR += $$OUT_PWD/
    install_it.path = $${OUT_PWD}/../ScreenPlaySDK

    html_data.files = index.html
    html_data.path = Contents/MacOS
    QMAKE_BUNDLE_DATA += html_data

    QMAKE_LFLAGS += -framework Cocoa
    SOURCES +=  src/macintegration.cpp
    HEADERS +=  src/macintegration.h \
                src/macbridge.h
    OBJECTIVE_SOURCES += src/MacBridge.mm

    SOURCES += \
        src/macwindow.cpp

    HEADERS += \
        src/macwindow.h
}

!macx {
    CONFIG(debug, debug|release) {
        LIBS += -lScreenPlaySDKd
        QMAKE_LIBDIR += $$OUT_PWD/../ScreenPlaySDK/debug
     } else {
        LIBS += -lScreenPlaySDK
        QMAKE_LIBDIR += $$OUT_PWD/../ScreenPlaySDK/release
     }
    QMAKE_LIBDIR += $$OUT_PWD/../ScreenPlaySDK
}

install_it.files += index.html \

INSTALLS += install_it
DISTFILES += \
    index.html
