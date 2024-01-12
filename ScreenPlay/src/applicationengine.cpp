#include "ScreenPlay/applicationengine.h"
#include "ScreenPlay/CMakeVariables.h"
#include "ScreenPlay/app.h"
#include "ScreenPlay/applicationengine.h"

#include <QIcon>
#include <QQuickStyle>

namespace ScreenPlay {

ApplicationEngine::ApplicationEngine(QObject* parent)
    : QQmlApplicationEngine { parent }
{
    QGuiApplication::setWindowIcon(QIcon(":/qml/ScreenPlayApp/assets/icons/app.ico"));
    QGuiApplication::setOrganizationName("ScreenPlay");
    QGuiApplication::setOrganizationDomain("screen-play.app");
    QGuiApplication::setApplicationName("ScreenPlay");
    QGuiApplication::setApplicationVersion(QString(SCREENPLAY_VERSION));
    QGuiApplication::setQuitOnLastWindowClosed(false);
}

/*!
    \brief Checks if another ScreenPlay instance is running by trying to connect to a pipe
    with the name ScreenPlay.
    If successful we send a raise command and quit via m_isAnotherScreenPlayInstanceRunning = true.
*/

bool ScreenPlay::ApplicationEngine::isAnotherScreenPlayInstanceRunning()
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

void ApplicationEngine::init()
{
    auto* guiAppInst = dynamic_cast<QGuiApplication*>(QGuiApplication::instance());
    addImportPath(guiAppInst->applicationDirPath() + "/qml");
    guiAppInst->addLibraryPath(guiAppInst->applicationDirPath() + "/qml");

    QQuickStyle::setStyle("Material");
    load(QUrl(QStringLiteral("qrc:/qml/ScreenPlayApp/main.qml")));

    App* singleton = singletonInstance<App*>("ScreenPlayApp", "App");
    singleton->setEngine(this);
}
}
