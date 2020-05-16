#pragma once

#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QLocalServer>
#include <QLocalSocket>
#include <QObject>
#include <QPluginLoader>
#include <QQuickItem>
#include <QSharedDataPointer>
#include <QSharedPointer>
#include <QTimer>
#include <QtGlobal>

class ScreenPlaySDK : public QQuickItem {
    Q_OBJECT
    Q_DISABLE_COPY(ScreenPlaySDK)

public:
    ScreenPlaySDK(QQuickItem* parent = nullptr);
    ScreenPlaySDK( const QString& appID, const QString& type,QQuickItem* parent = nullptr);
    ~ScreenPlaySDK();

    Q_PROPERTY(QString type READ type WRITE setType NOTIFY typeChanged)
    Q_PROPERTY(bool isConnected READ isConnected WRITE setIsConnected NOTIFY isConnectedChanged)
    Q_PROPERTY(QString appID READ appID WRITE setAppID NOTIFY appIDChanged)

    QString type() const
    {
        return m_type;
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
    void readyRead();
    void error(QLocalSocket::LocalSocketError socketError);
    void redirectMessage(QByteArray& msg);

    void setType(QString type)
    {
        if (m_type == type)
            return;

        m_type = type;
        emit typeChanged(m_type);
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
    }

    static void redirectMessageOutputToMainWindow(QtMsgType type, const QMessageLogContext& context, const QString& msg);

signals:
    void incommingMessage(QString key, QString value);
    void incommingMessageError(QString msg);

    void sdkConnected();
    void sdkDisconnected();

    void typeChanged(QString type);
    void isConnectedChanged(bool isConnected);

    void appIDChanged(QString appID);
    void newRedirectMessage(QByteArray& msg);

private:
    void init();

private:
    QLocalSocket m_socket;

    QString m_type = "undefined";
    bool m_isConnected = false;

    QString m_appID;
};
