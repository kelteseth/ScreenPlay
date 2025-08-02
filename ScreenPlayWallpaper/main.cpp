#include <QGuiApplication>
#include <QObject>
#include <QStringList>
#include <QVector>
#include <QtGlobal>
#include <QtWebEngineQuick>

#include "CMakeVariables.h"

#include "ScreenPlayCore/exitcodes.h"
#include "ScreenPlayCore/globalenums.h"
#include "ScreenPlayCore/logginghandler.h"
#include "ScreenPlayCore/util.h"

#include <QQmlEngine>

#if defined(Q_OS_WIN)
#include "src/winwindow.h"
#elif defined(Q_OS_LINUX)
#include "src/linuxwaylandwindow.h"
#include "src/linuxx11window.h"
#elif defined(Q_OS_MACOS)
#include "src/macwindow.h"
#endif

int main(int argc, char* argv[])
{
    // Graphics API will be set later based on command line arguments
    using namespace ScreenPlay;
    QtWebEngineQuick::initialize();

    QGuiApplication app(argc, argv);

    QCoreApplication::setApplicationName("ScreenPlayWallpaper");
    QCoreApplication::setApplicationVersion("1.0.0");
    std::unique_ptr<const ScreenPlayCore::LoggingHandler> logging;

    auto quickView = std::make_shared<QQuickView>();

#if defined(Q_OS_WIN)
    auto window = std::make_unique<WinWindow>();
    qmlRegisterSingletonInstance<WinWindow>("ScreenPlayWallpaper", 1, 0, "Wallpaper", window.get());
    window->setQuickView(quickView);
#elif defined(Q_OS_LINUX)
    const auto platformName = QGuiApplication::platformName();
    std::unique_ptr<BaseWindow> window;

    if (platformName == "xcb") {
        auto x11Window = std::make_unique<LinuxX11Window>();
        qmlRegisterSingletonInstance<LinuxX11Window>("ScreenPlayWallpaper", 1, 0, "Wallpaper", x11Window.get());
        x11Window->setQuickView(quickView);
        window = std::move(x11Window);
    } else if (platformName == "wayland") {
        auto waylandWindow = std::make_unique<LinuxWaylandWindow>();
        qmlRegisterSingletonInstance<LinuxWaylandWindow>("ScreenPlayWallpaper", 1, 0, "Wallpaper", waylandWindow.get());
        waylandWindow->setQuickView(quickView);
        window = std::move(waylandWindow);
    }

    if (!window) {
        return -5;
    }
#elif defined(Q_OS_MACOS)
    auto window = std::make_unique<MacWindow>();
    qmlRegisterSingletonInstance<MacWindow>("ScreenPlayWallpaper", 1, 0, "Wallpaper", window.get());
    window->setQuickView(quickView);
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
                "--projectpath", projectPath,
                "--appID", "qmz9lq4wglox5DdYaXumVgRSDeZYAUjC",
                "--screens", "{0}",
                "--volume", "1",
                "--fillmode", "Contain",
                "--type", "VideoWallpaper",
                "--check", "0",
                "--mainapppid", "-1",
                "--graphicsapi", QString::number(static_cast<int>(ScreenPlayEnums::GraphicsApi::Auto)) });
    } else {
        argumentList = app.arguments();
    }

    QCommandLineParser parser;
    parser.setApplicationDescription("ScreenPlay Wallpaper");
    parser.addHelpOption();

    // Define the command line options
    QCommandLineOption pathOption("projectpath", "Set the project path.", "projectpath");
    QCommandLineOption appIDOption("appID", "Set the application ID.", "appID");
    QCommandLineOption screensOption("screens", "Set screens parameter.", "screens");
    QCommandLineOption volumeOption("volume", "Set volume level.", "volume");
    QCommandLineOption fillmodeOption("fillmode", "Set fill mode.", "fillmode");
    QCommandLineOption typeOption("type", "Set the type.", "type");
    QCommandLineOption checkOption("check", "Set check value.", "check");
    QCommandLineOption mainAppPidOption("mainapppid", "pid of the main ScreenPlay app. User to check if we are still alive.", "mainapppid");
    QCommandLineOption graphicsApiOption("graphicsapi", "Set the graphics API.", "graphicsapi");

    // Add the options to the parser
    parser.addOption(pathOption);
    parser.addOption(appIDOption);
    parser.addOption(screensOption);
    parser.addOption(volumeOption);
    parser.addOption(fillmodeOption);
    parser.addOption(typeOption);
    parser.addOption(checkOption);
    parser.addOption(mainAppPidOption);
    parser.addOption(graphicsApiOption);

    // Process the actual command line arguments given by the user
    parser.process(argumentList);

    // Check if all required options are provided
    if (!parser.isSet(pathOption)
        || !parser.isSet(appIDOption)
        || !parser.isSet(screensOption)
        || !parser.isSet(volumeOption)
        || !parser.isSet(fillmodeOption)
        || !parser.isSet(typeOption)
        || !parser.isSet(checkOption)
        || !parser.isSet(mainAppPidOption)) {
        qCritical() << "Missing required arguments. Please provide all arguments."
                    << argumentList
                    << "pathOption" << parser.value(pathOption)
                    << "appIDOption" << parser.value(appIDOption)
                    << "typeOption" << parser.value(typeOption)
                    << "volumeOption" << parser.value(volumeOption)
                    << "fillmodeOption" << parser.value(fillmodeOption)
                    << "typeOption" << parser.value(typeOption)
                    << "checkOption" << parser.value(checkOption)
                    << "mainAppPidOption" << parser.value(mainAppPidOption);
        return -1;
    }

    QString path = parser.value(pathOption);
    QString appID = parser.value(appIDOption);
    QString screens = parser.value(screensOption);
    QString volume = parser.value(volumeOption);
    QString fillmode = parser.value(fillmodeOption);
    QString type = parser.value(typeOption);
    QString check = parser.value(checkOption);
    QString pid = parser.value(mainAppPidOption);
    QString graphicsApi = parser.value(graphicsApiOption); // Optional parameter

    ScreenPlay::Util util;
    logging = std::make_unique<const ScreenPlayCore::LoggingHandler>("ScreenPlayWallpaper_" + parser.value(appIDOption));

    // Set graphics API before any graphics-related initialization
    if (!graphicsApi.isEmpty()) {
        bool ok;
        int enumValue = graphicsApi.toInt(&ok);
        if (ok) {
            auto apiEnum = static_cast<ScreenPlayEnums::GraphicsApi>(enumValue);
            switch (apiEnum) {
            case ScreenPlayEnums::GraphicsApi::DirectX11:
#ifdef Q_OS_WIN
                QQuickWindow::setGraphicsApi(QSGRendererInterface::Direct3D11Rhi);
                qInfo() << "Graphics API set to Direct3D11";
#else
                qWarning() << "DirectX11 is only available on Windows, falling back to default";
#endif
                break;
            case ScreenPlayEnums::GraphicsApi::OpenGL:
                QQuickWindow::setGraphicsApi(QSGRendererInterface::OpenGLRhi);
                qInfo() << "Graphics API set to OpenGL";
                break;
            case ScreenPlayEnums::GraphicsApi::Auto:
                // Don't set anything, let Qt decide
                qInfo() << "Graphics API set to Auto (Qt default)";
                break;
            default:
                qWarning() << "Unknown graphics API enum value:" << enumValue << "- using default";
                break;
            }
        } else {
            qWarning() << "Invalid graphics API value:" << graphicsApi << "- using default";
        }
    } else {
        qInfo() << "No graphics API specified, using Qt default";
    }

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

    bool okPid = false;
    const qint64 mainAppPidInt = pid.toInt(&okPid);
    if (!okPid) {
        qCritical("Could not parse mainAppPid");
        return static_cast<int>(WallpaperExit::Code::Invalid_PID);
    }

    // Set the properties of the window object
    window->setActiveScreensList(activeScreensList.value());
    window->setProjectPath(path);
    window->setAppID(appID);
    window->setVolume(volumeFloat);
    window->setFillMode(fillmode);
    window->setType(installedType.value());
    window->setCheckWallpaperVisible(checkWallpaperVisible);
    window->setDebugMode(mainAppPidInt == -1);
    window->setMainAppPID(mainAppPidInt);

    const auto setupStatus = window->setup();
    if (setupStatus != WallpaperExit::Code::Ok) {
        return static_cast<int>(setupStatus);
    }
    const auto startStatus = window->start();
    if (startStatus != WallpaperExit::Code::Ok) {
        return static_cast<int>(startStatus);
    }
    quickView->loadFromModule("ScreenPlayWallpaper", "ScreenPlayWallpaperMain");
    emit window->qmlStart();

    // Explicit order when to connect:
    // Make sure to load the wallpaper first and only then
    // connect to the main app. This is needed, because the
    // main app sends all user defined property settings
    // on successful connection. At this time, the qml engine
    // must be ready to receive them in onQmlSceneValueReceived(key, value)
    window->connectToMainApp();

    const int status = app.exec();
    logging.reset();
    return status;
}
