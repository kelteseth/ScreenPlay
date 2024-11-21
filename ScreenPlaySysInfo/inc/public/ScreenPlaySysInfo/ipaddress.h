// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#pragma once

#include <QObject>
#include <QQmlEngine>
#include <QString>
#include <QStringList>
#include <QUrl>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkInterface>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

class IpAddress : public QObject {
    Q_OBJECT
    Q_PROPERTY(QStringList privateIpV4AddressList READ privateIpV4AddressList WRITE setPrivateIpV4AddressList NOTIFY privateIpV4AddressListChanged)
    Q_PROPERTY(QStringList privateIpV6AddressList READ privateIpV6AddressList WRITE setPrivateIpV6AddressList NOTIFY privateIpV6AddressListChanged)
    Q_PROPERTY(QString publicIpV4Address READ publicIpV4Address WRITE setPublicIpV4Address NOTIFY publicIpV4AddressChanged)
    Q_PROPERTY(QString publicIpV6Address READ publicIpV6Address WRITE setPublicIpV6Address NOTIFY publicIpV6AddressChanged)
    QML_ELEMENT

public:
    explicit IpAddress(QObject* parent = nullptr);

    const QStringList& privateIpV4AddressList() const { return m_privateIpV4AddressList; }
    const QStringList& privateIpV6AddressList() const { return m_privateIpV6AddressList; }
    const QString& publicIpV4Address() const { return m_publicIpV4Address; }
    const QString& publicIpV6Address() const { return m_publicIpV6Address; }

public slots:
    void setPrivateIpV4AddressList(const QStringList& privateIpV4AddressList)
    {
        if (m_privateIpV4AddressList == privateIpV4AddressList)
            return;
        m_privateIpV4AddressList = privateIpV4AddressList;
        emit privateIpV4AddressListChanged(privateIpV4AddressList);
    }

    void setPrivateIpV6AddressList(const QStringList& privateIpV6AddressList)
    {
        if (m_privateIpV6AddressList == privateIpV6AddressList)
            return;
        m_privateIpV6AddressList = privateIpV6AddressList;
        emit privateIpV6AddressListChanged(m_privateIpV6AddressList);
    }

    void setPublicIpV4Address(const QString& publicIpV4Address)
    {
        if (m_publicIpV4Address == publicIpV4Address)
            return;
        m_publicIpV4Address = publicIpV4Address;
        emit publicIpV4AddressChanged(m_publicIpV4Address);
    }

    void setPublicIpV6Address(const QString& publicIpV6Address)
    {
        if (m_publicIpV6Address == publicIpV6Address)
            return;
        m_publicIpV6Address = publicIpV6Address;
        emit publicIpV6AddressChanged(m_publicIpV6Address);
    }

signals:
    void privateIpV4AddressListChanged(const QStringList& privateIpAddressesList);
    void privateIpV6AddressListChanged(const QStringList& privateIpV6AddressList);
    void publicIpV4AddressChanged(const QString& publicIpV4Address);
    void publicIpV6AddressChanged(const QString& publicIpV6Address);

private:
    QNetworkAccessManager m_networkAccessManager;
    QStringList m_privateIpV4AddressList;
    QStringList m_privateIpV6AddressList;
    QString m_publicIpV4Address = "UNKNOWN";
    QString m_publicIpV6Address = "UNKNOWN";
    const QString m_ipV4hostCheck = "https://ipv4.icanhazip.com/";
    const QString m_ipV6hostCheck = "https://ipv6.icanhazip.com/";
};
