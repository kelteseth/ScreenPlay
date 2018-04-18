#pragma once

#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QLocalServer>
#include <QLocalSocket>
#include <QObject>
#include <QQuickItem>
#include <QSharedDataPointer>
#include <QSharedPointer>
#include <QTimer>

class ScreenPlaySDK : public QQuickItem {
    Q_OBJECT
    Q_DISABLE_COPY(ScreenPlaySDK)

public:
    ScreenPlaySDK(QQuickItem* parent = nullptr);
    ~ScreenPlaySDK();

    Q_PROPERTY(QString contentType READ contentType WRITE setContentType NOTIFY contentTypeChanged)
    Q_PROPERTY(bool isConnected READ isConnected WRITE setIsConnected NOTIFY isConnectedChanged)
    Q_PROPERTY(QString appID READ appID WRITE setAppID NOTIFY appIDChanged)

    QString contentType() const
    {
        return m_contentType;
    }

    bool isConnected() const
    {
        return m_isConnected;
    }

    QString appID() const
    {
        return m_appID;
    }

public slots:
    void connected();
    void disconnected();
    void bytesWritten(qint64 bytes);
    void readyRead();
    void error(QLocalSocket::LocalSocketError socketError);

    void setContentType(QString contentType)
    {
        if (m_contentType == contentType)
            return;

        m_contentType = contentType;

        if (isConnected()) {
            m_socket.data()->write(QByteArray(m_contentType.toLatin1()));
            m_socket.data()->flush();
            m_socket.data()->waitForBytesWritten();
        }
        emit contentTypeChanged(m_contentType);
    }

    void setIsConnected(bool isConnected)
    {
        if (m_isConnected == isConnected)
            return;

        m_isConnected = isConnected;
        emit isConnectedChanged(m_isConnected);
    }

    void setAppID(QString appID)
    {
        if (m_appID == appID)
            return;

        m_appID = appID;
        emit appIDChanged(m_appID);

        m_socket.data()->write(QByteArray(appID.toUtf8()));
        m_socket.data()->waitForBytesWritten();
    }

signals:
    void incommingMessage(QString key, QString value);
    void incommingMessageError(QString msg);

    void sdkConnected();
    void sdkDisconnected();
    void sdkSocketError(QString type);

    void contentTypeChanged(QString contentType);
    void isConnectedChanged(bool isConnected);

    void appIDChanged(QString appID);

private:
    QSharedPointer<QLocalSocket> m_socket;

    QString m_contentType = "undefined";
    bool m_isConnected = false;

    QString m_appID;
};
