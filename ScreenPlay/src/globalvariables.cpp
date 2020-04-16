#include "globalvariables.h"
namespace ScreenPlay {
ScreenPlay::GlobalVariables::GlobalVariables(QObject* parent)
    : QObject(parent)
{
    qRegisterMetaType<GlobalVariables::FillMode>("FillMode");
    qRegisterMetaType<GlobalVariables::WallpaperType>("WallpaperType");
    qmlRegisterUncreatableType<GlobalVariables>("ScreenPlay.GlobalVariables", 1, 0, "GlobalVariables", "Error only for enums");
    setLocalSettingsPath(QUrl { QStandardPaths::writableLocation(QStandardPaths::DataLocation) });
}
}
