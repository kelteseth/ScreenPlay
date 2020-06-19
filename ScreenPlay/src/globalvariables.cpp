#include "globalvariables.h"
namespace ScreenPlay {
ScreenPlay::GlobalVariables::GlobalVariables(QObject* parent)
    : QObject(parent)
{
    setLocalSettingsPath(QUrl { QStandardPaths::writableLocation(QStandardPaths::DataLocation) });
}
}
