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

    /*!
        \brief Identifies content shipped as an example next to the executable.
               UserInstalled is everything the user imported/subscribed; every
               other value maps to exactly one folder under Content/ so tests
               and the UI can select a specific example type-safely instead of
               matching magic folder-name strings. ExampleOther covers example
               folders not (yet) enumerated here so they stay protected.
               When adding a folder to Content/, add a matching value here and
               a case in exampleContentFromFolderName().
    */
    enum class ExampleContent {
        UserInstalled = 0,
        VideoAstronautVP9,
        VideoNebulaH264,
        VideoShapesAV1,
        Html,
        Qml,
        QmlParticles,
        Godot,
        WidgetAnalogClock,
        WidgetDigitalClock,
        WidgetHelloWorld,
        WidgetRssGuardian,
        WidgetRssHackernews,
        WidgetSystemStats,
        WidgetWeather,
        WidgetXkcd,
        WidgetYearCountdown,
        ExampleOther,
    };
    Q_ENUM(ExampleContent)

public:
    // Static utility function to convert content type enum to user-friendly translatable string
    static QString toString(ScreenPlay::ContentTypes::InstalledType type);
    // Map a Content/ subfolder name to its ExampleContent value. Returns
    // ExampleOther for an unmapped example folder (still treated as example).
    static ScreenPlay::ContentTypes::ExampleContent exampleContentFromFolderName(const QString& folderName);
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
        NoConversion,
        VP8,
        VP9,
        AV1,
        H264,
        H265,
        MJPEG
    };
    Q_ENUM(VideoCodec)
};

class Godot : public QObject {
    Q_OBJECT
    QML_ELEMENT
    Q_CLASSINFO("RegisterEnumClassesUnscoped", "false")
public:
    Godot(QObject* parent = nullptr);

    enum class Fps {
        Fps1,
        Fps6,
        Fps12,
        Fps24,
        Fps30,
        Fps60,
        Fps120,
        Fps144,
        Unlimited,
        Vsync
    };
    Q_ENUM(Fps)

    enum class ScaleMode3D {
        Bilinear,
        FSR1_0,
        FSR2_2
    };
    Q_ENUM(ScaleMode3D)

    enum class RenderingDriver {
        Vulkan,
        D3D12,
        OpenGL3,
        OpenGL3_Angle
    };
    Q_ENUM(RenderingDriver)
};
}
