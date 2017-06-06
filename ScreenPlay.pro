TEMPLATE = app

QT += qml quick av widgets quickcontrols2
CONFIG += c++11



SOURCES += main.cpp \
    src/screenplay.cpp \
    src/steamworkshop.cpp \
    src/installedlistmodel.cpp \
    src/backend.cpp \
    src/monitorlistmodel.cpp \
    src/settings.cpp \
    src/packagefilehandler.cpp \
    src/wallpaper.cpp \
    src/profilelistmodel.cpp \
    src/profile.cpp

RESOURCES += qml.qrc

HEADERS += \
    src/screenplay.h \
    src/steamworkshop.h \
    src/installedlistmodel.h \
    src/backend.h \
    src/monitorlistmodel.h \
    src/settings.h \
    src/packagefilehandler.h \
    src/wallpaper.h \
    src/profilelistmodel.h \
    src/profile.h

INCLUDEPATH += \
    $$PWD/ThirdParty/ \
    $$PWD/src/\

INCLUDEPATH += .

CONFIG(debug, debug|release) {
    install_it.path = $${OUT_PWD}/debug/
 } else {
    install_it.path = $${OUT_PWD}/release/
 }


install_it.files += assets/templates/config.json \
                    assets/icons/favicon.ico \
                    steam_appid.txt \
                    settings.json \

INSTALLS += install_it


# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH += [QtAVSourceCodeDir]/qml

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

#contains(QT_ARCH, i386) {
#    #32-bit
#    win32: LIBS += -L$$PWD/ThirdParty/Steam/redistributable_bin/ -lsteam_api
#    DEPENDPATH += $$PWD/ThirdParty/Steam/redistributable_bin/
#} else {
#    #64-bit
#    win32: LIBS += -L$$PWD/ThirdParty/Steam/redistributable_bin/win64/ -lsteam_api64
#    DEPENDPATH += $$PWD/ThirdParty/Steam/redistributable_bin/win64
#}
