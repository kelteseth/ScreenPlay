TEMPLATE = subdirs
SUBDIRS =   \
    ScreenPlay/ScreenPlay.pro \
    ScreenPlaySDK/ScreenPlaySDK.pro \
    ScreenPlaySysInfo/ScreenPlaySysInfo.pro \
    ScreenPlayWindow/ScreenPlayWindow.pro \
    ScreenPlayWidget/ScreenPlayWidget.pro \
    ScreenPlay/ThirdParty/stomt-qt-sdk/sdk/stomt-qt-sdk.pro \

ScreenPlayWindow.depends = ScreenPlaySDK
ScreenPlayWidget.depends = ScreenPlaySDK

