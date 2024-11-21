// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#include "cpu.h"
/*!
    \class CPU
    \inmodule ScreenPlaySysInfo
    \brief  .
*/
CPU::CPU(QObject* parent)
    : QObject(parent)
{
    connect(&m_updateTimer, &QTimer::timeout, this, &CPU::update);
    m_updateTimer.start(m_tickRate);
    updateCPUInfo();
}
#ifdef Q_OS_WIN
inline uint64_t FileTimeToInt64(const FILETIME& ft)
{
    ULARGE_INTEGER ul;
    ul.LowPart = ft.dwLowDateTime;
    ul.HighPart = ft.dwHighDateTime;
    return ul.QuadPart;
}
#endif
/*!
 * \brief CPU::update
 */
void CPU::update()
{
#ifdef Q_OS_WIN
    BOOL status;
    FILETIME ftIdleTime, ftKernelTime, ftUserTime;
    // get new CPU's idle/kernel/user time
    status = GetSystemTimes(&ftIdleTime, &ftKernelTime, &ftUserTime);
    if (status == 0)
        return;
    // convert times to uint by appending low and high bits
    uint64_t newIdleTime = FileTimeToInt64(ftIdleTime);
    uint64_t newKernelTime = FileTimeToInt64(ftKernelTime);
    uint64_t newUserTime = FileTimeToInt64(ftUserTime);
    // subtract old times
    uint64_t userTime = newUserTime - lastUserTime;
    uint64_t kernelTime = newKernelTime - lastKernelTime;
    double idleTime = newIdleTime - lastIdleTime;
    // calculate the usage
    double sysTime = kernelTime + userTime;
    float currentUsage = float((sysTime - idleTime) * 100 / sysTime);
    // save the new values
    lastIdleTime = newIdleTime;
    lastKernelTime = newKernelTime;
    lastUserTime = newUserTime;
    float cu = int(currentUsage * 100);
    currentUsage = float(cu / 100);
    // publish result
    setUsage(currentUsage);
#endif
}

void CPU::setUsage(float usage)
{
    if (qFuzzyCompare(m_usage, usage))
        return;
    m_usage = usage;
    emit usageChanged(m_usage);
}

void CPU::setTickRate(int tickRate)
{
    if (m_tickRate == tickRate)
        return;
    qDebug() << "Tick rate changed";
    m_tickRate = tickRate;
    m_updateTimer.setInterval(m_tickRate);
    emit tickRateChanged(m_tickRate);
}

void CPU::refreshCPUInfo()
{
    updateCPUInfo();
}

void CPU::updateCPUInfo()
{
#ifdef Q_OS_WIN
    qDebug() << "Updating CPU info via WMI";
    QVariantMap cpuInfo = WMIHelper::getCPUInfo();

    if (cpuInfo.isEmpty()) {
        qWarning() << "Failed to get CPU info via WMI";

        // Enhanced fallback implementation
        QSettings settings("HKEY_LOCAL_MACHINE\\HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",
            QSettings::NativeFormat);

        m_name = settings.value("ProcessorNameString").toString().trimmed();
        m_maxClockSpeed = settings.value("~MHz").toInt();

        // Use GetLogicalProcessorInformation for accurate core/thread count
        DWORD length = 0;
        SYSTEM_LOGICAL_PROCESSOR_INFORMATION* buffer = nullptr;

        // First call to get required buffer size
        GetLogicalProcessorInformation(nullptr, &length);
        if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
            buffer = (SYSTEM_LOGICAL_PROCESSOR_INFORMATION*)malloc(length);
            if (buffer && GetLogicalProcessorInformation(buffer, &length)) {
                DWORD processorCoreCount = 0;
                DWORD logicalProcessorCount = 0;

                for (DWORD i = 0; i < length / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION); i++) {
                    if (buffer[i].Relationship == RelationProcessorCore) {
                        processorCoreCount++;
                        // Count bits set in ProcessorMask to get logical processors per core
                        ULONG_PTR mask = buffer[i].ProcessorMask;
                        while (mask) {
                            logicalProcessorCount += (mask & 1);
                            mask >>= 1;
                        }
                    }
                }

                m_numberOfCores = processorCoreCount;
                m_threadCount = logicalProcessorCount;
            }
            free(buffer);
        }

        qDebug() << "Fallback CPU info - Name:" << m_name
                 << "Speed:" << m_maxClockSpeed
                 << "Cores:" << m_numberOfCores
                 << "Threads:" << m_threadCount;
    } else {
        // Use WMI data with proper property names
        m_name = cpuInfo["Name"].toString().trimmed();
        m_maxClockSpeed = cpuInfo["MaxClockSpeed"].toInt();
        m_numberOfCores = cpuInfo["NumberOfCores"].toInt();
        m_threadCount = cpuInfo["ThreadCount"].toInt();
        m_l2CacheSize = cpuInfo["L2CacheSize"].toInt();
        m_l3CacheSize = cpuInfo["L3CacheSize"].toInt();
        m_socketDesignation = cpuInfo["SocketDesignation"].toString();

        qDebug() << "WMI CPU info:";
        qDebug() << " - Name:" << m_name;
        qDebug() << " - Cores:" << m_numberOfCores;
        qDebug() << " - Threads:" << m_threadCount;
        qDebug() << " - Speed:" << m_maxClockSpeed;
        qDebug() << " - L2 Cache:" << m_l2CacheSize;
        qDebug() << " - L3 Cache:" << m_l3CacheSize;
        qDebug() << " - Socket:" << m_socketDesignation;
    }

    emit cpuInfoChanged();
#endif
}

#ifdef Q_OS_WIN
QVariantMap WMIHelper::getCPUInfo()
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

#endif

#include "moc_cpu.cpp"
