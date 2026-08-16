#include <QGuiApplication>
#include <QObject>
#include <QStringList>
#include <QVector>
#include <QtGlobal>
#include <QtWebEngineQuick>

#include "CMakeVariables.h"

#include "ScreenPlayCore/exitcodes.h"
#include "ScreenPlayCore/globalenums.h"
#include "ScreenPlayCore/graphicsapi.h"
#include "ScreenPlayCore/logginghandler.h"
#include "ScreenPlayCore/util.h"

#include <QQmlEngine>

#if defined(Q_OS_WIN)
#include "src/winwindow.h"
#include <sentry.h>
#elif defined(Q_OS_LINUX)
#include "src/linuxwaylandwindow.h"
#include "src/linuxx11window.h"
#elif defined(Q_OS_MACOS)
#include "src/macwindow.h"
#endif

#include "src/framelimiter.h"
#include "src/wallpaperstate.h"

int main(int argc, char* argv[])
{
    // Graphics API will be set later based on command line arguments
    using namespace ScreenPlay;
    QtWebEngineQuick::initialize();

    QGuiApplication app(argc, argv);

    QCoreApplication::setApplicationName("ScreenPlayWallpaper");
    QCoreApplication::setApplicationVersion("1.0.0");
    std::unique_ptr<const ScreenPlayCore::LoggingHandler> logging;

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
                "--graphicsapi", QString::number(static_cast<int>(ScreenPlayEnums::GraphicsApi::Auto)),
                "--anonymoustelemetry", "true" });
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
    QCommandLineOption fpsLimitOption("fpslimit", "Limit rendering to the given frames per second. 0 disables the limit.", "fpslimit");
    QCommandLineOption anonymousTelemetryOption("anonymoustelemetry", "Enable anonymous telemetry.", "anonymoustelemetry");
    QCommandLineOption reapplySpacesOption("reapplyspaces", "Reapply wallpaper window after Mission Control space changes (macOS only).", "reapplyspaces");

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
    parser.addOption(fpsLimitOption);
    parser.addOption(anonymousTelemetryOption);
    parser.addOption(reapplySpacesOption);

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
    QString fpsLimit = parser.value(fpsLimitOption); // Optional parameter
    QString anonymousTelemetry = parser.value(anonymousTelemetryOption); // Optional parameter
    QString reapplySpacesValue = parser.value(reapplySpacesOption);

    ScreenPlay::Util util;
    logging = std::make_unique<const ScreenPlayCore::LoggingHandler>("ScreenPlayWallpaper_" + parser.value(appIDOption));

    // Initialize Sentry for crash reporting if anonymousTelemetry is enabled
    bool enableTelemetry = false;
    if (!anonymousTelemetry.isEmpty()) {
        enableTelemetry = (anonymousTelemetry.toLower() == "true");
    }

    if (enableTelemetry) {
#if defined(Q_OS_WIN)
        sentry_options_t* options = sentry_options_new();
        sentry_options_set_dsn(options, "https://6aeb6d6dcf3106f15f936acd484d43aa@o428218.ingest.us.sentry.io/4510085148639233");
        QString environment = QGuiApplication::applicationVersion() + "";
        sentry_options_set_environment(options, QString(environment).toStdString().c_str());

        const QString appPath = QGuiApplication::applicationDirPath();
        sentry_options_set_handler_path(options, QString(appPath + "/crashpad_handler.exe").toStdString().c_str());
        sentry_options_set_database_path(options, appPath.toStdString().c_str());
        const int sentryInitStatus = sentry_init(options);
        if (sentryInitStatus != 0) {
            qWarning() << "Unable to init sentry crashhandler with statuscode: " << sentryInitStatus;
        }
#endif
    }

    // Set graphics API before creating the QQuickView below. A QQuickWindow
    // fixes its native surface type (Direct3D/Vulkan/OpenGL) when it is
    // constructed; selecting a different API afterwards leaves the window
    // without a matching surface and crashes the graphics driver on startup.
    {
        auto apiEnum = ScreenPlayEnums::GraphicsApi::Auto;
        bool ok = false;
        const int enumValue = graphicsApi.toInt(&ok);
        if (ok && QMetaEnum::fromType<ScreenPlayEnums::GraphicsApi>().valueToKey(enumValue)) {
            apiEnum = static_cast<ScreenPlayEnums::GraphicsApi>(enumValue);
        } else if (!graphicsApi.isEmpty()) {
            qWarning() << "Invalid graphics API value:" << graphicsApi << "- using Auto";
        }
        applyGraphicsApi(apiEnum);
    }

    // The wallpaper always uses the single threaded "basic" render loop:
    // its UpdateRequest driven scheduling is what lets FrameRateLimiter
    // throttle rendering (including live fps limit changes from the main
    // app), and unlike the threaded loop its animations advance by wall
    // clock time, so they stay time-correct at any cap. Vsync stays on;
    // the absolute pacing grid in FrameRateLimiter keeps the average rate
    // exact even though single frames snap to vblanks.
    // QT_QPA_UPDATE_IDLE_TIME removes the 5ms platform delay between
    // frames, which would otherwise drop frames at high refresh rates.
    int fpsLimitValue = 0;
    {
        bool okFpsLimit = false;
        fpsLimitValue = fpsLimit.toInt(&okFpsLimit);
        if (!okFpsLimit || fpsLimitValue < 0)
            fpsLimitValue = 0;
        if (qEnvironmentVariableIsSet("QSG_RENDER_LOOP")) {
            qWarning() << "QSG_RENDER_LOOP override active - fps limit unavailable";
            fpsLimitValue = 0;
        } else {
            qputenv("QSG_RENDER_LOOP", "basic");
            qputenv("QT_QPA_UPDATE_IDLE_TIME", "0");
        }
        if (fpsLimitValue > 0)
            qInfo() << "Wallpaper fps limit set to" << fpsLimitValue;
    }

    auto quickView = std::make_shared<QQuickView>();
    FrameRateLimiter frameRateLimiter(quickView.get());
    frameRateLimiter.setMaxFps(fpsLimitValue);

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

    // Live fps limit updates arrive from the main app as an "fpsLimit"
    // SDK message (see BaseWindow::messageReceived).
    window->setFpsLimit(fpsLimitValue);
    QObject::connect(window.get(), &BaseWindow::fpsLimitChanged, &frameRateLimiter, &FrameRateLimiter::setMaxFps);

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

    bool reapplySpaces = false;
    if (!reapplySpacesValue.isEmpty()) {
        reapplySpaces = reapplySpacesValue.trimmed() == "true";
    }

    // Set the properties of the window object
    window->setActiveScreensList(activeScreensList.value());
    window->setProjectPath(path);
    window->setAppID(appID);
    // Initialize both state objects with the same values
    window->currentState()->setVolume(volumeFloat);
    window->currentState()->setFillMode(fillmode);
    window->currentState()->setLoops(true);
    window->currentState()->setIsPlaying(true);
    window->currentState()->setCheckWallpaperVisible(checkWallpaperVisible);
    // Target state starts with the same values
    window->targetState()->setVolume(volumeFloat);
    window->targetState()->setFillMode(fillmode);
    window->targetState()->setLoops(true);
    window->targetState()->setIsPlaying(true);
    window->targetState()->setCheckWallpaperVisible(checkWallpaperVisible);
    window->setType(installedType.value());
    window->setDebugMode(mainAppPidInt == -1);
    window->setMainAppPID(mainAppPidInt);
    window->setReapplySpacesEnabled(reapplySpaces);

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
#if defined(Q_OS_WIN)
    if (enableTelemetry) {
        sentry_close();
    }
#endif
    logging.reset();
    return status;
}
