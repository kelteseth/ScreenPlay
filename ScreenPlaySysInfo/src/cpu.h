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
    static QVariantMap getCPUInfo()
    {
        QVariantMap result;
        QAxObject* wmiService = nullptr;
        QAxObject* objService = nullptr;
        QAxObject* collection = nullptr;
        QAxObject* item = nullptr;

        try {
            // Initialize COM with different threading model
            HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
            if (FAILED(hr)) {
                if (hr == RPC_E_CHANGED_MODE) {
                    // If threading mode was already set differently, try reinitializing
                    CoUninitialize();
                    hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
                }
                if (FAILED(hr)) {
                    qWarning() << "Failed to initialize COM:" << hr;
                    return result;
                }
            }

            // Set general COM security levels
            hr = CoInitializeSecurity(
                nullptr,
                -1, // COM authentication
                nullptr, // Authentication services
                nullptr, // Reserved
                RPC_C_AUTHN_LEVEL_DEFAULT, // Default authentication
                RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation
                nullptr, // Authentication info
                EOAC_NONE, // Additional capabilities
                nullptr // Reserved
            );

            if (FAILED(hr) && hr != RPC_E_TOO_LATE) {
                qWarning() << "Failed to initialize security:" << hr;
                CoUninitialize();
                return result;
            }

            wmiService = new QAxObject("WbemScripting.SWbemLocator");
            if (!wmiService || wmiService->isNull()) {
                qWarning() << "Failed to create WbemScripting.SWbemLocator";
                CoUninitialize();
                return result;
            }

            objService = wmiService->querySubObject("ConnectServer(QString&,QString&)",
                QString("."), QString("root\\CIMV2"));
            if (!objService || objService->isNull()) {
                qWarning() << "Failed to connect to WMI";
                CoUninitialize();
                return result;
            }

            // Modified query to ensure we get the correct information
            QString query = "SELECT Name, NumberOfCores, NumberOfLogicalProcessors, "
                            "MaxClockSpeed, L2CacheSize, L3CacheSize, SocketDesignation "
                            "FROM Win32_Processor";

            collection = objService->querySubObject("ExecQuery(QString&)", query);
            if (!collection || collection->isNull()) {
                qWarning() << "Failed to execute WMI query";
                CoUninitialize();
                return result;
            }

            QAxObject* count = collection->querySubObject("Count");
            if (!count || count->isNull()) {
                qWarning() << "Failed to get item count";
                CoUninitialize();
                return result;
            }

            int itemCount = count->property("Value").toInt();
            count->deleteLater();

            if (itemCount > 0) {
                item = collection->querySubObject("ItemIndex(0)");
                if (item && !item->isNull()) {
                    // Map the properties with correct names
                    result["Name"] = item->property("Name");
                    result["NumberOfCores"] = item->property("NumberOfCores");
                    result["ThreadCount"] = item->property("NumberOfLogicalProcessors");
                    result["MaxClockSpeed"] = item->property("MaxClockSpeed");
                    result["L2CacheSize"] = item->property("L2CacheSize");
                    result["L3CacheSize"] = item->property("L3CacheSize");
                    result["SocketDesignation"] = item->property("SocketDesignation");
                }
            }

        } catch (const std::exception& e) {
            qWarning() << "Exception in WMI query:" << e.what();
        } catch (...) {
            qWarning() << "Unknown exception in WMI query";
        }

        // Cleanup
        if (item)
            item->deleteLater();
        if (collection)
            collection->deleteLater();
        if (objService)
            objService->deleteLater();
        if (wmiService)
            wmiService->deleteLater();

        CoUninitialize();
        return result;
    }
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
    int m_tickRate = 100;
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
