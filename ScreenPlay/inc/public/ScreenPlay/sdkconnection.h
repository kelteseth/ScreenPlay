// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#pragma once

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QLocalServer>
#include <QLocalSocket>
#include <QObject>
#include <QString>
#include <QTimer>
#include <QVector>
#include <QWebSocketServer>

#include "ScreenPlay/globalvariables.h"
#include "ScreenPlay/util.h"
#include "ScreenPlayUtil/util.h"

#include <memory>

namespace ScreenPlay {

/*!
    \class SDKConnection
    \brief

*/

class SDKConnection : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString appID READ appID WRITE setAppID NOTIFY appIDChanged)
    Q_PROPERTY(QString type READ type WRITE setType NOTIFY typeChanged)
    Q_PROPERTY(QVector<int> monitor READ monitor WRITE setMonitor NOTIFY monitorChanged)

public:
    /*!
        SDKConnection.
    */
    explicit SDKConnection(QLocalSocket* socket, QObject* parent = nullptr);

    QString appID() const { return m_appID; }
    QLocalSocket* socket() const { return m_socket; }
    QVector<int> monitor() const { return m_monitor; }
    QString type() const { return m_type; }

signals:
    void disconnected();
    void requestCloseAt(int at);
    void appIDChanged(QString appID);
    void monitorChanged(QVector<int> monitor);
    void typeChanged(QString type);
    void requestRaise();
    void appConnected(const ScreenPlay::SDKConnection* connection);
    void jsonMessageReceived(const QJsonObject obj);
    void pingAliveReceived();

public slots:
    void readyRead();
    bool sendMessage(const QByteArray& message);
    bool close();

    void setAppID(QString appID)
    {
        if (m_appID == appID)
            return;

        m_appID = appID;
        emit appIDChanged(m_appID);
    }

    void setMonitor(QVector<int> monitor)
    {
        if (m_monitor == monitor)
            return;

        m_monitor = monitor;
        emit monitorChanged(m_monitor);
    }

    void setType(QString type)
    {
        if (m_type == type)
            return;

        m_type = type;
        emit typeChanged(m_type);
    }

private:
    QLocalSocket* m_socket { nullptr };
    QString m_appID;
    QVector<int> m_monitor;
    QString m_type;
};
}
