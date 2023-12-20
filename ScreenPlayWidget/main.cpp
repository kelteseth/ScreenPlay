// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#include "ScreenPlayWidget/CMakeVariables.h"
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlExtensionPlugin>
#include <QString>
#include <QStringList>
#include <QtWebEngineQuick>

#include "ScreenPlayUtil/contenttypes.h"
#include "ScreenPlayUtil/logginghandler.h"
#include "src/widgetwindow.h"

#if defined(Q_OS_WIN)
Q_IMPORT_QML_PLUGIN(ScreenPlaySysInfoPlugin)
#endif
#if defined(Q_OS_MACOS)
#include "ScreenPlayUtil/macutils.h"
#endif

Q_IMPORT_QML_PLUGIN(ScreenPlayWeatherPlugin)
Q_IMPORT_QML_PLUGIN(ScreenPlayUtilPlugin)

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

    QStringList argumentList;

    // If we start with only one argument (path, appID, type),
    // it means we want to test a single widget
    if (app.arguments().length() == 1) {
        QString exampleContentPath = QString(SCREENPLAY_SOURCE_DIR) + "/Content";
        using namespace ScreenPlay;
        QList<QPair<QString, ContentTypes::InstalledType>> contentFolder = {
            { "/widget_weather", ContentTypes::InstalledType::QMLWidget }, // 0
            { "/widget_system_stats", ContentTypes::InstalledType::QMLWidget }, // 1 (Note: Windows only)
            { "/widget_hello_world", ContentTypes::InstalledType::QMLWidget }, // 2
            { "/widget_year_count_down", ContentTypes::InstalledType::QMLWidget }, // 3
            { "/widget_analog_clock", ContentTypes::InstalledType::QMLWidget }, // 4
            { "/widget_digital_clock", ContentTypes::InstalledType::QMLWidget }, // 5
            { "/widget_rss_hackernews", ContentTypes::InstalledType::QMLWidget }, // 6
            { "/widget_rss_guardian_news", ContentTypes::InstalledType::QMLWidget }, // 7
            { "/widget_xkcd", ContentTypes::InstalledType::QMLWidget }, // 8
        };
        const int index = 5;
        QString projectPath = exampleContentPath + contentFolder.at(index).first;

        // Lets center the widget
        const auto* screen = QGuiApplication::screens().at(0);
        const int offset = -200;
        const QPoint center((screen->size().width() / 2) + offset, (screen->size().height() / 2) + offset);
        const QString type = QVariant::fromValue<ContentTypes::InstalledType>(contentFolder.at(index).second).toString();

        argumentList.append(
            QStringList {
                // Docs: Don't forget that arguments must start with the name of the executable (ignored, though).
                QGuiApplication::applicationName(),
                "--path", projectPath,
                "--appID", "qmz9lq4wglox5DdYaXumVgRSDeZYAUjC",
                "--type", type,
                "--posX", QString::number(center.x()),
                "--posY", QString::number(center.y()),
                "--debug", "1" });
    } else {
        argumentList = app.arguments();
    }

    QCommandLineParser parser;
    parser.setApplicationDescription("ScreenPlay Widget");
    parser.addHelpOption();
    QCommandLineOption pathOption("path", "Project path", "path");
    QCommandLineOption appIDOption("appID", "Application ID", "appid");
    QCommandLineOption typeOption("type", "Content type", "type");
    QCommandLineOption posXOption("posX", "X position", "positionX");
    QCommandLineOption posYOption("posY", "Y position", "positionY");
    QCommandLineOption debugOption("debug", "debug enabled", "debug");

    parser.addOption(pathOption);
    parser.addOption(appIDOption);
    parser.addOption(typeOption);
    parser.addOption(posXOption);
    parser.addOption(posYOption);
    parser.addOption(debugOption);

    parser.process(argumentList);

    // Check if all required options are provided
    // debug option is optional
    if (!parser.isSet(pathOption)
        || !parser.isSet(appIDOption)
        || !parser.isSet(typeOption)
        || !parser.isSet(posXOption)
        || !parser.isSet(posYOption)) {
        qCritical() << "Missing required arguments. Please provide all arguments."
                    << argumentList
                    << "pathOption" << parser.value(pathOption)
                    << "appIDOption" << parser.value(appIDOption)
                    << "typeOption" << parser.value(typeOption)
                    << "posXOption" << parser.value(posXOption)
                    << "posYOption" << parser.value(posYOption);
        return -1;
    }

    bool okPosX = false, okPosY = false;
    const int positionX = parser.value(posXOption).toInt(&okPosX);
    if (!okPosX) {
        qWarning() << "Could not parse PositionX value to int: " << parser.value(posXOption);
        return -1;
    }

    const int positionY = parser.value(posYOption).toInt(&okPosY);
    if (!okPosY) {
        qWarning() << "Could not parse PositionY value to int: " << parser.value(posYOption);
        return -1;
    }

    bool debugOk = false;
    const int debug = parser.value(debugOption).toInt(&debugOk);
    if (!debugOk) {
        qWarning() << "Could not parse debugOk value to bool: " << parser.value(posYOption);
    }

    QString appID = parser.value(appIDOption);
    QString projectPath = parser.value(pathOption);
    QString type = parser.value(typeOption);

    WidgetWindow spwmw(
        projectPath,
        appID,
        type,
        QPoint { positionX, positionY },
        debug);

#if defined(Q_OS_MACOS)
    MacUtils::showDockIcon(false);
#endif
    logging = std::make_unique<const ScreenPlayUtil::LoggingHandler>("ScreenPlayWidget_" + appID);
    const int status = app.exec();
    logging.reset();
    return status;
}
