// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#pragma once

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
#include <memory>
#include <optional>

namespace ScreenPlay {

class ScreenPlayManager : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")

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
    bool setWallpaperFillModeAtMonitorIndex(const int index, const int fillmode);
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
    QVector<QWebSocket*> m_connections;

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
