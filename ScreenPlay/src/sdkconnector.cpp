#include "sdkconnector.h"

#include <QJsonDocument>
#include <QJsonObject>
namespace ScreenPlay {
SDKConnector::SDKConnector(QObject* parent)
    : QObject(parent)
    , m_server { make_unique<QLocalServer>() }
{

    connect(m_server.get(), &QLocalServer::newConnection, this, &SDKConnector::newConnection);
    if (!m_server->listen("ScreenPlay")) {
        qErrnoWarning("Could not open Local Socket with the name ScreenPlay. Usually this means another ScreenPlay instance is running!");
    }
}

void SDKConnector::readyRead()
{
}

void SDKConnector::newConnection()
{
    m_clients.append(make_shared<SDKConnection>(m_server->nextPendingConnection()));
}

void SDKConnector::closeAllConnections()
{
    for (int i = 0; i < m_clients.size(); ++i) {
        m_clients.at(i)->close();
        m_clients.clear();
        m_clients.squeeze();
    }
}

void SDKConnector::closeWallpapersAt(int at)
{
    for (const shared_ptr<SDKConnection>& refSDKConnection : m_clients) {
        refSDKConnection->close();
        if (!refSDKConnection->monitor().empty()) {
            // problem here !!
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

QLocalSocket* SDKConnection::socket() const
{
    return m_socket;
}
}
