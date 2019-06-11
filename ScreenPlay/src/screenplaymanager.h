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
#include "screenplaywallpaper.h"
#include "screenplaywidget.h"
#include "sdkconnector.h"
#include "settings.h"

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

class ScreenPlayManager final : public QObject {
    Q_OBJECT

public:
    explicit ScreenPlayManager(
        const shared_ptr<InstalledListModel>& ilm,
        const shared_ptr<Settings>& settings,
        const shared_ptr<MonitorListModel>& mlm,
        const shared_ptr<SDKConnector>& sdkc,
        QObject* parent = nullptr);

    ~ScreenPlayManager() {}

signals:
    void allWallpaperRemoved();
    void projectSettingsListModelFound(ProjectSettingsListModel* li, const QString& type);
    void projectSettingsListModelNotFound();

public slots:
    void createWallpaper(
        const int monitorIndex, QUrl absoluteStoragePath,
        const QString& previewImage, const float volume,
        const QString& fillMode, const QString& type);
    void createWidget(QUrl absoluteStoragePath, const QString& previewImage);
    void removeAllWallpaper();
    void requestProjectSettingsListModelAt(const int index);
    void setWallpaperValue(const int at, const QString& key, const QString& value);
    void setAllWallpaperValue(const QString& key, const QString& value);
    void removeWallpaperAt(const int at = 0);
    QString generateID() const;

private:
    const shared_ptr<InstalledListModel> m_installedListModel;
    const shared_ptr<Settings> m_settings;
    const shared_ptr<MonitorListModel> m_monitorListModel;
    const shared_ptr<SDKConnector> m_sdkconnector;

    const QGuiApplication* const m_qGuiApplication;
    vector<unique_ptr<ScreenPlayWallpaper>> m_screenPlayWallpapers;
    vector<unique_ptr<ScreenPlayWidget>> m_screenPlayWidgets;
};

}
