TEMPLATE = lib
TARGET = ScreenPlaySDK
TARGET = $$qtLibraryTarget($$TARGET)

QT += qml quick
CONFIG += plugin c++11

uri = app.screenplay.screenplaysdk

# Input
SOURCES += \
    $$PWD/screenplay-sdk_plugin.cpp \
    $$PWD/screenplaysdk.cpp

HEADERS += \
    $$PWD/screenplay-sdk_plugin.h \
    $$PWD/screenplaysdk.h

DISTFILES = qmldir

!equals(_PRO_FILE_PWD_, $$OUT_PWD) {
    copy_qmldir.target = $$OUT_PWD/qmldir
    copy_qmldir.depends = $$_PRO_FILE_PWD_/qmldir
    copy_qmldir.commands = $(COPY_FILE) \"$$replace(copy_qmldir.depends, /, $$QMAKE_DIR_SEP)\" \"$$replace(copy_qmldir.target, /, $$QMAKE_DIR_SEP)\"
    QMAKE_EXTRA_TARGETS += copy_qmldir
    PRE_TARGETDEPS += $$copy_qmldir.target
}

qmldir.files = qmldir

installPath = $$[QT_INSTALL_QML]/$$replace(uri, \\., /)
qmldir.path = $$installPath
target.path = $$installPath
INSTALLS += target qmldir

