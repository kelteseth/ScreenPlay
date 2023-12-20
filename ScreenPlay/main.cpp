// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#include "ScreenPlay/CMakeVariables.h"
#include "ScreenPlay/app.h"
#include "ScreenPlayUtil/logginghandler.h"
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

Q_IMPORT_QML_PLUGIN(ScreenPlayAppPlugin)
Q_IMPORT_QML_PLUGIN(ScreenPlayUtilPlugin)
Q_IMPORT_QML_PLUGIN(PlausiblePlugin)

int main(int argc, char* argv[])
{
    QCoro::Qml::registerTypes();
#if !defined(Q_OS_LINUX)
    qputenv("QT_MEDIA_BACKEND", "ffmpeg");
#endif

    QGuiApplication qtGuiApp(argc, argv);
    ScreenPlay::App app;

    if (app.m_isAnotherScreenPlayInstanceRunning) {
        return 0;
    }

    auto logging = std::make_unique<const ScreenPlayUtil::LoggingHandler>("ScreenPlay");
    app.init();
    const int status = qtGuiApp.exec();
#if defined(Q_OS_WIN)
    sentry_shutdown();
#endif
    logging.reset();
    return status;
}
