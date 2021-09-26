#include "screenplayworkshop_plugin.h"

void ScreenPlayWorkshopPlugin::registerTypes(const char* uri)
{
    qRegisterMetaType<ScreenPlayWorkshop::SteamQMLImageProvider*>("SteamQMLImageProvider*");
    qmlRegisterType<ScreenPlayWorkshop::SteamQMLImageProvider>("Workshop", 1, 0, "SteamImage");

    qmlRegisterUncreatableMetaObject(ScreenPlayWorkshopSteamEnums::staticMetaObject,
        "Workshop",
        1, 0,
        "SteamEnums",
        "Error: only enums");

    qRegisterMetaType<ScreenPlayWorkshop::SteamAccount*>("SteamAccount*");
    qRegisterMetaType<ScreenPlayWorkshop::InstalledListModel*>("InstalledListModel*");

    qmlRegisterType<ScreenPlayWorkshop::SteamWorkshop>("Workshop", 1, 0, "SteamWorkshop");
    qmlRegisterType<ScreenPlayWorkshop::Workshop>("Workshop", 1, 0, "ScreenPlayWorkshop");
}
