#pragma once

#include <QObject>

#include "projectsettingslistmodel.h"

/*!
    \class ScreenPlayWallpaper
    \brief Used for ...
*/

class ScreenPlay;
class QProcess;

class ScreenPlayWallpaper final : public QObject {
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
    QProcess* m_process;
    QSharedPointer<ProjectSettingsListModel> m_projectSettingsListModel;

public:
    // constructor(s)
    explicit ScreenPlayWallpaper(const std::vector<int>& screenNumber, const QString& projectPath,
        const QString& previewImage, const float volume, const QString& fillMode,
        const QString& type, ScreenPlay* parent = nullptr);

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
