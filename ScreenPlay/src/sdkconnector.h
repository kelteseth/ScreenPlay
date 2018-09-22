#pragma once

#include <QJsonValue>
#include <QLocalServer>
#include <QLocalSocket>
#include <QObject>
#include <QSharedPointer>
#include <QTimer>
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
    void readyRead();
    void newConnection();
    void closeAllWallpapers();
    void closeWallpapersAt(int at);
    void setWallpaperValue(QString appID, QString key, QString value);
    void setSceneValue(QString appID, QString key, QString value);

private:
    QSharedPointer<QLocalServer> m_server;
    QVector<QSharedPointer<SDKConnection>> m_clients;

};

class SDKConnection : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString appID READ appID WRITE setAppID NOTIFY appIDChanged)
    Q_PROPERTY(QVector<int> monitor READ monitor WRITE setMonitor NOTIFY monitorChanged)

public:
    explicit SDKConnection(QLocalSocket* socket, QObject* parent = nullptr)
    {

        m_socket = socket;
        connect(m_socket, &QLocalSocket::readyRead, this, &SDKConnection::readyRead);
        connect(m_socket, &QLocalSocket::disconnected, this, &SDKConnection::disconnected);
    }
    ~SDKConnection()
    {
        qDebug() << "destroying SDKConnection Object";
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

        QString msg = QString(m_socket->readAll());

        // The first message allways contains the appID
        if (msg.startsWith("appID=")) {
            //Only use the first 32 chars for the appID
            m_appID = msg.remove("appID=").mid(0,32);
            msg.remove(m_appID);

            qDebug() << "### APPID:\t "<< m_appID << "\n### Monitor: "<< m_monitor.at(0);
        }
        qDebug() << "SDK CONNECTOR "<< msg;
    }

    void disconnected()
    {
        close();
    }

    void close()
    {
        m_socket->disconnect();
        m_socket->close();
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
