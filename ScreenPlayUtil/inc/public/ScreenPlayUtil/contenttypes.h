// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#pragma once
#include <QObject>
#include <QQmlEngine>
#include <QtCore/qmetatype.h>

// We must package everything into a class for
// qml to be able to have typed enums. Making
// qml enums unscoped as default was a mistake.
namespace ScreenPlay {

class ContentTypes : public QObject {
    Q_OBJECT
    QML_ELEMENT
    Q_CLASSINFO("RegisterEnumClassesUnscoped", "false")
    /*!
        \brief Global enum for search types. Used in the "Installed" tab.
    */
public:
    ContentTypes(QObject* parent = nullptr);
    enum class SearchType {
        All,
        Text,
        Scene, // QML, HTML, Godot, Gif, Website wallpaper
        Wallpaper,
        Widget,
    };
    Q_ENUM(SearchType)
    /*!
        \brief When changing the enum, one also needs to change:
             GlobalVariables::getAvailableWallpaper
             GlobalVariables::getAvailableWidgets
             Common/Util.js isWallpaper() and isWidget()
             ScreenPlayWallpaper: BaseWindow::parseWallpaperType()
    */
    enum class InstalledType {
        Unknown,
        // Wallpaper
        VideoWallpaper,
        QMLWallpaper,
        HTMLWallpaper,
        GodotWallpaper,
        GifWallpaper,
        WebsiteWallpaper,
        // Widgets
        QMLWidget,
        HTMLWidget,
    };
    Q_ENUM(InstalledType)
};

class Video : public QObject {
    Q_OBJECT
    QML_ELEMENT
    Q_CLASSINFO("RegisterEnumClassesUnscoped", "false")
public:
    Video(QObject* parent = nullptr);
    /*!
        \brief Global enum for fill mode. This is a c++ representation
               of HTML fill modes. We use "_" instead of "-" for scale down,
               because c++ forbids "-" in enum names.
    */
    enum class FillMode {
        Stretch,
        Fill,
        Contain,
        Cover,
        Scale_Down
    };
    Q_ENUM(FillMode)

    /*!
        \brief When changing the enum, one also needs to change:
             GlobalVariables::getAvailableWallpaper
             GlobalVariables::getAvailableWidgets
             Common/Util.js isWallpaper() and isWidget()
             ScreenPlayWallpaper: BaseWindow::parseWallpaperType()
    */

    enum class VideoCodec {
        Unknown,
        VP8,
        VP9,
        AV1,
        H264,
        H265
    };
    Q_ENUM(VideoCodec)
};
}
