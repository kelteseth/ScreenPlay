#pragma once

#include <QJsonValue>
#include <QLocalServer>
#include <QLocalSocket>
#include <QObject>
#include <QTimer>
#include <QVector>

#include <memory>

/*!
    \class SDKConnector
    \brief Used for every Wallpaper, Scene or Widget communication via Windows pipes/QLocalSocket

*/

namespace ScreenPlay {
class SDKConnection;

using std::make_unique,
    std::unique_ptr,
    std::shared_ptr,
    std::make_shared;

class SDKConnector : public QObject {
    Q_OBJECT
public:
    explicit SDKConnector(QObject* parent = nullptr);

public slots:
    void readyRead();
    void newConnection();
    void closeAllConnections();

    void closeWallpapersAt(int at);
    void setWallpaperValue(QString appID, QString key, QString value);
    void setSceneValue(QString appID, QString key, QString value);

private:
    unique_ptr<QLocalServer> m_server;
    QVector<shared_ptr<SDKConnection>> m_clients;
};

class SDKConnection : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString appID READ appID WRITE setAppID NOTIFY appIDChanged)
    Q_PROPERTY(QVector<int> monitor READ monitor WRITE setMonitor NOTIFY monitorChanged)

public:
    explicit SDKConnection(QLocalSocket* socket, QObject* parent = nullptr)
        : QObject(parent)
    {

        m_socket = socket;
        connect(m_socket, &QLocalSocket::readyRead, this, &SDKConnection::readyRead);
        connect(m_socket, &QLocalSocket::disconnected, this, &SDKConnection::close);
    }
    ~SDKConnection()
    {

        // We need to call this manually because
        // sometimes it wont close the connection in
        // the descructor
        m_socket->disconnect();
        m_socket->disconnectFromServer();
    }

    QString appID() const
    {
        return m_appID;
    }

    QLocalSocket* socket() const;

    QVector<int> monitor() const
    {
        return m_monitor;
    }

signals:
    void requestCloseAt(int at);
    void appIDChanged(QString appID);
    void monitorChanged(QVector<int> monitor);

public slots:
    void readyRead()
    {

        auto msg = QString(m_socket->readAll());

        // The first message allways contains the appID
        if (msg.startsWith("appID=")) {
            //Only use the first 32 chars for the appID
            m_appID = msg.remove("appID=").mid(0, 32);
            msg.remove(m_appID);
            qDebug() << "###### Wallpaper width APPID created:"
                     << "\n######" << m_appID;
        } else {
            qDebug() << "### Message from: " << m_appID << "\n###" << msg;
        }
    }

    void close()
    {
        if ((m_socket->state()) == QLocalSocket::UnconnectedState || (m_socket->state()) == QLocalSocket::ClosingState)
            return;

        m_socket->disconnectFromServer();
        m_socket->close();

        qDebug() << "### Destroy APPID:\t " << m_appID << " State: " << m_socket->state();
    }

    void setAppID(QString appID)
    {
        if (m_appID == appID)
            return;

        m_appID = appID;
        emit appIDChanged(m_appID);
    }

    void setMonitor(QVector<int> monitor)
    {
        if (m_monitor == monitor)
            return;

        m_monitor = monitor;
        emit monitorChanged(m_monitor);
    }

private:
    QLocalSocket* m_socket;
    QString m_appID;
    QVector<int> m_monitor;
};
}
