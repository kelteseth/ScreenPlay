#pragma once

#include <QLocalServer>
#include <QLocalSocket>
#include <QObject>
#include <QSharedPointer>
#include <QTimer>
#include <QJsonValue>
#include <QVector>

/*!
    \class SDKConnector
    \brief Used for every Wallpaper, Scene or Widget communication via Windows pipes/QLocalSocket

*/
class SDKConnection;

class SDKConnector : public QObject {
    Q_OBJECT
public:
    explicit SDKConnector(QObject* parent = nullptr);

signals:

public slots:
    void newConnection();
    void closeAllWallpapers();
    void setWallpaperValue(QString appID, QString key, QString value);

private:
    QSharedPointer<QLocalServer> m_server;
    QVector<QSharedPointer<SDKConnection>> m_clients;
};



class SDKConnection : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString appID READ appID WRITE setAppID NOTIFY appIDChanged)
public:
    explicit SDKConnection(QLocalSocket* socket, QObject* parent = nullptr)
    {
        m_socket = socket;
        connect(m_socket, &QLocalSocket::readyRead, this, &SDKConnection::readyRead);
        connect(m_socket, &QLocalSocket::disconnected, this, &SDKConnection::disconnected);

    }

    void setSocket(QLocalSocket* socket);

    QString appID() const
    {
        return m_appID;
    }

    QLocalSocket *socket() const;

signals:
    void requestCloseAt(int at);
    void appIDChanged(QString appID);

public slots:
    void readyRead()
    {
        QString msg = QString(m_socket->readAll());
        msg.contains("appID=");
        m_appID = msg;
    }

    void disconnected(){
        close();
    }
    void close()
    {
        m_socket->close();
    }

    void setAppID(QString appID)
    {
        if (m_appID == appID)
            return;

        m_appID = appID;
        emit appIDChanged(m_appID);
    }

private:
    QLocalSocket* m_socket;
    QString m_appID;
};
