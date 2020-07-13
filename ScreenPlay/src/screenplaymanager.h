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

#include "ganalytics.h"
#include "globalvariables.h"
#include "installedlistmodel.h"
#include "monitorlistmodel.h"
#include "profilelistmodel.h"
#include "projectfile.h"
#include "projectsettingslistmodel.h"
#include "screenplaywallpaper.h"
#include "screenplaywidget.h"
#include "sdkconnector.h"
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
    explicit ScreenPlayManager(
        const std::shared_ptr<GlobalVariables>& globalVariables,
        const std::shared_ptr<MonitorListModel>& mlm,
        const std::shared_ptr<SDKConnector>& sdkc,
        const std::shared_ptr<GAnalytics>& telemetry,
        const std::shared_ptr<Settings>& settings,
        QObject* parent = nullptr);

    int activeWallpaperCounter() const
    {
        return m_activeWallpaperCounter;
    }

    int activeWidgetsCounter() const
    {
        return m_activeWidgetsCounter;
    }

signals:
    void projectSettingsListModelResult(
        const bool found,
        ProjectSettingsListModel* li = nullptr,
        const ScreenPlay::InstalledType::InstalledType type = ScreenPlay::InstalledType::InstalledType::VideoWallpaper);
    void activeWallpaperCounterChanged(int activeWallpaperCounter);
    void activeWidgetsCounterChanged(int activeWidgetsCounter);

public slots:
    // moc needs full enum namespace info see QTBUG-58454
    void createWallpaper(
        const ScreenPlay::InstalledType::InstalledType type,
        const ScreenPlay::FillMode::FillMode fillMode,
        const QString& absoluteStoragePath,
        const QString& previewImage,
        const QString& file,
        QVector<int> monitorIndex,
        const float volume,
        const bool saveToProfilesConfigFile);

    void createWidget(const ScreenPlay::InstalledType::InstalledType type, const QPoint &position,
        const QString& absoluteStoragePath,
        const QString& previewImage,
        const bool saveToProfilesConfigFile);

    void removeAllWallpapers();
    void removeAllWidgets();
    bool removeWallpaperAt(const int index);

    void requestProjectSettingsAtMonitorIndex(const int index);
    void setWallpaperValueAtMonitorIndex(const int index, const QString& key, const QString& value);
    void setAllWallpaperValue(const QString& key, const QString& value);
    std::optional<std::shared_ptr<ScreenPlayWallpaper>> getWallpaperByAppID(const QString& appID);

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
    void loadProfiles();
    bool saveProfiles();
    [[nodiscard]] bool removeWallpaperByAppID(const QString& appID);

private:
    const std::shared_ptr<GlobalVariables>& m_globalVariables;
    const std::shared_ptr<MonitorListModel>& m_monitorListModel;
    const std::shared_ptr<SDKConnector>& m_sdkconnector;
    const std::shared_ptr<GAnalytics>& m_telemetry;
    const std::shared_ptr<Settings>& m_settings;

    QVector<std::shared_ptr<ScreenPlayWallpaper>> m_screenPlayWallpapers;
    QVector<std::shared_ptr<ScreenPlayWidget>> m_screenPlayWidgets;
    int m_activeWallpaperCounter { 0 };
    int m_activeWidgetsCounter { 0 };
};

}
