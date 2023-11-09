
#include <QGuiApplication>
#include <QObject>
#include <QStringList>
#include <QVector>
#include <QtGlobal>
#include <QtWebEngineQuick>

#include "ScreenPlayWallpaper/CMakeVariables.h"

#include "ScreenPlayUtil/exitcodes.h"
#include "ScreenPlayUtil/logginghandler.h"
#include "ScreenPlayUtil/util.h"

#if defined(Q_OS_WIN)
#include "src/winwindow.h"
Q_IMPORT_QML_PLUGIN(ScreenPlaySysInfoPlugin)
#elif defined(Q_OS_LINUX)
#include "src/linuxwaylandwindow.h"
#include "src/linuxx11window.h"
#elif defined(Q_OS_MACOS)
#include "src/macwindow.h"
#endif

Q_IMPORT_QML_PLUGIN(ScreenPlayWeatherPlugin)

int main(int argc, char* argv[])
{

    // Lets keep using it until: https://bugreports.qt.io/browse/QTBUG-109401
    QtWebEngineQuick::initialize();

#if defined(Q_OS_WIN)
    // Workaround for Qt 6.5.1 crash https://bugreports.qt.io/browse/QTBUG-113832
    qputenv("QT_DISABLE_HW_TEXTURES_CONVERSION", "1");
    qputenv("QT_MEDIA_BACKEND", "ffmpeg");
#endif

    QGuiApplication app(argc, argv);
    std::unique_ptr<const ScreenPlayUtil::LoggingHandler> logging;
    std::unique_ptr<BaseWindow> window;
    const auto platformName = QGuiApplication::platformName();

#if defined(Q_OS_WIN)
    window = std::make_unique<WinWindow>();
#elif defined(Q_OS_LINUX)
    if (platformName == "xcb") {
        window = std::make_unique<LinuxX11Window>();
    } else if (platformName == "wayland") {
        window = std::make_unique<LinuxWaylandWindow>();
    }
#elif defined(Q_OS_MACOS)
    window = std::make_unique<MacWindow>();
#endif

    // If we start with only one argument (app path)
    // It means we want to test a single wallpaper
    const QStringList argumentList = app.arguments();
    QString appID;

    // For testing purposes when starting the ScreenPlayWallpaper directly.
    if (argumentList.length() == 1) {
        QString exampleContentPath = QString(SCREENPLAY_SOURCE_DIR) + "/Content";
        QStringList contentFolder = {
            "/wallpaper_html", // 0
            "/wallpaper_qml", // 1
            "/wallpaper_qml_landscape", // 2
            "/wallpaper_qml_particles", // 3
            "/wallpaper_video_astronaut_vp9", // 4
            "/wallpaper_video_nebula_h264" // 5
        };
        const int index = 5;
        QString projectPath = exampleContentPath + contentFolder.at(index);

        window->setActiveScreensList({ 0 });
        window->setProjectPath(projectPath);
        window->setAppID("test");
        window->setVolume(1);
        window->setFillMode("cover");
        window->setType(ScreenPlay::InstalledType::InstalledType::VideoWallpaper);
        window->setCheckWallpaperVisible(true);
        window->setDebugMode(false);
    } else {
        // 8 parameter + 1 OS working directory as the first default paramter
        if (argumentList.length() != 9) {
            return static_cast<int>(ScreenPlay::WallpaperExitCode::Invalid_ArgumentSize);
        }

        const auto activeScreensList = ScreenPlayUtil::parseStringToIntegerList(argumentList.at(1));

        if (!activeScreensList.has_value()) {
            qCritical("Could not activeScreensList");
            return static_cast<int>(ScreenPlay::WallpaperExitCode::Invalid_ActiveScreensList);
        }

        auto installedType = ScreenPlay::InstalledType::InstalledType::Unknown;
        if (auto typeOpt = ScreenPlayUtil::getInstalledTypeFromString(argumentList.at(6))) {
            installedType = typeOpt.value();
        } else {
            qCritical() << "Cannot parse Wallpaper type from value" << argumentList.at(6);
            return static_cast<int>(ScreenPlay::WallpaperExitCode::Invalid_InstalledType);
        }

        bool okParseCheckWallpaperVisible = false;
        const bool checkWallpaperVisible = argumentList.at(7).toInt(&okParseCheckWallpaperVisible);
        if (!okParseCheckWallpaperVisible) {
            qCritical("Could not parse checkWallpaperVisible");
            return static_cast<int>(ScreenPlay::WallpaperExitCode::Invalid_CheckWallpaperVisible);
        }
        bool okParseVolume = 0.0f;
        const float volume = argumentList.at(4).toFloat(&okParseVolume);
        if (!okParseVolume) {
            qCritical("Could not parse Volume");
            return static_cast<int>(ScreenPlay::WallpaperExitCode::Invalid_Volume);
        }

        appID = argumentList.at(3);
        if (!appID.startsWith("appID=")) {
            qCritical("Invalid appID");
            return static_cast<int>(ScreenPlay::WallpaperExitCode::Invalid_AppID);
        }
        appID = appID.remove("appID=");

        window->setActiveScreensList(activeScreensList.value());
        window->setProjectPath(argumentList.at(2));
        window->setAppID(appID);
        window->setVolume(volume);
        window->setFillMode(argumentList.at(5));
        window->setType(installedType);
        window->setCheckWallpaperVisible(checkWallpaperVisible);
        window->setDebugMode(false);
    }

    const auto setupStatus = window->setup();
    if (setupStatus != ScreenPlay::WallpaperExitCode::Ok) {
        return static_cast<int>(setupStatus);
    }
    const auto startStatus = window->start();
    if (startStatus != ScreenPlay::WallpaperExitCode::Ok) {
        return static_cast<int>(startStatus);
    }
    emit window->qmlStart();
    logging = std::make_unique<const ScreenPlayUtil::LoggingHandler>("ScreenPlayWallpaper_" + appID);
    const int status = app.exec();
    logging.reset();
    return status;
}
