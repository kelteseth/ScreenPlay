QT += quick qml quickcontrols2 core widgets gui
CONFIG += c++17
CONFIG += qtquickcompiler
# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    SPWidgetmain.cpp \
    src/spwidgetmainwindow.cpp

RESOURCES += \
    SPWidgetResources.qrc

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


# Additional import path used to resolve QML modules just for Qt Quick Designer
# QML_DESIGNER_IMPORT_PATH =
QT_QUICK_CONTROLS_STYLE = "Material"

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    src/spwidgetmainwindow.h
