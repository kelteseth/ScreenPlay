// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#include "ScreenPlayWidget/CMakeVariables.h"
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlExtensionPlugin>
#include <QString>
#include <QStringList>
#include <QtWebEngineQuick>

#include "src/widgetwindow.h"
#include "ScreenPlayUtil/logginghandler.h"

#if defined(Q_OS_WIN)
Q_IMPORT_QML_PLUGIN(ScreenPlaySysInfoPlugin)
#endif
#if defined(Q_OS_OSX)
#include "ScreenPlayUtil/macutils.h"
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

    const QStringList argumentList = app.arguments();

    // If we start with only one argument (path, appID, type),
    // it means we want to test a single widget
    if (argumentList.length() == 1) {
        QString exampleContentPath = QString(SCREENPLAY_SOURCE_DIR) + "/Content";
        QStringList contentFolder = {
            "/widget_weather", // 0
            "/widget_system_stats", // 1 (Note: Windows only)
            "/widget_hello_world", // 2
            "/widget_year_count_down", // 3
            "/widget_analog_clock", // 4
            "/widget_digital_clock", // 5
            "/widget_rss_hackernews", // 6
            "/widget_rss_guardian_news", // 7
            "/widget_xkcd" // 8
        };
        const int index = 5;
        QString projectPath = exampleContentPath + contentFolder.at(index);

        // Lets center the widget
        const auto* screen = QGuiApplication::screens().at(0);
        const int offset = -200;
        QPoint center((screen->size().width() / 2) + offset, (screen->size().height() / 2) + offset);

        WidgetWindow spwmw(projectPath,
            "appid",
            "qmlWidget",
            center,
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
    const QString appID = argumentList.at(2);
    WidgetWindow spwmw(
        argumentList.at(1), // Project path,
        appID, // AppID
        argumentList.at(3), // Type
        QPoint { positionX, positionY });

#if defined(Q_OS_OSX)
    MacUtils::showDockIcon(false);
#endif
    logging = std::make_unique<const ScreenPlayUtil::LoggingHandler>("ScreenPlayWidget_"+ appID);
    const int status = app.exec();
    logging.reset();
    return status;
}
