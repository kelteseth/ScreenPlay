// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#pragma once
#include <QDebug>
#include <QObject>
#include <QQmlEngine>
#include <QString>
#include <QTimer>
#include <QVariantMap>

#ifdef Q_OS_WIN
#include <objbase.h>

#include <QAxObject>
#include <QSettings>
#include <qt_windows.h>
#pragma comment(lib, "wbemuuid.lib")

class WMIHelper {
public:
    static QVariantMap getCPUInfo();
};
#endif

class CPU : public QObject {
    Q_OBJECT
    Q_PROPERTY(float usage READ usage NOTIFY usageChanged)
    Q_PROPERTY(int tickRate READ tickRate WRITE setTickRate NOTIFY tickRateChanged)
    Q_PROPERTY(QString name READ name NOTIFY cpuInfoChanged)
    Q_PROPERTY(QString description READ description NOTIFY cpuInfoChanged)
    Q_PROPERTY(int architecture READ architecture NOTIFY cpuInfoChanged)
    Q_PROPERTY(int l2CacheSize READ l2CacheSize NOTIFY cpuInfoChanged)
    Q_PROPERTY(int l3CacheSize READ l3CacheSize NOTIFY cpuInfoChanged)
    Q_PROPERTY(QString socketDesignation READ socketDesignation NOTIFY cpuInfoChanged)
    Q_PROPERTY(int maxClockSpeed READ maxClockSpeed NOTIFY cpuInfoChanged)
    Q_PROPERTY(int numberOfCores READ numberOfCores NOTIFY cpuInfoChanged)
    Q_PROPERTY(int threadCount READ threadCount NOTIFY cpuInfoChanged)
    QML_ELEMENT

public:
    explicit CPU(QObject* parent = nullptr);

    float usage() const { return m_usage; }
    int tickRate() const { return m_tickRate; }
    QString name() const { return m_name; }
    QString description() const { return m_description; }
    int architecture() const { return m_architecture; }
    int l2CacheSize() const { return m_l2CacheSize; }
    int l3CacheSize() const { return m_l3CacheSize; }
    QString socketDesignation() const { return m_socketDesignation; }
    int maxClockSpeed() const { return m_maxClockSpeed; }
    int numberOfCores() const { return m_numberOfCores; }
    int threadCount() const { return m_threadCount; }

signals:
    void usageChanged(float usage);
    void tickRateChanged(int tickRate);
    void cpuInfoChanged();

public slots:
    void update();
    void setUsage(float usage);
    void setTickRate(int tickRate);
    void refreshCPUInfo();

private:
    float m_usage = 0.0f;
    int m_tickRate = 1000;
    QString m_name;
    QString m_description;
    int m_architecture = 0;
    int m_l2CacheSize = 0;
    int m_l3CacheSize = 0;
    QString m_socketDesignation;
    int m_maxClockSpeed = 0;
    int m_numberOfCores = 0;
    int m_threadCount = 0;

    uint64_t lastIdleTime = 0;
    uint64_t lastKernelTime = 0;
    uint64_t lastUserTime = 0;
    QTimer m_updateTimer;

    void updateCPUInfo();
};
