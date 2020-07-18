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
#include <QTimer>
#include <QVector>

#include <memory>

#include "globalvariables.h"
#include "util.h"

/*!
    \class SDKConnector
    \brief Used for every Wallpaper, Scene or Widget communication via Windows pipes/QLocalSocket

*/

namespace ScreenPlay {
class SDKConnection;

class SDKConnector : public QObject {
    Q_OBJECT

public:
    explicit SDKConnector(QObject* parent = nullptr);

    bool m_isAnotherScreenPlayInstanceRunning { false };

signals:
    void requestDecreaseWidgetCount();
    void requestRaise();
    void appConnected(const std::shared_ptr<SDKConnection>& connection);

public slots:
    void newConnection();
    void closeConntectionByType(const QStringList& list);
    void closeAllConnections();
    void closeAllWallpapers();
    void closeAllWidgets();
    bool closeWallpaper(const QString& appID);
    void setWallpaperValue(QString appID, QString key, QString value);
    void replace(const QString& appID, const QJsonObject& obj);

private:
    std::unique_ptr<QLocalServer> m_server;
    QVector<std::shared_ptr<SDKConnection>> m_clients;
    bool isAnotherScreenPlayInstanceRunning();
};

}
