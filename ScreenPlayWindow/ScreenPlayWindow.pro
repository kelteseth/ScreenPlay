TEMPLATE = app
QT += qml quick av widgets quickcontrols2 widgets
CONFIG += c++17


TARGETPATH = ScreenPlayWindow

SOURCES += \
        main.cpp \
        src/mainwindow.cpp

HEADERS += \
        src/mainwindow.h

RESOURCES += \
        Resources.qrc \


INCLUDEPATH += \
    $$PWD/../../ThirdParty/ \
    $$PWD/../../src/\


CONFIG(debug, debug|release) {
    install_it.path = $${OUT_PWD}/debug/
 } else {
    install_it.path = $${OUT_PWD}/release/
 }


# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH += [QtAVSourceCodeDir]/qml
QML_IMPORT_PATH += "C:\msys64\mingw64\share\qt5\qml"

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

win32 {
    INCLUDEPATH += "C:\msys64\mingw64\include"
}

win32:CONFIG(release, debug|release): LIBS += -L$${OUT_PWD}/../ScreenPlaySDK/release -llibScreenPlaySDK.dll
else:win32:CONFIG(debug, debug|release): LIBS += -L$${OUT_PWD}/../ScreenPlaySDK/debug -llibScreenPlaySDKd.dll
