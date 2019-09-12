#pragma once

#include <QGuiApplication>
#include <QObject>
#include <QPoint>
#include <QProcess>

#include "globalvariables.h"
#include "installedlistmodel.h"
#include "monitorlistmodel.h"
#include "profilelistmodel.h"
#include "projectfile.h"
#include "projectsettingslistmodel.h"
#include "screenplaywallpaper.h"
#include "screenplaywidget.h"
#include "sdkconnector.h"
#include "util.h"

#include <memory>

/*!
    \class ScreenPlay
    \brief Used for Creation of Wallpaper, Scenes and Widgets
*/

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
        QObject* parent = nullptr);

    int activeWallpaperCounter() const
    {
        return m_activeWallpaperCounter;
    }

    int activeWidgetsCounter() const
    {
        return m_activeWidgetsCounter;
    }

    void increaseActiveWidgetsCounter()
    {
        m_activeWidgetsCounter++;
        emit activeWidgetsCounterChanged(m_activeWidgetsCounter);
    }

    void decreaseActivewidgetsCounter()
    {
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

signals:
    void projectSettingsListModelFound(ProjectSettingsListModel* li, const QString& type);
    void projectSettingsListModelNotFound();
    void activeWallpaperCounterChanged(int activeWallpaperCounter);
    void activeWidgetsCounterChanged(int activeWidgetsCounter);

public slots:
    void createWallpaper(
        QVector<int> monitorIndex,
        const QString& absoluteStoragePath,
        const QString& previewImage,
        const float volume,
        const QString& fillMode,
        const QString& type);

    void createWidget(const QUrl& absoluteStoragePath, const QString& previewImage);

    void closeAllConnections();
    void requestProjectSettingsListModelAt(const int index);
    void setWallpaperValue(const int index, const QString& key, const QString& value);
    void setAllWallpaperValue(const QString& key, const QString& value);
    void removeWallpaperAt(const int at = 0);
    void monitorListChanged();
    void closeWallpaper(const QVector<int> screenNumber);

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

private:
    void loadActiveProfiles();
    bool saveConfigWallpaper(const QString& profileName, const QVector<int>& monitors, const QJsonObject& content);

private:
    const shared_ptr<GlobalVariables>& m_globalVariables;
    const shared_ptr<MonitorListModel>& m_monitorListModel;
    const shared_ptr<SDKConnector>& m_sdkconnector;

    QVector<shared_ptr<ScreenPlayWallpaper>> m_screenPlayWallpapers;
    QVector<shared_ptr<ScreenPlayWidget>> m_screenPlayWidgets;
    int m_activeWallpaperCounter { 0 };
    int m_activeWidgetsCounter { 0 };
};

}
