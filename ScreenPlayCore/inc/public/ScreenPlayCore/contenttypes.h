// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#pragma once
#include <QObject>
#include <QQmlEngine>
#include <QtCore/qmetatype.h>

namespace ScreenPlay {

class ContentTypes : public QObject {
    Q_OBJECT
    QML_ELEMENT
    Q_CLASSINFO("RegisterEnumClassesUnscoped", "false")
    /*!
        \brief Used in the "Installed" tab.
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
        \brief This is a c++ representation
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
