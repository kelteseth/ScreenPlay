#include "sdkconnector.h"

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
    Starts the QLocalServer with the pipename ScreenPlay.
 */
SDKConnector::SDKConnector(QObject* parent)
    : QObject(parent)
    , m_server { make_unique<QLocalServer>() }
{

    connect(m_server.get(), &QLocalServer::newConnection, this, &SDKConnector::newConnection);
    if (!m_server->listen("ScreenPlay")) {
        qCritical("Could not open Local Socket with the name ScreenPlay. Usually this means another ScreenPlay instance is running!");
    }
}

void SDKConnector::readyRead()
{
}

/*!
    Appends a new SDKConnection object shared_ptr to the m_clients list.
*/
void SDKConnector::newConnection()
{
    m_clients.append(make_shared<SDKConnection>(m_server->nextPendingConnection()));
}

/*!
 Closes all m_clients connections and clears the QVector.
*/
void SDKConnector::closeAllConnections()
{
    for (auto& client : m_clients) {
        client->close();
    }
    m_clients.clear();
}

/*!
 Closes all wallpaper connection with the following type:
 \list
    \li videoWallpaper
    \li qmlWallpaper
    \li htmlWallpaper
    \li godotWallpaper
 \endlist
*/
void SDKConnector::closeAllWallpapers()
{
    QStringList types {
        "videoWallpaper",
        "qmlWallpaper",
        "htmlWallpaper",
        "godotWallpaper"
    };

    for (auto& client : m_clients) {
        if (types.contains(client->type())) {
            client->close();
            m_clients.removeOne(client);
        }
    }
}

/*!
 Closes a wallpaper at the given \a index. The native monitor index is used here.
 On Windows the monitor 0 is the main display.
*/
void SDKConnector::closeWallpapersAt(int at)
{
    for (const shared_ptr<SDKConnection>& refSDKConnection : m_clients) {
        refSDKConnection->close();
        if (!refSDKConnection->monitor().empty()) {
            if (refSDKConnection->monitor().at(0) == at) {
                refSDKConnection->close();
                qDebug() << "Wall Closed...!";
            } else {
                qDebug() << "COULD NOT CLOSE!";
            }
        } else {
            qDebug() << "no wp window ";
        }
    }
}

/*!
    Closes a wallpaper by the given \a appID.
*/
void SDKConnector::closeWallpaper(const QString& appID)
{
    for (auto& item : m_clients) {
        if (item->appID() == appID) {
            item->close();
            m_clients.removeOne(item);
            return;
        }
    }
}

/*!
    Sets a given \a value to a given \a key. The \a appID is used to identify the receiver socket.
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

/*!
 Sets a given \a value to a given \a key. The \a appID is used to identify the receiver socket.
*/
void SDKConnector::setSceneValue(QString appID, QString key, QString value)
{
    for (int i = 0; i < m_clients.count(); ++i) {
        if (m_clients.at(i)->appID() == appID) {
            QJsonObject obj;
            obj.insert("type", QJsonValue("qmlScene"));
            obj.insert(key, QJsonValue(value));

            QByteArray send = QJsonDocument(obj).toJson();
            m_clients.at(i)->socket()->write(send);
            m_clients.at(i)->socket()->waitForBytesWritten();
        }
    }
}

}
