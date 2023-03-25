// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QString>
#include <QStringList>
#include <QtWebEngineQuick>

#include "ScreenPlayWidget/CMakeVariables.h"

#include "src/widgetwindow.h"

#if defined(Q_OS_WIN)
Q_IMPORT_QML_PLUGIN(ScreenPlaySysInfoPlugin)
#endif
#if defined(Q_OS_OSX)
#include "ScreenPlayUtil/macutils.h"
#endif

Q_IMPORT_QML_PLUGIN(ScreenPlayWeatherPlugin)

int main(int argc, char* argv[])
{

#if !defined(Q_OS_LINUX)
    qputenv("QT_MEDIA_BACKEND", "ffmpeg");
#endif

    QGuiApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
    QtWebEngineQuick::initialize();

    QGuiApplication app(argc, argv);

    const QStringList argumentList = app.arguments();

    // If we start with only one argument (path, appID, type),
    // it means we want to test a single widget
    if (argumentList.length() == 1) {
        //WidgetWindow spwmw("test", "appid", "qmlWidget", { 100, 100 }, true);

        QString exampleContentPath = QString(SCREENPLAY_SOURCE_DIR) + "/Content";
        QStringList contentFolder = {
            "/widget_weather",
            "/widget_system_stats", // Note: Windows only
            "/widget_hello_world",
            "/widget_countDown",
            "/widget_analogClock",
            "/widget_digitalClock",
            "/widget_rss_reddit",
            "/widget_rss_dw_news",
            "/widget_xkcd"
        };
        QString projectPath = exampleContentPath + contentFolder.at(0);

        WidgetWindow spwmw(projectPath,
            "appid",
            "qmlWidget",
            { 0, 0 },
            true);

        return app.exec();
    }

    if (argumentList.length() != 6) {
        return -3;
    }

    bool okPosX = false;
    int positionX = QVariant(argumentList.at(4)).toInt(&okPosX);
    if (!okPosX) {
        qWarning() << "Could not parse PositionX value to int: " << argumentList.at(4);
        positionX = 0;
    }
    bool okPosY = false;
    int positionY = QVariant(argumentList.at(5)).toInt(&okPosY);
    if (!okPosY) {
        qWarning() << "Could not parse PositionY value to int: " << argumentList.at(5);
        positionY = 0;
    }

    WidgetWindow spwmw(
        argumentList.at(1), // Project path,
        argumentList.at(2), // AppID
        argumentList.at(3), // Type
        QPoint { positionX, positionY });

#if defined(Q_OS_OSX)
    MacUtils::showDockIcon(false);
#endif

    return app.exec();
}
