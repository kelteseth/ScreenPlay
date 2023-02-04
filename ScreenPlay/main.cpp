// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#include "ScreenPlay/app.h"
#include <QCommandLineParser>
#include <QDebug>
#include <QGuiApplication>
#include <QStyleFactory>

#if defined(Q_OS_WIN)
#include <sentry.h>
#endif

Q_IMPORT_QML_PLUGIN(ScreenPlayAppPlugin)
Q_IMPORT_QML_PLUGIN(ScreenPlayUtilPlugin)
#ifdef SCREENPLAY_STEAM
Q_IMPORT_QML_PLUGIN(ScreenPlayWorkshopPlugin)
#endif

int main(int argc, char* argv[])
{

#if !defined(Q_OS_LINUX)
    qputenv("QT_MEDIA_BACKEND", "ffmpeg");
#endif

    QGuiApplication qtGuiApp(argc, argv);
    ScreenPlay::App app;

    if (app.m_isAnotherScreenPlayInstanceRunning) {
        return 0;
    } else {
        app.init();
        const int status = qtGuiApp.exec();
#if defined(Q_OS_WIN)
        sentry_shutdown();
#endif
        return status;
    }
}
