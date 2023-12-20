
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
Q_IMPORT_QML_PLUGIN(ScreenPlayUtilPlugin)

int main(int argc, char* argv[])
{
    using namespace ScreenPlay;
    // Lets keep using it until: https://bugreports.qt.io/browse/QTBUG-109401
    QtWebEngineQuick::initialize();

#if defined(Q_OS_WIN)
    // Workaround for Qt 6.5.1 crash https://bugreports.qt.io/browse/QTBUG-113832
    qputenv("QT_DISABLE_HW_TEXTURES_CONVERSION", "1");
    qputenv("QT_MEDIA_BACKEND", "ffmpeg");
#endif

    QGuiApplication app(argc, argv);

    QCoreApplication::setApplicationName("ScreenPlayWallpaper");
    QCoreApplication::setApplicationVersion("1.0");
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

    QStringList argumentList;
    if (app.arguments().length() == 1) {
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

        argumentList.append(
            QStringList {
                // Docs: Don't forget that arguments must start with the name of the executable (ignored, though).
                QGuiApplication::applicationName(),
                "--path", projectPath,
                "--appID", "qmz9lq4wglox5DdYaXumVgRSDeZYAUjC",
                "--screens", "{0}",
                "--volume", "1",
                "--fillmode", "Contain",
                "--type", "VideoWallpaper",
                "--check", "0" });
    } else {
        argumentList = app.arguments();
    }

    QCommandLineParser parser;
    parser.setApplicationDescription("ScreenPlay Wallpaper");
    parser.addHelpOption();

    // Define the command line options
    QCommandLineOption pathOption("path", "Set the project path.", "path");
    QCommandLineOption appIDOption("appID", "Set the application ID.", "appID");
    QCommandLineOption screensOption("screens", "Set screens parameter.", "screens");
    QCommandLineOption volumeOption("volume", "Set volume level.", "volume");
    QCommandLineOption fillmodeOption("fillmode", "Set fill mode.", "fillmode");
    QCommandLineOption typeOption("type", "Set the type.", "type");
    QCommandLineOption checkOption("check", "Set check value.", "check");

    // Add the options to the parser
    parser.addOption(pathOption);
    parser.addOption(appIDOption);
    parser.addOption(screensOption);
    parser.addOption(volumeOption);
    parser.addOption(fillmodeOption);
    parser.addOption(typeOption);
    parser.addOption(checkOption);

    // Process the actual command line arguments given by the user
    parser.process(argumentList);

    // Check if all required options are provided
    if (!parser.isSet(pathOption)
        || !parser.isSet(appIDOption)
        || !parser.isSet(screensOption)
        || !parser.isSet(volumeOption)
        || !parser.isSet(fillmodeOption)
        || !parser.isSet(typeOption)
        || !parser.isSet(checkOption)) {
        qCritical() << "Missing required arguments. Please provide all arguments."
                    << argumentList
                    << "pathOption" << parser.value(pathOption)
                    << "appIDOption" << parser.value(appIDOption)
                    << "typeOption" << parser.value(typeOption)
                    << "volumeOption" << parser.value(volumeOption)
                    << "fillmodeOption" << parser.value(fillmodeOption)
                    << "typeOption" << parser.value(typeOption)
                    << "checkOption" << parser.value(checkOption);
        return -1;
    }

    QString path = parser.value(pathOption);
    QString appID = parser.value(appIDOption);
    QString screens = parser.value(screensOption);
    QString volume = parser.value(volumeOption);
    QString fillmode = parser.value(fillmodeOption);
    QString type = parser.value(typeOption);
    QString check = parser.value(checkOption);

    ScreenPlay::Util util;

    auto activeScreensList = util.parseStringToIntegerList(screens);
    if (!activeScreensList.has_value()) {
        qCritical() << "Could not parse activeScreensList" << screens;
        return static_cast<int>(WallpaperExit::Code::Invalid_ActiveScreensList);
    }

    auto installedType = util.getInstalledTypeFromString(type);
    if (!installedType.has_value()) {
        qCritical() << "Cannot parse Wallpaper type from value" << type;
        return static_cast<int>(WallpaperExit::Code::Invalid_InstalledType);
    }

    bool okParseCheckWallpaperVisible = false;
    const bool checkWallpaperVisible = check.toInt(&okParseCheckWallpaperVisible);
    if (!okParseCheckWallpaperVisible) {
        qCritical("Could not parse checkWallpaperVisible");
        return static_cast<int>(WallpaperExit::Code::Invalid_CheckWallpaperVisible);
    }

    bool okParseVolume = false;
    const float volumeFloat = volume.toFloat(&okParseVolume);
    if (!okParseVolume) {
        qCritical("Could not parse Volume");
        return static_cast<int>(WallpaperExit::Code::Invalid_Volume);
    }

    // Set the properties of the window object
    window->setActiveScreensList(activeScreensList.value());
    window->setProjectPath(path);
    window->setAppID(appID);
    window->setVolume(volumeFloat);
    window->setFillMode(fillmode);
    window->setType(installedType.value());
    window->setCheckWallpaperVisible(checkWallpaperVisible);
    window->setDebugMode(false);

    const auto setupStatus = window->setup();
    if (setupStatus != WallpaperExit::Code::Ok) {
        return static_cast<int>(setupStatus);
    }
    const auto startStatus = window->start();
    if (startStatus != WallpaperExit::Code::Ok) {
        return static_cast<int>(startStatus);
    }
    emit window->qmlStart();
    logging = std::make_unique<const ScreenPlayUtil::LoggingHandler>("ScreenPlayWallpaper_" + parser.value(appIDOption));
    const int status = app.exec();
    logging.reset();
    return status;
}
