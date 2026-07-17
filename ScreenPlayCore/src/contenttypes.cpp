// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#include "ScreenPlayCore/contenttypes.h"

#include <QHash>

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

ContentTypes::ExampleContent ContentTypes::exampleContentFromFolderName(const QString& folderName)
{
    static const QHash<QString, ExampleContent> map {
        { "wallpaper_video_astronaut_vp9", ExampleContent::VideoAstronautVP9 },
        { "wallpaper_video_nebula_h264", ExampleContent::VideoNebulaH264 },
        { "wallpaper_video_shapes_av1", ExampleContent::VideoShapesAV1 },
        { "wallpaper_html", ExampleContent::Html },
        { "wallpaper_qml", ExampleContent::Qml },
        { "wallpaper_qml_particles", ExampleContent::QmlParticles },
        { "wallpaper_godot", ExampleContent::Godot },
        { "widget_analog_clock", ExampleContent::WidgetAnalogClock },
        { "widget_digital_clock", ExampleContent::WidgetDigitalClock },
        { "widget_hello_world", ExampleContent::WidgetHelloWorld },
        { "widget_rss_guardian", ExampleContent::WidgetRssGuardian },
        { "widget_rss_hackernews", ExampleContent::WidgetRssHackernews },
        { "widget_system_stats", ExampleContent::WidgetSystemStats },
        { "widget_weather", ExampleContent::WidgetWeather },
        { "widget_xkcd", ExampleContent::WidgetXkcd },
        { "widget_year_countdown", ExampleContent::WidgetYearCountdown },
    };
    return map.value(folderName, ExampleContent::ExampleOther);
}

Video::Video(QObject* parent)
    : QObject(parent)
{
}

Godot::Godot(QObject* parent)
    : QObject(parent)
{
}
}
