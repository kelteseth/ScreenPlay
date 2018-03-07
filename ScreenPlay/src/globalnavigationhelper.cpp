#include "globalnavigationhelper.h"

GlobalNavigationHelper::GlobalNavigationHelper(QObject *parent) : QObject(parent)
{

}

void GlobalNavigationHelper::setNavigation(QString nav)
{
    emit requestNavigation(nav);
}

void GlobalNavigationHelper::setToggleWallpaperConfiguration()
{
    emit requestToggleWallpaperConfiguration();
}
