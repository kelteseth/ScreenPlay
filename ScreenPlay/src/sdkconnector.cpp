#include "sdkconnector.h"
#include "sdkconnection.h"

namespace ScreenPlay {

/*!
    \class ScreenPlay::SDKConnector
    \inmodule ScreenPlay
    \brief SDKConnector is used for the QLocalSocket connection between ScreenPlay and the standalone ScreenPlayWallpaper and ScreenPlayWidget executables.

    To deal with crashes in faulty widgets and wallpaper we open an external application. The communication is done via the ScreenPlaySDK subproject.
    To identify a incomming connection we use the appID. When a new connection is established we save the QLocalSocket and wait for the first message.
    If the message contains "appID=MyUniqueKeyA-Z_a-z_0-9,type=qmlWallpaper" we cant map the socket to a wallpaper/widget.
*/

/*!
    \brief Starts the QLocalServer with the pipename ScreenPlay.
*/
SDKConnector::SDKConnector(QObject* parent)
    : QObject(parent)
    , m_server { std::make_unique<QLocalServer>() }
{

    if (isAnotherScreenPlayInstanceRunning()) {
        m_isAnotherScreenPlayInstanceRunning = true;
        return;
    }

    connect(m_server.get(), &QLocalServer::newConnection, this, &SDKConnector::newConnection);
    m_server->setSocketOptions(QLocalServer::WorldAccessOption);
    if (!m_server->listen("ScreenPlay")) {
        qCritical("Could not open Local Socket with the name ScreenPlay!");
    }
}

/*!
    \brief Checks if another ScreenPlay instance is running by trying to connect to a pipe
    with the name ScreenPlay.
    If successful we send a raise command and quit via m_isAnotherScreenPlayInstanceRunning = true.
*/
bool SDKConnector::isAnotherScreenPlayInstanceRunning()
{
    QLocalSocket socket;
    socket.connectToServer("ScreenPlay");

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

/*!
    \brief Appends a new SDKConnection object shared_ptr to the m_clients list.
*/
void SDKConnector::newConnection()
{
    auto connection = std::make_shared<SDKConnection>(m_server->nextPendingConnection());
    // Because user can close widgets by pressing x the widgets must send us the event
    QObject::connect(connection.get(), &SDKConnection::requestDecreaseWidgetCount, this, &SDKConnector::requestDecreaseWidgetCount);
    QObject::connect(connection.get(), &SDKConnection::requestRaise, this, &SDKConnector::requestRaise);
    // Only after we receive the first message with appID and type we can set the shared reference to the
    // ScreenPlayWallpaper or ScreenPlayWidgets class
    QObject::connect(connection.get(), &SDKConnection::appConnected, this, [this](const SDKConnection* connection) {
        for (const auto& client : m_clients) {
            if (client.get() == connection) {
                emit appConnected(client);
                return;
            }
        }
    });
    m_clients.append(connection);
}

/*!
    \brief Closes all m_clients connections and clears the QVector.
*/
void SDKConnector::closeAllConnections()
{
    for (auto& client : m_clients) {
        client->close();
    }
    m_clients.clear();
    m_clients.squeeze();
}

/*!
 \brief Closes all wallpaper connection with the following type:
 \list
    \li videoWallpaper
    \li qmlWallpaper
    \li htmlWallpaper
    \li godotWallpaper
 \endlist
*/
void SDKConnector::closeAllWallpapers()
{
    closeConntectionByType(GlobalVariables::getAvailableWallpaper());
}

/*!
 \brief Closes all widgets connection with the following type:
 \list
    \li qmlWidget
    \li htmlWidget
    \li standaloneWidget
 \endlist
*/
void SDKConnector::closeAllWidgets()
{
    closeConntectionByType(GlobalVariables::getAvailableWidgets());
}

/*!
  \brief Closes a connection by type. Used only by closeAllWidgets() and closeAllWallpapers()
*/
void SDKConnector::closeConntectionByType(const QStringList& list)
{
    for (auto& client : m_clients) {
        if (list.contains(client->type(), Qt::CaseInsensitive)) {
            client->close();
            m_clients.removeOne(client);
        }
    }
}

/*!
  \brief Closes a wallpaper by the given \a appID.
*/
bool SDKConnector::closeWallpaper(const QString& appID)
{
    for (auto& client : m_clients) {
        if (client->appID() == appID) {
            client->close();
            m_clients.removeOne(client);
            return true;
        }
    }
    return false;
}

/*!
   \brief Sets a given \a value to a given \a key. The \a appID is used to identify the receiver socket.
*/
void SDKConnector::setWallpaperValue(QString appID, QString key, QString value)
{

    for (int i = 0; i < m_clients.count(); ++i) {
        if (m_clients.at(i)->appID() == appID) {
            QJsonObject obj;
            obj.insert(key, QJsonValue(value));

            QByteArray send = QJsonDocument(obj).toJson();
            m_clients.at(i)->socket()->write(send);
            m_clients.at(i)->socket()->waitForBytesWritten();
        }
    }
}

void SDKConnector::replace(const QString& appID, const QJsonObject& obj)
{
    for (int i = 0; i < m_clients.count(); ++i) {
        if (m_clients.at(i)->appID() == appID) {

            QByteArray send = QJsonDocument(obj).toJson();
            m_clients.at(i)->socket()->write(send);
            m_clients.at(i)->socket()->waitForBytesWritten();
        }
    }
}

}
