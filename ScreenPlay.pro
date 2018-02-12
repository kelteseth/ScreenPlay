TEMPLATE = subdirs
SUBDIRS =   \
            ScreenPlay/ScreenPlay.pro \
            ScreenPlayWindow/ScreenPlayWindow.pro \
            ScreenPlaySDK/Screenplaysdk.pro

ScreenPlayWindow.depends = ScreenPlaySDK
