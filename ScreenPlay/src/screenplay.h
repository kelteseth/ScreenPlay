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

class ScreenPlay final: public QObject {
    Q_OBJECT
private:
    InstalledListModel *const m_ilm{nullptr};
    Settings *const           m_settings{nullptr};
    MonitorListModel *const   m_mlm{nullptr};
    QGuiApplication *const    m_qGuiApplication{nullptr};
    SDKConnector *const       m_sdkc{nullptr};
    std::vector<RefSPWall>   m_screenPlayWallpaperList;
    std::vector<RefSPWidget> m_screenPlayWidgetList;

public:
    // constructor(s)
    explicit ScreenPlay(
            InstalledListModel* ilm, Settings* set,
            MonitorListModel* mlm, SDKConnector* sdkc,
            QObject* parent = nullptr);

    // copy and move disable(for now) : remember rule of 1/3/5
    Q_DISABLE_COPY_MOVE(ScreenPlay)

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

/*!
    \class ScreenPlayWallpaper
    \brief Used for ...
*/

class ScreenPlayWallpaper final: public QObject {
    Q_OBJECT

    Q_PROPERTY(std::vector<int> screenNumber READ screenNumber WRITE setScreenNumber NOTIFY screenNumberChanged)
    Q_PROPERTY(QString projectPath READ projectPath WRITE setProjectPath NOTIFY projectPathChanged)
    Q_PROPERTY(QString previewImage READ previewImage WRITE setPreviewImage NOTIFY previewImageChanged)
    Q_PROPERTY(QString appID READ appID WRITE setAppID NOTIFY appIDChanged)
    Q_PROPERTY(QString type READ type WRITE setType NOTIFY typeChanged)

private:
    std::vector<int> m_screenNumber;
    QString m_projectPath;
    QString m_previewImage;
    QString m_type;
    QString m_appID;
    QProcess *m_process;
    QSharedPointer<ProjectSettingsListModel> m_projectSettingsListModel;

public:
    // constructor(s)
    explicit ScreenPlayWallpaper(const std::vector<int>& screenNumber, const QString& projectPath,
                                 const QString& previewImage, const float volume, const QString& fillMode,
                                 const QString& type, ScreenPlay *parent = nullptr);

    // copy and move disable(for now) : remember rule of 1/3/5
    Q_DISABLE_COPY_MOVE(ScreenPlayWallpaper)

    // destructor
    ~ScreenPlayWallpaper();

    // getters
    const std::vector<int>& screenNumber() const noexcept;
    const QString& projectPath() const noexcept;
    const QString& previewImage() const noexcept;
    const QString& type() const noexcept;
    const QString& appID() const noexcept;
    const QSharedPointer<ProjectSettingsListModel>& projectSettingsListModel() const noexcept;

signals:
    void screenNumberChanged(std::vector<int> screenNumber) const;
    void projectPathChanged(QString projectPath) const;
    void previewImageChanged(QString previewImage) const;
    void typeChanged(QString type) const;
    void appIDChanged(QString appID) const;
    //void projectSettingsListModelAt(ProjectSettingsListModel* li); ??? not used

public slots:
    void setScreenNumber(const std::vector<int>& screenNumber) noexcept;
    void setProjectPath(const QString& projectPath) noexcept;
    void setPreviewImage(const QString& previewImage) noexcept;
    void setType(const QString& type) noexcept;
    void setAppID(const QString& appID) noexcept;
};

/*!
    \class ScreenPlayWidget
    \brief Used for ...
*/

class ScreenPlayWidget final: public QObject {
    Q_OBJECT

    Q_PROPERTY(QString projectPath READ projectPath WRITE setProjectPath NOTIFY projectPathChanged)
    Q_PROPERTY(QString fullPath READ fullPath WRITE setFullPath NOTIFY fullPathChanged)
    Q_PROPERTY(QString previewImage READ previewImage WRITE setPreviewImage NOTIFY previewImageChanged)
    Q_PROPERTY(QPoint position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(QString appID READ appID WRITE setAppID NOTIFY appIDChanged)

private:
    QString m_projectPath;
    QString m_previewImage;
    QString m_fullPath;
    QString m_appID;
    QPoint m_position;
    QProcess *m_process;

public:
    // constructor(s)
    explicit ScreenPlayWidget(const QString& projectPath, const QString& previewImage,
                     const QString& fullPath, ScreenPlay *parent = nullptr);

    // copy and move disable(for now) : remember rule of 1/3/5
    Q_DISABLE_COPY_MOVE(ScreenPlayWidget)

    // destructor
    ~ScreenPlayWidget();

    // getters
    const QString& projectPath() const noexcept;
    const QString& previewImage() const noexcept;
    const QString& fullPath() const noexcept;
    const QString& appID() const noexcept;
    const QPoint& position() const noexcept;

signals:
    void projectPathChanged(QString projectPath) const;
    void previewImageChanged(QString previewImage) const;
    void fullPathChanged(QString fullPath) const;
    void appIDChanged(QString appID) const;
    void positionChanged(QPoint position) const;

public slots:
    void setProjectPath(const QString& projectPath) noexcept;
    void setPreviewImage(const QString& previewImage) noexcept;
    void setFullPath(const QString& fullPath) noexcept;
    void setAppID(const QString& appID) noexcept;
    void setPosition(const QPoint& position) noexcept;
};
