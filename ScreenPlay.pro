TEMPLATE = subdirs
SUBDIRS =   \
    ScreenPlay/ScreenPlay.pro \
    ScreenPlaySDK/Screenplaysdk.pro \
    ScreenPlayWindow/ScreenPlayWindow.pro \
    ScreenPlayWidget/ScreenPlayWidget.pro

ScreenPlayWindow.depends = ScreenPlaySDK
ScreenPlayWidget.depends = ScreenPlaySDK
