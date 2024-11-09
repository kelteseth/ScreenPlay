// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#include "ScreenPlay/CMakeVariables.h"
#include "ScreenPlay/applicationengine.h"
#include "ScreenPlayCore/logginghandler.h"
#include "qcorotask.h"
#include "qml/qcoroqml.h"
#include "qml/qcoroqmltask.h"
#include <QCommandLineParser>
#include <QDebug>
#include <QGuiApplication>
#include <QStyleFactory>
#if defined(Q_OS_WIN)
#include <sentry.h>
#endif

#ifdef SCREENPLAY_STEAM
Q_IMPORT_QML_PLUGIN(ScreenPlayWorkshopPlugin)
#endif

#include <QQmlEngineExtensionPlugin>
Q_IMPORT_QML_PLUGIN(ScreenPlayAppPlugin)
Q_IMPORT_QML_PLUGIN(ScreenPlayCorePlugin)

int main(int argc, char* argv[])
{
    QCoro::Qml::registerTypes();
#if !defined(Q_OS_LINUX)
    qputenv("QT_MEDIA_BACKEND", "ffmpeg");
#endif
    QGuiApplication qtGuiApp(argc, argv);
    ScreenPlay::ApplicationEngine appEngine;

    if (appEngine.isAnotherScreenPlayInstanceRunning()) {
        return 0;
    }

    auto logging = std::make_unique<const ScreenPlayCore::LoggingHandler>("ScreenPlay");
    appEngine.init();
    const int status = qtGuiApp.exec();
#if defined(Q_OS_WIN)
    sentry_shutdown();
#endif
    logging.reset();
    return status;
}
