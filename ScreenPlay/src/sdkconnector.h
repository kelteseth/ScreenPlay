#pragma once

#include <QApplication>
#include <QJsonDocument>
#include <QJsonObject>
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


class SDKConnector : public QObject {
    Q_OBJECT

public:
    explicit SDKConnector(QObject* parent = nullptr);

    bool m_isAnotherScreenPlayInstanceRunning { false };

signals:
    void requestDecreaseWidgetCount();
    void requestRaise();

public slots:
    void newConnection();
    void closeConntectionByType(const QStringList& list);
    void closeAllConnections();
    void closeAllWallpapers();
    void closeAllWidgets();

    void closeWallpapersAt(int at);
    void closeWallpaper(const QString& appID);
    void setWallpaperValue(QString appID, QString key, QString value);

private:
    std::unique_ptr<QLocalServer> m_server;
    QVector<std::shared_ptr<SDKConnection>> m_clients;
    bool isAnotherScreenPlayInstanceRunning();
};

class SDKConnection : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString appID READ appID WRITE setAppID NOTIFY appIDChanged)
    Q_PROPERTY(QString type READ type WRITE setType NOTIFY typeChanged)
    Q_PROPERTY(QVector<int> monitor READ monitor WRITE setMonitor NOTIFY monitorChanged)

public:
    /*!
        SDKConnection.
    */
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

    QLocalSocket* socket() const
    {
        return m_socket;
    }

    QVector<int> monitor() const
    {
        return m_monitor;
    }

    QString type() const
    {
        return m_type;
    }

signals:
    void requestCloseAt(int at);
    void appIDChanged(QString appID);
    void monitorChanged(QVector<int> monitor);
    void typeChanged(QString type);
    void requestDecreaseWidgetCount();
    void requestRaise();

public slots:
    void readyRead()
    {

        auto msg = QString(m_socket->readAll());

        // The first message allways contains the appID
        if (msg.startsWith("appID=")) {
            QStringList args = msg.split(",");
            //Only use the first 32 chars for the appID
            QString appID = args.at(0);
            m_appID = appID.remove("appID=");

            QStringList types {
                "videoWallpaper",
                "qmlWallpaper",
                "htmlWallpaper",
                "godotWallpaper",

                "qmlWidget",
                "htmlWidget",
                "standaloneWidget"
            };
            for (const QString& type : types) {
                if (msg.contains(type)) {
                    m_type = type;
                }
            }
            qInfo() << "###### Wallpaper created:"
                    << "\t AppID:" << m_appID << "\t type: " << m_type;

        } else if(msg.startsWith("command=")){
            msg.remove("command=");
            if(msg == "requestRaise") {
                qInfo() << "Another ScreenPlay instance reuqested this one to raise!";
                emit requestRaise();
            }
        } else {
            qInfo() << "### Message from: " << m_appID << ": " << msg;
        }
    }

    void close()
    {
        if (m_socket->state() == QLocalSocket::ConnectedState) {
            m_socket->disconnectFromServer();
            m_socket->close();

            qDebug() << "### Destroy APPID:\t " << m_appID << " State: " << m_socket->state();
        }

        if (m_type.contains("widget", Qt::CaseInsensitive)) {
            emit requestDecreaseWidgetCount();
        }
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

    void setType(QString type)
    {
        if (m_type == type)
            return;

        m_type = type;
        emit typeChanged(m_type);
    }

private:
    QLocalSocket* m_socket { nullptr };
    QString m_appID;
    QVector<int> m_monitor;
    QString m_type;
};
}
