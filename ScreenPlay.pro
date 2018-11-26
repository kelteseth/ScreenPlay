TEMPLATE = subdirs
SUBDIRS =   \
    ScreenPlay/ScreenPlay.pro \
    ScreenPlaySDK/ScreenPlaySDK.pro \
    ScreenPlaySysInfo/ScreenPlaySysInfo.pro \
    ScreenPlayWindow/ScreenPlayWindow.pro \
    ScreenPlay/ThirdParty/stomt-qt-sdk/sdk/stomt-qt-sdk.pro \
    ScreenPlayWidget/ScreenPlayWidget.pro \
#ScreenPlay/ThirdParty/qt-google-analytics/qt-google-analytics.pro \

ScreenPlayWindow.depends = ScreenPlaySDK
ScreenPlayWidget.depends = ScreenPlaySDK
#ScreenPlay.depends = qt-google-analytics

