// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#include "CMakeVariables.h"
#include "ScreenPlay/app.h"
#include "ScreenPlayCore/logginghandler.h"
#include "qml/qcoroqml.h"
#include <QCommandLineParser>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QGuiApplication>
#include <QLocalSocket>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QStandardPaths>
#include <QStyleFactory>

#include <QIcon>
#include <QQuickStyle>

#include <tester/TesterServer.h>

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
    // https://bugreports.qt.io/browse/QTBUG-116388
    qputenv("QT_QUICK_FLICKABLE_WHEEL_DECELERATION", "5000");
    using namespace ScreenPlay;
    QCoro::Qml::registerTypes();

    QGuiApplication qtGuiApp(argc, argv);
    QGuiApplication::setOrganizationName("ScreenPlay");
    QGuiApplication::setOrganizationDomain("screen-play.app");
    QGuiApplication::setApplicationName("ScreenPlay");
    QGuiApplication::setApplicationVersion(QString(SCREENPLAY_VERSION));
    QGuiApplication::setQuitOnLastWindowClosed(false);

    // --tester-port=N starts a chuck_tester WebSocket automation server bound
    // to the main window. Intended for UI test runs only; production launches
    // omit the flag and the server never starts.
    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();
    QCommandLineOption testerPortOption(
        "tester-port",
        "Enable the chuck_tester automation server on the given TCP port.",
        "port");
    parser.addOption(testerPortOption);
    QCommandLineOption isolatedAppdataOption(
        "isolated-appdata",
        "Redirect writable app data (profiles.json, logs) to Qt's test-mode "
        "directories so test runs never touch the user's real profile.");
    parser.addOption(isolatedAppdataOption);
    parser.process(qtGuiApp);

    if (parser.isSet(isolatedAppdataOption)) {
        // Must run before anything resolves QStandardPaths (GlobalVariables,
        // LoggingHandler). QSettings (registry) is unaffected on purpose: the
        // user's content-storage path keeps working, only profile state is
        // isolated.
        const QString realDataPath = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
        QStandardPaths::setTestModeEnabled(true);
        const QString testDataPath = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
        QDir().mkpath(testDataPath);
        // Mirror this machine's license into the sandbox - without it the app
        // degrades to the basic version, which disables timeline editing (the
        // main UI-test surface). Same machine, same license; profiles.json
        // and logs stay isolated.
        const QString realLicense = realDataPath + "/license.json";
        if (QFile::exists(realLicense)) {
            QFile::remove(testDataPath + "/license.json");
            QFile::copy(realLicense, testDataPath + "/license.json");
        }
        qInfo() << "Isolated appdata:" << testDataPath;
    }
    quint16 testerPort = 0;
    if (parser.isSet(testerPortOption)) {
        bool ok = false;
        const int requested = parser.value(testerPortOption).toInt(&ok);
        if (ok && requested > 0 && requested < 65536) {
            testerPort = static_cast<quint16>(requested);
        } else {
            qWarning() << "Ignoring invalid --tester-port value:" << parser.value(testerPortOption);
        }
    }

    // Tests opt out of the single-instance guard by passing --tester-port; they
    // need their own isolated process that does not piggy-back on a running app.
    if (testerPort == 0 && isAnotherScreenPlayInstanceRunning()) {
        return -5;
    }
    auto logging = std::make_unique<const ScreenPlayCore::LoggingHandler>("ScreenPlay");

    QQuickStyle::setStyle("Material");
    auto engine = std::make_shared<QQmlApplicationEngine>();
    // First access triggers App::create(), which attaches the engine internally.
    (void)engine->singletonInstance<App*>("ScreenPlay", "App");
    engine->loadFromModule("ScreenPlay", "ScreenPlayMain");

    std::unique_ptr<tester::TesterServer> testerServer;
    if (testerPort != 0) {
        QQuickWindow* testerWindow = nullptr;
        for (QObject* root : engine->rootObjects()) {
            if (auto* w = qobject_cast<QQuickWindow*>(root)) {
                testerWindow = w;
                break;
            }
        }
        if (!testerWindow) {
            qWarning() << "tester: no QQuickWindow root, automation server disabled";
        } else {
            testerServer = std::make_unique<tester::TesterServer>(
                *testerWindow, tester::TesterServer::Config { .port = testerPort });
            if (!testerServer->start()) {
                qWarning() << "tester: failed to start on port" << testerPort;
                testerServer.reset();
            }
        }
    }

    const int status = qtGuiApp.exec();
    logging.reset();
    return status;
}
