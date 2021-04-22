/****************************************************************************
**
** Copyright (C) 2020 Elias Steurer (Kelteseth)
** Contact: https://screen-play.app
**
** This file is part of ScreenPlay. ScreenPlay is licensed under a dual license in
** order to ensure its sustainability. When you contribute to ScreenPlay
** you accept that your work will be available under the two following licenses:
**
** $SCREENPLAY_BEGIN_LICENSE$
**
** #### Affero General Public License Usage (AGPLv3)
** Alternatively, this file may be used under the terms of the GNU Affero
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file "ScreenPlay License.md" included in the
** packaging of this App. Please review the following information to
** ensure the GNU Affero Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/agpl-3.0.en.html.
**
** #### Commercial License
** This code is owned by Elias Steurer. By changing/adding to the code you agree to the
** terms written in:
**  * Legal/corporate_contributor_license_agreement.md - For corporate contributors
**  * Legal/individual_contributor_license_agreement.md - For individual contributors
**
** #### Additional Limitations to the AGPLv3 and Commercial Lincese
** This License does not grant any rights in the trademarks,
** service marks, or logos.
**
**
** $SCREENPLAY_END_LICENSE$
**
****************************************************************************/

#pragma once

#include <QApplication>
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

#include "ScreenPlayUtil/util.h"
#include "globalvariables.h"
#include "util.h"

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
    void requestCloseAt(int at);
    void appIDChanged(QString appID);
    void monitorChanged(QVector<int> monitor);
    void typeChanged(QString type);
    void requestDecreaseWidgetCount();
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
