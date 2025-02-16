// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#include "CMakeVariables.h"
#include "ScreenPlay/app.h"
#include "ScreenPlayCore/logginghandler.h"
#include "qcorotask.h"
#include "qml/qcoroqml.h"
#include "qml/qcoroqmltask.h"
#include <QCommandLineParser>
#include <QDebug>
#include <QGuiApplication>
#include <QLocalSocket>
#include <QQmlApplicationEngine>
#include <QStyleFactory>

#include <QIcon>
#include <QQuickStyle>

#if defined(Q_OS_WIN)
#include <sentry.h>
#endif

bool isAnotherScreenPlayInstanceRunning()
{
    QLocalSocket socket;
    socket.connectToServer("ScreenPlay", QIODeviceBase::ExistingOnly);
    // If we cannot connect to a running ScreenPlay
    // pipe we are alone
    if (!socket.isOpen()) {
        socket.close();
        return false;
    }

    qInfo("Another ScreenPlay app is already running!");
    QByteArray msg = "command=requestRaise";
    socket.write(msg);
    socket.waitForBytesWritten(500);
    socket.close();
    return true;
}

int main(int argc, char* argv[])
{
    using namespace ScreenPlay;
    QCoro::Qml::registerTypes();
#if !defined(Q_OS_LINUX)
    qputenv("QT_MEDIA_BACKEND", "ffmpeg");
#endif

    QGuiApplication qtGuiApp(argc, argv);
    QGuiApplication::setOrganizationName("ScreenPlay");
    QGuiApplication::setOrganizationDomain("screen-play.app");
    QGuiApplication::setApplicationName("ScreenPlay");
    QGuiApplication::setApplicationVersion(QString(SCREENPLAY_VERSION));
    QGuiApplication::setQuitOnLastWindowClosed(false);
    QGuiApplication::setWindowIcon(QIcon(":/qt/qml/ScreenPlay/assets/icons/app.ico"));
    if (isAnotherScreenPlayInstanceRunning()) {
        return -5;
    }
    auto logging = std::make_unique<const ScreenPlayCore::LoggingHandler>("ScreenPlay");

    QQuickStyle::setStyle("Material");
    QQmlApplicationEngine qmlApplicationEngine;
    qmlApplicationEngine.loadFromModule("ScreenPlay", "ScreenPlayMain");
    const int status = qtGuiApp.exec();
#if defined(Q_OS_WIN)
    sentry_shutdown();
#endif
    logging.reset();
    return status;
}
