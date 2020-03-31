#pragma once

#include <QGuiApplication>
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

using std::shared_ptr,
    std::unique_ptr,
    std::make_shared,
    std::make_unique,
    std::vector,
    std::size_t,
    std::remove_if;

class ScreenPlayManager : public QObject {
    Q_OBJECT

    Q_PROPERTY(int activeWallpaperCounter READ activeWallpaperCounter WRITE setActiveWallpaperCounter NOTIFY activeWallpaperCounterChanged)
    Q_PROPERTY(int activeWidgetsCounter READ activeWidgetsCounter WRITE setActiveWidgetsCounter NOTIFY activeWidgetsCounterChanged)

public:
    explicit ScreenPlayManager(
        const shared_ptr<GlobalVariables>& globalVariables,
        const shared_ptr<MonitorListModel>& mlm,
        const shared_ptr<SDKConnector>& sdkc,
        const shared_ptr<GAnalytics>& telemetry,
        const shared_ptr<Settings>& settings,
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
    void projectSettingsListModelFound(ProjectSettingsListModel* li, const QString& type);
    void projectSettingsListModelNotFound();
    void activeWallpaperCounterChanged(int activeWallpaperCounter);
    void activeWidgetsCounterChanged(int activeWidgetsCounter);

public slots:
    void createWallpaper(QVector<int> monitorIndex,
        const QString& absoluteStoragePath,
        const QString& previewImage,
        const float volume,
        const QString& fillMode,
        const QString& type, const bool saveToProfilesConfigFile = true);

    void createWidget(
        const QUrl& absoluteStoragePath,
        const QString& previewImage,
        const QString& type);

    void removeAllWallpapers();
    void removeAllWidgets();
    bool removeWallpaperAt(const int at = 0);

    void requestProjectSettingsListModelAt(const int index);
    void setWallpaperValue(const int index, const QString& key, const QString& value);
    void setAllWallpaperValue(const QString& key, const QString& value);
    std::optional<shared_ptr<ScreenPlayWallpaper>> getWallpaperByAppID(const QString& appID);

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
    void loadWallpaperProfiles();
    bool saveWallpaperProfile(const QString& profileName, const QJsonObject& content);

private:
    const shared_ptr<GlobalVariables>& m_globalVariables;
    const shared_ptr<MonitorListModel>& m_monitorListModel;
    const shared_ptr<SDKConnector>& m_sdkconnector;
    const shared_ptr<GAnalytics>& m_telemetry;
    const shared_ptr<Settings>& m_settings;

    QVector<shared_ptr<ScreenPlayWallpaper>> m_screenPlayWallpapers;
    QVector<shared_ptr<ScreenPlayWidget>> m_screenPlayWidgets;
    int m_activeWallpaperCounter { 0 };
    int m_activeWidgetsCounter { 0 };
};

}
