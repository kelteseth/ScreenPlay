TEMPLATE = app
QT += qml quick av  sql widgets quickcontrols2 webkit
CONFIG += c++17
#DEFINES  += QT_NO_DEBUG_OUTPUT QT_NO_WARNING_OUTPUT

TARGETPATH = ScreenPlay

SOURCES += main.cpp \
    src/steamworkshop.cpp \
    src/installedlistmodel.cpp \
    src/monitorlistmodel.cpp \
    src/settings.cpp \
    src/wallpaper.cpp \
    src/profilelistmodel.cpp \
    src/profile.cpp \
    src/projectfile.cpp \
    src/widget.cpp \
    src/steamworkshoplistmodel.cpp \
    src/workshopitem.cpp \
    src/widgetbridge.cpp \
    src/installedlistfilter.cpp \
    src/sdkconnector.cpp \
    src/projectsettingslistitem.cpp \
    src/projectsettingslistmodel.cpp \
    src/globalnavigationhelper.cpp

RESOURCES += \
    Resources.qrc \

TRANSLATIONS = languages/ScreenPlay_en.ts  languages/ScreenPlay_de.ts

HEADERS += \
    src/steamworkshop.h \
    src/installedlistmodel.h \
    src/monitorlistmodel.h \
    src/settings.h \
    src/wallpaper.h \
    src/profilelistmodel.h \
    src/profile.h \
    src/projectfile.h \
    src/widget.h \
    src/steamworkshoplistmodel.h \
    src/workshopitem.h \
    src/widgetbridge.h \
    src/installedlistfilter.h \
    src/sdkconnector.h \
    src/projectsettingslistitem.h \
    src/projectsettingslistmodel.h \
    src/globalnavigationhelper.h

INCLUDEPATH += \
    $$PWD/ThirdParty/ \
    $$PWD/src/\


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
# QML_DESIGNER_IMPORT_PATH =
QT_QUICK_CONTROLS_STYLE = "Material"

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

win32 {
    INCLUDEPATH += "C:\msys64\mingw64\include"

    win32: LIBS += -L$$PWD/ThirdParty/Steam/redistributable_bin/win64/ -lsteam_api64
    DEPENDPATH += $$PWD/ThirdParty/Steam/redistributable_bin/win64
}
