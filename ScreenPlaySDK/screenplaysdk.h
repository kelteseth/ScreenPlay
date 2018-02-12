#pragma once

#include <QQuickItem>
#include <QObject>
#include <QLocalSocket>
#include <QLocalServer>
#include <QSharedPointer>
#include <QSharedDataPointer>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QByteArray>
#include <QTimer>

class ScreenPlaySDK : public QQuickItem
{
    Q_OBJECT
    Q_DISABLE_COPY(ScreenPlaySDK)

public:
    ScreenPlaySDK(QQuickItem *parent = nullptr);
    ~ScreenPlaySDK();

    Q_PROPERTY(QString contentType READ contentType WRITE setContentType NOTIFY contentTypeChanged)
    Q_PROPERTY(bool isConnected READ isConnected WRITE setIsConnected NOTIFY isConnectedChanged)
    QString contentType() const
    {
        return m_contentType;
    }

    bool isConnected() const
    {
        return m_isConnected;
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

        if(isConnected()){
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

signals:
    void incommingMessage(QString msg);
    void incommingMessageError(QString msg);


    void sdkConnected();
    void sdkDisconnected();
    void sdkSocketError(QString type);

    void contentTypeChanged(QString contentType);

    void isConnectedChanged(bool isConnected);

private:
    QSharedPointer<QLocalSocket> m_socket;

    QString m_contentType = "undefined";
    bool m_isConnected = false;
};

