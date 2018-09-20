TEMPLATE = app
QT += qml quick quickcontrols2 widgets core webengine
CONFIG += c++17
CONFIG += qtquickcompiler
msvc: LIBS += -luser32
TARGETPATH = ScreenPlayWindow

SOURCES += \
    SPWmain.cpp \
    src/SPWmainwindow.cpp

HEADERS += \
    src/SPWmainwindow.h

RESOURCES += \
    SPWResources.qrc


INCLUDEPATH += \
    $$PWD/../../ThirdParty/ \
    $$PWD/../../src/ \

include(../ScreenPlaySDK/Screenplaysdk.pri)


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

install_it.files += index.html \

INSTALLS += install_it

DISTFILES += \
    index.html

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =
QT_QUICK_CONTROLS_STYLE = "Material"

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


