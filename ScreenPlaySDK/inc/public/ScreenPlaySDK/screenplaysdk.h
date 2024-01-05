// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#pragma once

#include "ScreenPlayUtil/processmanager.h"
#include <QByteArray>
#include <QJsonArray>
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

class ScreenPlaySDK : public QObject {
    Q_OBJECT

public:
    ScreenPlaySDK(const QString& appID, const QString& type);
    ~ScreenPlaySDK();
    Q_PROPERTY(QString type READ type WRITE setType NOTIFY typeChanged)
    Q_PROPERTY(bool isConnected READ isConnected WRITE setIsConnected NOTIFY isConnectedChanged)
    Q_PROPERTY(QString appID READ appID WRITE setAppID NOTIFY appIDChanged)
    Q_PROPERTY(qint64 mainAppPID READ mainAppPID WRITE setMainAppPID NOTIFY mainAppPIDChanged FINAL)

    QString type() const { return m_type; }
    bool isConnected() const { return m_isConnected; }
    QString appID() const { return m_appID; }

    qint64 mainAppPID() const;
    void setMainAppPID(qint64 mainAppPID);

public slots:
    void sendMessage(const QJsonObject& obj);
    void connected();
    void disconnected();
    void readyRead();
    void error(QLocalSocket::LocalSocketError socketError);
    void redirectMessage(QByteArray& msg);
    void pingAlive();
    void start();

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

    void replaceWallpaper(
        const QString absolutePath,
        const QString file,
        const float volume,
        const QString fillMode,
        const QString type,
        const bool checkWallpaperVisible);

    void mainAppPIDChanged(qint64 mainAppPID);

private:
    QLocalSocket m_socket;

    QString m_type;
    bool m_isConnected = false;

    QString m_appID;
    QTimer m_pingAliveTimer;
    qint64 m_mainAppPID { 0 };
    ScreenPlay::ProcessManager m_processManager;
};
