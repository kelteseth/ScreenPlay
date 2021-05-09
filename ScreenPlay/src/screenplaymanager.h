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
#include <QObject>
#include <QPoint>
#include <QProcess>
#include <QtWebSockets/QWebSocket>

#include "ScreenPlayUtil/projectfile.h"
#include "globalvariables.h"
#include "installedlistmodel.h"
#include "monitorlistmodel.h"
#include "profilelistmodel.h"
#include "projectsettingslistmodel.h"
#include "screenplaywallpaper.h"
#include "screenplaywidget.h"
#include "settings.h"
#include "util.h"

#include <memory>
#include <optional>

namespace ScreenPlay {

class ScreenPlayManager : public QObject {
    Q_OBJECT

    Q_PROPERTY(int activeWallpaperCounter READ activeWallpaperCounter WRITE setActiveWallpaperCounter NOTIFY activeWallpaperCounterChanged)
    Q_PROPERTY(int activeWidgetsCounter READ activeWidgetsCounter WRITE setActiveWidgetsCounter NOTIFY activeWidgetsCounterChanged)

public:
    explicit ScreenPlayManager(QObject* parent = nullptr);

    void init(
        const std::shared_ptr<GlobalVariables>& globalVariables,
        const std::shared_ptr<MonitorListModel>& mlm,
        const std::shared_ptr<Settings>& settings);

    int activeWallpaperCounter() const { return m_activeWallpaperCounter; }
    int activeWidgetsCounter() const { return m_activeWidgetsCounter; }
    bool isAnotherScreenPlayInstanceRunning() { return m_isAnotherScreenPlayInstanceRunning; }

signals:
    void activeWallpaperCounterChanged(int activeWallpaperCounter);
    void activeWidgetsCounterChanged(int activeWidgetsCounter);
    void monitorConfigurationChanged();

    void projectSettingsListModelResult(ScreenPlay::ProjectSettingsListModel* li = nullptr);
    void requestSaveProfiles();
    void requestRaise();
    void profilesSaved();
    void displayErrorPopup(const QString& msg);

private slots:
    bool saveProfiles();

public slots:
    // moc needs full enum namespace info see QTBUG-58454
    bool createWallpaper(
        const ScreenPlay::InstalledType::InstalledType type,
        const ScreenPlay::FillMode::FillMode fillMode,
        const QString& absoluteStoragePath,
        const QString& previewImage,
        const QString& file,
        const QVector<int>& monitorIndex,
        const float volume,
        const float playbackRate,
        const QJsonObject& properties,
        const bool saveToProfilesConfigFile);

    bool createWidget(
        const ScreenPlay::InstalledType::InstalledType type,
        const QPoint& position,
        const QString& absoluteStoragePath,
        const QString& previewImage,
        const QJsonObject& properties,
        const bool saveToProfilesConfigFile);

    bool removeAllWallpapers();
    bool removeAllWidgets();
    bool removeWallpaperAt(const int index);

    bool requestProjectSettingsAtMonitorIndex(const int index);
    bool setWallpaperValueAtMonitorIndex(const int index, const QString& key, const QString& value);
    bool setAllWallpaperValue(const QString& key, const QString& value);
    bool setWallpaperValue(const QString& appID, const QString& key, const QString& value);
    ScreenPlayWallpaper* getWallpaperByAppID(const QString& appID) const;

    void newConnection();

    void setActiveWallpaperCounter(int activeWallpaperCounter)
    {
        if (m_activeWallpaperCounter == activeWallpaperCounter)
            return;

        m_activeWallpaperCounter = activeWallpaperCounter;
        emit activeWallpaperCounterChanged(m_activeWallpaperCounter);
    }

    void setActiveWidgetsCounter(int activeWidgetsCounter)
    {
        if (m_activeWidgetsCounter == activeWidgetsCounter)
            return;

        m_activeWidgetsCounter = activeWidgetsCounter;
        emit activeWidgetsCounterChanged(m_activeWidgetsCounter);
    }

    void increaseActiveWidgetsCounter()
    {
        m_activeWidgetsCounter++;
        emit activeWidgetsCounterChanged(m_activeWidgetsCounter);
    }

    void decreaseActiveWidgetsCounter()
    {
        if (m_activeWidgetsCounter <= 0) {
            return;
        }
        m_activeWidgetsCounter--;
        emit activeWidgetsCounterChanged(m_activeWidgetsCounter);
    }

    void increaseActiveWallpaperCounter()
    {
        m_activeWallpaperCounter++;
        emit activeWallpaperCounterChanged(m_activeWallpaperCounter);
    }

    void decreaseActiveWallpaperCounter()
    {
        if (m_activeWallpaperCounter <= 0) {
            return;
        }
        m_activeWallpaperCounter--;
        emit activeWallpaperCounterChanged(m_activeWallpaperCounter);
    }

private:
    bool loadProfiles();
    bool checkIsAnotherScreenPlayInstanceRunning();
    bool removeWallpaper(const QString& appID);
    bool removeWidget(const QString& appID);

private:
    std::shared_ptr<GlobalVariables> m_globalVariables;
    std::shared_ptr<MonitorListModel> m_monitorListModel;
    std::shared_ptr<Settings> m_settings;
    std::unique_ptr<QLocalServer> m_server;
    std::unique_ptr<QWebSocketServer> m_websocketServer;

    QVector<std::shared_ptr<ScreenPlayWallpaper>> m_screenPlayWallpapers;
    QVector<std::shared_ptr<ScreenPlayWidget>> m_screenPlayWidgets;
    std::vector<std::unique_ptr<SDKConnection>> m_unconnectedClients;

    int m_activeWallpaperCounter { 0 };
    int m_activeWidgetsCounter { 0 };

    bool m_isAnotherScreenPlayInstanceRunning = false;
    QTimer m_saveLimiter;

    const quint16 m_webSocketPort = 16395;
};

}
