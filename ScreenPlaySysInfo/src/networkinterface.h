// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#pragma once

#include <QNetworkInterface>
#include <QObject>
#include <QQmlEngine>
#include <QString>
#include <QTimer>

// Forward declare Windows-specific types
#ifdef Q_OS_WIN
typedef unsigned long NET_IFINDEX;
#endif

class NetworkInterface : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString name READ name NOTIFY statsChanged)
    Q_PROPERTY(qint64 bytesReceived READ bytesReceived NOTIFY statsChanged)
    Q_PROPERTY(qint64 bytesSent READ bytesSent NOTIFY statsChanged)
    Q_PROPERTY(double downloadSpeed READ downloadSpeed NOTIFY statsChanged)
    Q_PROPERTY(double uploadSpeed READ uploadSpeed NOTIFY statsChanged)
    Q_PROPERTY(bool isActive READ isActive NOTIFY statsChanged)
    QML_ELEMENT

public:
    explicit NetworkInterface(QObject* parent = nullptr);
    ~NetworkInterface() override;
    NetworkInterface(const NetworkInterface&) = delete;
    NetworkInterface& operator=(const NetworkInterface&) = delete;

    QString name() const { return m_name; }
    qint64 bytesReceived() const { return m_bytesReceived; }
    qint64 bytesSent() const { return m_bytesSent; }
    double downloadSpeed() const { return m_downloadSpeed; }
    double uploadSpeed() const { return m_uploadSpeed; }
    bool isActive() const { return m_isActive; }

signals:
    void statsChanged();

private slots:
    void updateStats();

private:
    QString findMainInterface();

    QString m_name;
    QString m_interfaceIndex;
    QNetworkInterface m_currentInterface;
    qint64 m_bytesReceived { 0 };
    qint64 m_bytesSent { 0 };
    qint64 m_lastBytesReceived { 0 };
    qint64 m_lastBytesSent { 0 };
    double m_downloadSpeed { 0.0 };
    double m_uploadSpeed { 0.0 };
    bool m_isActive { false };
    QTimer m_updateTimer;
    qint64 m_lastUpdateTime { 0 };

#ifdef Q_OS_WIN
    NET_IFINDEX m_ifIndex { 0 };
#endif
};
