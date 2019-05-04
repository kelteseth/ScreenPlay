#pragma once

#include <QCryptographicHash>
#include <QGuiApplication>
#include <QObject>
#include <QPoint>
#include <QProcess>
#include <QRandomGenerator>
#include <QSharedPointer>

#include "installedlistmodel.h"
#include "monitorlistmodel.h"
#include "projectfile.h"
#include "projectsettingslistmodel.h"
#include "sdkconnector.h"
#include "settings.h"

/*!
    \class ScreenPlay
    \brief Used for Creation of Wallpaper, Scenes and Widgets
*/

class ScreenPlayWallpaper;
class ScreenPlayWidget;

// convenience types
using RefSPWall = QSharedPointer<ScreenPlayWallpaper>;
using RefSPWidget = QSharedPointer<ScreenPlayWidget>;

class ScreenPlay final : public QObject {
    Q_OBJECT
private:
    InstalledListModel* const m_ilm { nullptr };
    Settings* const m_settings { nullptr };
    MonitorListModel* const m_mlm { nullptr };
    QGuiApplication* const m_qGuiApplication { nullptr };
    SDKConnector* const m_sdkc { nullptr };
    std::vector<RefSPWall> m_screenPlayWallpaperList;
    std::vector<RefSPWidget> m_screenPlayWidgetList;

public:
    // constructor(s)
    explicit ScreenPlay(
        InstalledListModel* ilm, Settings* set,
        MonitorListModel* mlm, SDKConnector* sdkc,
        QObject* parent = nullptr);

    // copy and move disabled(for now) : remember rule of 1/3/5
    Q_DISABLE_COPY(ScreenPlay)
    ScreenPlay(ScreenPlay&&) = delete;
    ScreenPlay& operator=(ScreenPlay&&) = delete;

    // destructor
    ~ScreenPlay();

    // getters
    const InstalledListModel* listModelInstalled() const noexcept;
    const Settings* settings() const noexcept;
    const MonitorListModel* monitorListModel() const noexcept;
    const QGuiApplication* guiApp() const noexcept;
    const SDKConnector* sdkConnector() const noexcept;
    const std::vector<RefSPWall>& spWallList() const noexcept;
    const std::vector<RefSPWidget>& spWidgetList() const noexcept;

signals:
    void allWallpaperRemoved() const;
    void projectSettingsListModelFound(ProjectSettingsListModel* li, const QString& type) const;
    void projectSettingsListModelNotFound() const;

public slots:
    void createWallpaper(
        const int monitorIndex, QUrl absoluteStoragePath,
        const QString& previewImage, const float volume,
        const QString& fillMode, const QString& type);
    void createWidget(QUrl absoluteStoragePath, const QString& previewImage);
    void removeAllWallpaper() noexcept;
    void requestProjectSettingsListModelAt(const int index) const noexcept;
    void setWallpaperValue(const int at, const QString& key, const QString& value) noexcept;
    void setAllWallpaperValue(const QString& key, const QString& value) noexcept;
    void removeWallpaperAt(const int at = 0);
    std::vector<int> getMonitorByAppID(const QString& appID) const;
    QString generateID() const;
};
