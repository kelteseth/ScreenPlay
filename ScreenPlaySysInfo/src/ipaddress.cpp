#include "ipaddress.h"

IpAddress::IpAddress(QObject* parent)
    : QObject { parent }
{
    const QHostAddress& localhost = QHostAddress(QHostAddress::LocalHost);
    QStringList privateIpV4Addresses;
    QStringList privateIpV6Addresses;
    for (const QHostAddress& address : QNetworkInterface::allAddresses()) {
        if (address == localhost)
            continue;

        if (address.protocol() == QAbstractSocket::IPv4Protocol) {
            privateIpV4Addresses.append(address.toString());
        }
        if (address.protocol() == QAbstractSocket::IPv6Protocol) {
            privateIpV6Addresses.append(address.toString());
        }
    }

    setPrivateIpV4AddressList(privateIpV4Addresses);
    setPrivateIpV6AddressList(privateIpV6Addresses);

    {

        QNetworkReply* replyV4 = m_networkAccessManager.get(QNetworkRequest(m_ipV4hostCheck));
        QObject::connect(replyV4, &QNetworkReply::readyRead, this, [this, replyV4]() {
            const QByteArray data = replyV4->readAll();
            if (data.size() <= 0)
                return;

            QString address(data);
            QHostAddress hostAddress(address);
            setPublicIpV4Address(address);
        });
    }
    {
        QNetworkReply* replyV6 = m_networkAccessManager.get(QNetworkRequest(m_ipV6hostCheck));
        QObject::connect(replyV6, &QNetworkReply::readyRead, this, [this, replyV6]() {
            const QByteArray data = replyV6->readAll();
            if (data.size() <= 0)
                return;

            QString address(data);
            QHostAddress hostAddress(address);
            setPublicIpV6Address(address);
        });
    }
}
