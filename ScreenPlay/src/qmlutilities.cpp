#include "qmlutilities.h"

QMLUtilities::QMLUtilities(QObject *parent) : QObject(parent)
{

}
void QMLUtilities::setNavigation(QString nav)
{
    emit requestNavigation(nav);
}

void QMLUtilities::setToggleWallpaperConfiguration()
{
    emit requestToggleWallpaperConfiguration();
}
