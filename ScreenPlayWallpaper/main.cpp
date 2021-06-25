#include <QApplication>
#include <QObject>
#include <QStringList>
#include <QVector>
#include <QtGlobal>
#include <QtWebEngine/QtWebEngine>

#include "ScreenPlayUtil/util.h"

#if defined(Q_OS_WIN)
#include "src/winwindow.h"
#elif defined(Q_OS_LINUX)
#include "src/linuxwindow.h"
#elif defined(Q_OS_OSX)
#include "src/macwindow.h"
#endif

int main(int argc, char* argv[])
{
    //QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
    QtWebEngine::initialize();

    QApplication app(argc, argv);

    // This gives us nice clickable output in QtCreator
    qSetMessagePattern("%{if-category}%{category}: %{endif}%{message}\n   Loc: [%{file}:%{line}]");

    // If we start with only one argument (app path)
    // It means we want to test a single wallpaper
    const QStringList argumentList = app.arguments();

    // For testing purposes when starting the ScreenPlayWallpaper directly.
    if (argumentList.length() == 1) {
#if defined(Q_OS_WIN)
        WinWindow window1({ 0 }, "test", "appID=test", "1", "fill", "videoWallpaper", true, true);
        //WinWindow window1({ 0 }, "C:/Program Files (x86)/Steam/steamapps/workshop/content/672870/_tmp_171806", "appID=test", "1", "fill", "videoWallpaper", true, true);
#elif defined(Q_OS_LINUX)
        LinuxWindow window({ 0 }, "test", "appid", "1", "fill", false);
#elif defined(Q_OS_OSX)
        MacWindow window({ 0 }, "test", "appid", "1", "fill");
#endif
        return app.exec();
    }

    // 8 parameter + 1 OS working directory as the first default paramter
    if (argumentList.length() != 9) {
        return -3;
    }

    const bool debugMode = false;

    const auto activeScreensList = ScreenPlayUtil::parseStringToIntegerList(argumentList.at(1));

    if (!activeScreensList.has_value())
        return -4;

    // See ScreenPlayWallpaper m_appArgumentsList constructor how the args get created
    const QString projectFilePath = argumentList.at(2);
    const QString appID = argumentList.at(3);
    const QString volume = argumentList.at(4);
    const QString fillmode = argumentList.at(5);
    const QString type = argumentList.at(6);

    bool okParseCheckWallpaperVisible = false;
    bool checkWallpaperVisible = argumentList.at(7).toInt(&okParseCheckWallpaperVisible);
    if (!okParseCheckWallpaperVisible) {
        qFatal("Could not parse checkWallpaperVisible");
        return -5;
    }

#if defined(Q_OS_WIN)
    WinWindow window(
        activeScreensList.value(),
        projectFilePath,
        appID,
        volume,
        fillmode,
        type,
        checkWallpaperVisible,
        debugMode);
#elif defined(Q_OS_LINUX)
    LinuxWindow window(
        activeScreensList.value(),
        projectPath,
        appID,
        fillmode,
        volume,
        checkWallpaperVisible);
#elif defined(Q_OS_OSX)
    MacWindow window(
        activeScreensList.value(),
        projectPath,
        appID,
        fillmode,
        volume,
        checkWallpaperVisible);
#endif

    return app.exec();
}
