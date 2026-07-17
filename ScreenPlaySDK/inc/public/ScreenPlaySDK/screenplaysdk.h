// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#pragma once

#include <QJsonObject>
#include <QLocalSocket>
#include <QObject>
#include <QTimer>
#include <QtGlobal>

#include "ScreenPlayCore/ipcframebuffer.h"
#include "ScreenPlayCore/processmanager.h"

class ScreenPlaySDK : public QObject {
    Q_OBJECT

public:
    ScreenPlaySDK(const QString& appID, const QString& type, QObject* parent = nullptr);
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
    void redirectMessage(const QByteArray& msg);
    void pingAlive();
    void start();

    void setType(const QString& type);
    void setIsConnected(bool isConnected);
    void setAppID(const QString& appID);

signals:
    void incomingMessage(const QString& key, const QString& value);
    void incomingMessageError(const QString& msg);

    void sdkConnected();
    void sdkDisconnected();

    void typeChanged(const QString& type);
    void isConnectedChanged(bool isConnected);

    void appIDChanged(const QString& appID);

    void replaceWallpaper(
        const QString& absolutePath,
        const QString& file,
        float volume,
        const QString& fillMode,
        const QString& type,
        bool checkWallpaperVisible,
        const QJsonObject& wallpaperProperties);

    void mainAppPIDChanged(qint64 mainAppPID);

private:
    static void messageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg);
    static std::atomic<ScreenPlaySDK*> s_instance;

    QLocalSocket m_socket;

    QString m_type;
    bool m_isConnected = false;

    QString m_appID;
    QTimer m_pingAliveTimer;
    qint64 m_mainAppPID { 0 };
    ScreenPlay::ProcessManager m_processManager;
    ScreenPlay::IpcFrameBuffer m_frameBuffer;
};
