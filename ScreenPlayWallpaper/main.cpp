#include <QApplication>
#include <QObject>
#include <QStringList>
#include <QVector>
#include <QtGlobal>
#include <QtWebEngineQuick>

#include "ScreenPlayUtil/util.h"

#if defined(Q_OS_WIN)
#include "src/winwindow.h"
Q_IMPORT_QML_PLUGIN(ScreenPlaySysInfoPlugin)
#elif defined(Q_OS_LINUX)
#include "src/linuxwindow.h"
#elif defined(Q_OS_OSX)
#include "src/macwindow.h"
#endif

Q_IMPORT_QML_PLUGIN(ScreenPlayWeatherPlugin)

int main(int argc, char* argv[])
{
    QApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
    QtWebEngineQuick::initialize();

    QApplication app(argc, argv);

#if defined(Q_OS_WIN)
    WinWindow window;
#elif defined(Q_OS_LINUX)
    LinuxWindow window;
#elif defined(Q_OS_OSX)
    MacWindow window;
#endif

    // If we start with only one argument (app path)
    // It means we want to test a single wallpaper
    const QStringList argumentList = app.arguments();

    // For testing purposes when starting the ScreenPlayWallpaper directly.
    if (argumentList.length() == 1) {
        window.setActiveScreensList({ 0 });
        // window.setProjectPath("test");
        window.setProjectPath("C:/Program Files (x86)/Steam/steamapps/workshop/content/672870/1958068741");
        window.setAppID("test");
        window.setVolume(1);
        window.setFillMode("fill");
        window.setType(ScreenPlay::InstalledType::InstalledType::VideoWallpaper);
        window.setCheckWallpaperVisible(true);
        window.setDebugMode(true);
    } else {
        // 8 parameter + 1 OS working directory as the first default paramter
        if (argumentList.length() != 9) {
            return -3;
        }

        const auto activeScreensList = ScreenPlayUtil::parseStringToIntegerList(argumentList.at(1));

        if (!activeScreensList.has_value()) {
            qCritical("Could not activeScreensList");
            return -1;
        }

        auto installedType = ScreenPlay::InstalledType::InstalledType::Unknown;
        if (auto typeOpt = ScreenPlayUtil::getInstalledTypeFromString(argumentList.at(6))) {
            installedType = typeOpt.value();
        } else {
            qCritical() << "Cannot parse Wallpaper type from value" << argumentList.at(6);
            return -6;
        }

        bool okParseCheckWallpaperVisible = false;
        const bool checkWallpaperVisible = argumentList.at(7).toInt(&okParseCheckWallpaperVisible);
        if (!okParseCheckWallpaperVisible) {
            qCritical("Could not parse checkWallpaperVisible");
            return -5;
        }
        bool okParseVolume = 0.0f;
        const float volume = argumentList.at(4).toFloat(&okParseVolume);
        if (!okParseVolume) {
            qCritical("Could not parse Volume");
            return -6;
        }

        QString appID = argumentList.at(3);
        if (!appID.startsWith("appID=")) {
            qCritical("Invalid appID");
            return -6;
        }
        appID = appID.remove("appID=");

        window.setActiveScreensList(activeScreensList.value());
        window.setProjectPath(argumentList.at(2));
        window.setAppID(appID);
        window.setVolume(volume);
        window.setFillMode(argumentList.at(5));
        window.setType(installedType);
        window.setCheckWallpaperVisible(checkWallpaperVisible);
        window.setDebugMode(false);
    }

    const auto setupStatus = window.setup();
    if (setupStatus != BaseWindow::ExitCode::Success) {
        return static_cast<int>(setupStatus);
    }
    const auto startStatus = window.start();
    if (startStatus != BaseWindow::ExitCode::Success) {
        return static_cast<int>(startStatus);
    }
    return app.exec();
}
