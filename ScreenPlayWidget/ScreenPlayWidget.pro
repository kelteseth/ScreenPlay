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
