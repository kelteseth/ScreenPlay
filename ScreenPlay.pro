TEMPLATE = subdirs
SUBDIRS +=   \
    ScreenPlay/ScreenPlay.pro \
    ScreenPlaySDK/ScreenPlaySDK.pro \
    ScreenPlayWallpaper/ScreenPlayWallpaper.pro \
    ScreenPlayWidget/ScreenPlayWidget.pro \
    ScreenPlay/ThirdParty/stomt-qt-sdk/sdk/stomt-qt-sdk.pro \

ScreenPlayWallpaper.depends = ScreenPlaySDK
ScreenPlayWidget.depends = ScreenPlaySDK


win32 {
SUBDIRS +=   \
    ScreenPlaySysInfo/ScreenPlaySysInfo.pro \
}

