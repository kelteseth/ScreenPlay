TEMPLATE = subdirs
SUBDIRS =   \
    ScreenPlay/ScreenPlay.pro \
    ScreenPlaySDK/Screenplaysdk.pro \
    ScreenPlayWindow/ScreenPlayWindow.pro \
    ScreenPlay/ThirdParty/qt-google-analytics/qt-google-analytics.pro \
    ScreenPlay/ThirdParty/stomt-qt-sdk/sdk/stomt-qt-sdk.pro \
    ScreenPlayWidget/ScreenPlayWidget.pro

ScreenPlayWindow.depends = ScreenPlaySDK
ScreenPlayWidget.depends = ScreenPlaySDK
ScreenPlay.depends = qt-google-analytics
