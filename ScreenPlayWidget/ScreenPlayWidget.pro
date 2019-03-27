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

INCLUDEPATH += \
    $$PWD/../../ThirdParty/ \
    $$PWD/../../src/ \

include(../ScreenPlaySDK/Screenplaysdk.pri)

macx: {
    QMAKE_LIBDIR += $$OUT_PWD/
    install_it.path = $${OUT_PWD}/../ScreenPlaySDK
}

!macx: {
    CONFIG(debug, debug|release) {
    LIBS += -lScreenplaysdkd
        install_it.path = $${OUT_PWD}/debug/
        QMAKE_LIBDIR += $$OUT_PWD/../ScreenPlaySDK/debug
     } else {
    LIBS += -lScreenplaysdk
        install_it.path = $${OUT_PWD}/release/
        QMAKE_LIBDIR += $$OUT_PWD/../ScreenPlaySDK/release
     }
    QMAKE_LIBDIR += $$OUT_PWD/../ScreenPlaySDK
}
