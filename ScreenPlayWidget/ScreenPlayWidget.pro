QT += quick qml quickcontrols2 core widgets gui
CONFIG += c++17
CONFIG += qtquickcompiler
DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    main.cpp \
    src/widgetwindow.cpp

HEADERS += \
    src/widgetwindow.h

RESOURCES += \
    SPWidgetResources.qrc

include(../ScreenPlaySDK/ScreenPlaySDK.pri)



win32 {

    CONFIG(debug, debug|release) {
        install_it.path = $${OUT_PWD}/debug/
        install_it.files +=  \
                            $$PWD/../Common/vcpkg/installed/x64-windows/debug/bin/libcrypto-1_1-x64.dll \
                            $$PWD/../Common/vcpkg/installed/x64-windows/debug/bin/libssl-1_1-x64.dll \

    } else {
        install_it.path = $${OUT_PWD}/release/
        install_it.files +=  \
                            $$PWD/../Common/vcpkg/installed/x64-windows/bin/libcrypto-1_1-x64.dll \
                            $$PWD/../Common/vcpkg/installed/x64-windows/bin/libssl-1_1-x64.dll \
    }

}


macx: {
    QMAKE_LIBDIR += $$OUT_PWD/
}

!macx: {
    CONFIG(debug, debug|release) {
    LIBS += -lScreenPlaySDKd
        QMAKE_LIBDIR += $$OUT_PWD/../ScreenPlaySDK/debug
     } else {
    LIBS += -lScreenPlaySDK
        QMAKE_LIBDIR += $$OUT_PWD/../ScreenPlaySDK/release
     }
    QMAKE_LIBDIR += $$OUT_PWD/../ScreenPlaySDK
}

INSTALLS += install_it
