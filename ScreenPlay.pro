TEMPLATE = subdirs
SUBDIRS =   \
    ScreenPlay/ScreenPlay.pro \
    ScreenPlaySDK/Screenplaysdk.pro \
    ScreenPlayWindow/ScreenPlayWindow.pro \
    ScreenPlayWidget

ScreenPlayWindow.depends = ScreenPlaySDK
