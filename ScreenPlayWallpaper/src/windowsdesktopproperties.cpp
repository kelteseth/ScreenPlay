#include "windowsdesktopproperties.h"

/*!
    \class WindowsDesktopProperties
    \inmodule ScreenPlayWallpaper
    \brief  .
*/

WindowsDesktopProperties::WindowsDesktopProperties(QObject* parent)
    : QObject(parent)
{

    QSettings settings("HKEY_CURRENT_USER\\Control Panel\\Desktop", QSettings::NativeFormat);

    setWallpaperPath(settings.value("WallPaper").toString());
    setIsTiled(settings.value("TileWallpaper").toBool());

    QPoint pos;
    pos.setX(settings.value("WallpaperOriginX").toInt());
    pos.setY(settings.value("WallpaperOriginY").toInt());
    setPosition(pos);

    setWallpaperStyle(settings.value("WallpaperStyle").toInt());

    QSettings settingscolor("HKEY_CURRENT_USER\\Control Panel\\Colors", QSettings::NativeFormat);

    QString colorStringRGB = settingscolor.value("Background").toString();
    QStringList colorStringRGBList = colorStringRGB.split(" ");

    if (colorStringRGBList.length() == 3) {
        int colorR = colorStringRGBList.at(0).toInt();
        int colorG = colorStringRGBList.at(1).toInt();
        int colorB = colorStringRGBList.at(2).toInt();
        setColor(QColor::fromRgb(colorR, colorG, colorB));
    } else {
        setColor(QColor::fromRgb(0, 0, 0));
    }

    QSettings settingsWindowsVersion("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", QSettings::NativeFormat);

    // Only Windows > 10 has this variable
    if (!settingsWindowsVersion.contains("ReleaseID"))
        return;

    bool canParse { false };
    int value = settingsWindowsVersion.value("ReleaseId").toInt(&canParse);

    if (canParse)
        setWindowsVersion(value);
}
