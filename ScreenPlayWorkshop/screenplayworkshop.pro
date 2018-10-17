TEMPLATE = lib
TARGET = ScreenPlayWorkshop
QT += qml quick
CONFIG += plugin c++11

TARGET = $$qtLibraryTarget($$TARGET)
uri = net.aimber.workshop

# Input
SOURCES += \
        screenplayworkshop_plugin.cpp \
        workshop.cpp

HEADERS += \
        screenplayworkshop_plugin.h \
        workshop.h

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


win32 {
    win32: LIBS += -L$$PWD/ThirdParty/Steam/redistributable_bin/win64/ -lsteam_api64
    DEPENDPATH += $$PWD/ThirdParty/Steam/redistributable_bin/win64
}

unix {
    LIBS += -L$$PWD/ThirdParty/steam/redistributable_bin/linux64/ -lsteam_api
    DEPENDPATH += $$PWD/ThirdParty/steam/redistributable_bin/linux64

    INCLUDEPATH += $$PWD/ThirdParty/steam/lib/linux64
    DEPENDPATH += $$PWD/ThirdParty/steam/lib/linux64s

    LIBS += -L$$PWD/ThirdParty/steam/lib/linux64/ -lsdkencryptedappticket
}

