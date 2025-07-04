// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#include "ScreenPlayCore/contenttypes.h"

namespace ScreenPlay {
ContentTypes::ContentTypes(QObject* parent)
    : QObject(parent)
{
}

QString ContentTypes::toString(InstalledType type)
{
    switch (type) {
    case InstalledType::VideoWallpaper:
        return QObject::tr("Video Wallpaper");
    case InstalledType::QMLWallpaper:
        return QObject::tr("QML Wallpaper");
    case InstalledType::HTMLWallpaper:
        return QObject::tr("HTML Wallpaper");
    case InstalledType::GodotWallpaper:
        return QObject::tr("Godot Wallpaper");
    case InstalledType::GifWallpaper:
        return QObject::tr("GIF Wallpaper");
    case InstalledType::WebsiteWallpaper:
        return QObject::tr("Website Wallpaper");
    case InstalledType::QMLWidget:
        return QObject::tr("QML Widget");
    case InstalledType::HTMLWidget:
        return QObject::tr("HTML Widget");
    case InstalledType::Unknown:
    default:
        return QObject::tr("Unknown");
    }
}

Video::Video(QObject* parent)
    : QObject(parent)
{
}
}
