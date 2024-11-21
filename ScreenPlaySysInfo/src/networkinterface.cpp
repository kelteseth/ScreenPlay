// networkinterface.cpp
#include "networkinterface.h"
#include <QDateTime>
#include <algorithm>

#ifdef Q_OS_WIN
#include "networkinterface_win.h"
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")
#endif

NetworkInterface::NetworkInterface(QObject* parent)
    : QObject { parent }
{
    m_interfaceIndex = findMainInterface();
    if (!m_interfaceIndex.isEmpty()) {
        m_name = m_interfaceIndex;
        qDebug() << "Monitoring interface:" << m_name;
    }

    connect(&m_updateTimer, &QTimer::timeout, this, &NetworkInterface::updateStats);
    m_updateTimer.start(1000);
    m_lastUpdateTime = QDateTime::currentMSecsSinceEpoch();

    updateStats();
}

NetworkInterface::~NetworkInterface() = default;

QString NetworkInterface::findMainInterface()
{
    const auto interfaces = QNetworkInterface::allInterfaces();

    // Helper lambda to check if interface name contains any virtual/tunnel keywords
    const auto isVirtualInterface = [](const QString& name) {
        const auto virtualKeywords = { "TAP", "Tunnel", "VPN", "Virtual", "Tailscale" };
        return std::any_of(virtualKeywords.begin(), virtualKeywords.end(),
            [&name](const char* keyword) {
                return name.contains(QString(keyword), Qt::CaseInsensitive);
            });
    };

    // Helper lambda to check if interface has IPv4
    const auto hasIPv4 = [](const QNetworkInterface& iface) {
        const auto& addresses = iface.addressEntries();
        return std::any_of(addresses.begin(), addresses.end(),
            [](const QNetworkAddressEntry& addr) {
                return addr.ip().protocol() == QAbstractSocket::IPv4Protocol;
            });
    };

    // First try to find a physical Ethernet interface
    auto ethernetIface = std::find_if(interfaces.begin(), interfaces.end(),
        [&](const QNetworkInterface& iface) {
            const auto flags = iface.flags();
            const auto name = iface.humanReadableName();

            return flags.testFlag(QNetworkInterface::IsUp) && !flags.testFlag(QNetworkInterface::IsLoopBack) && !flags.testFlag(QNetworkInterface::IsPointToPoint) && iface.type() == QNetworkInterface::Ethernet && hasIPv4(iface) && !isVirtualInterface(name);
        });

    if (ethernetIface != interfaces.end()) {
        const auto name = ethernetIface->humanReadableName();
        qDebug() << "Found physical Ethernet interface:" << name;
        m_currentInterface = *ethernetIface;
#ifdef Q_OS_WIN
        m_ifIndex = ethernetIface->index();
        qDebug() << "Interface index:" << m_ifIndex;
#endif
        return name;
    }

    // If no Ethernet, try Wi-Fi
    auto wifiIface = std::find_if(interfaces.begin(), interfaces.end(),
        [&](const QNetworkInterface& iface) {
            const auto flags = iface.flags();
            const auto name = iface.humanReadableName();

            return flags.testFlag(QNetworkInterface::IsUp) && !flags.testFlag(QNetworkInterface::IsLoopBack) && !flags.testFlag(QNetworkInterface::IsPointToPoint) && hasIPv4(iface) && !isVirtualInterface(name) && (name.contains("Wi-Fi", Qt::CaseInsensitive) || name.contains("Wireless", Qt::CaseInsensitive));
        });

    if (wifiIface != interfaces.end()) {
        const auto name = wifiIface->humanReadableName();
        qDebug() << "Found Wi-Fi interface:" << name;
        m_currentInterface = *wifiIface;
#ifdef Q_OS_WIN
        m_ifIndex = wifiIface->index();
        qDebug() << "Interface index:" << m_ifIndex;
#endif
        return name;
    }

    qWarning() << "No suitable network interface found";
    return {};
}

void NetworkInterface::updateStats()
{
#ifdef Q_OS_WIN
    ULONG64 newBytesReceived = 0;
    ULONG64 newBytesSent = 0;
    bool interfaceUp = false;

    if (NetworkInterfaceImpl::getInterfaceStats(m_ifIndex, newBytesReceived, newBytesSent, interfaceUp)) {
        const auto currentTime = QDateTime::currentMSecsSinceEpoch();
        const auto timeDiff = currentTime - m_lastUpdateTime;

        // Update speeds
        if (timeDiff > 0 && m_lastUpdateTime > 0) {
            m_downloadSpeed = static_cast<double>(newBytesReceived - m_lastBytesReceived) * 1000.0 / timeDiff;
            m_uploadSpeed = static_cast<double>(newBytesSent - m_lastBytesSent) * 1000.0 / timeDiff;

            // Debug output for speeds
            // if (m_downloadSpeed > 0 || m_uploadSpeed > 0) {
            //     qDebug() << "Download speed:" << (m_downloadSpeed / 1024.0 / 1024.0) << "MB/s"
            //              << "Upload speed:" << (m_uploadSpeed / 1024.0 / 1024.0) << "MB/s";
            // }
        }

        // Update totals
        m_bytesReceived = newBytesReceived;
        m_bytesSent = newBytesSent;
        m_lastBytesReceived = newBytesReceived;
        m_lastBytesSent = newBytesSent;
        m_lastUpdateTime = currentTime;
        m_isActive = interfaceUp;

        emit statsChanged();
    }
#endif

    // Also check interface validity
    auto qtinterface = QNetworkInterface::interfaceFromName(m_currentInterface.name());
    if (!qtinterface.isValid()) {
        qWarning() << "Interface became invalid:" << m_interfaceIndex;
        m_interfaceIndex.clear();
        m_ifIndex = 0;
    }
}
