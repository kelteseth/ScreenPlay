#pragma once

#include <QObject>
#include <QProcess>

#include <memory>

#include "projectsettingslistmodel.h"
#include "settings.h"

namespace ScreenPlay {
using std::shared_ptr,
    std::make_shared,
    std::vector,
    std::move;

class ScreenPlayWallpaper final : public QObject {
    Q_OBJECT

    Q_PROPERTY(vector<int> screenNumber READ screenNumber WRITE setScreenNumber NOTIFY screenNumberChanged)
    Q_PROPERTY(QString projectPath READ projectPath WRITE setProjectPath NOTIFY projectPathChanged)
    Q_PROPERTY(QString previewImage READ previewImage WRITE setPreviewImage NOTIFY previewImageChanged)
    Q_PROPERTY(QString appID READ appID WRITE setAppID NOTIFY appIDChanged)
    Q_PROPERTY(QString type READ type WRITE setType NOTIFY typeChanged)

public:
    explicit ScreenPlayWallpaper(const vector<int>& screenNumber,
        const shared_ptr<Settings>& settings,
        const QString& appID,
        const QString& projectPath,
        const QString& previewImage,
        const float volume,
        const QString& fillMode,
        const QString& type,
        QObject* parent = nullptr);

    ~ScreenPlayWallpaper() {}

    const shared_ptr<ProjectSettingsListModel>& projectSettingsListModel() const
    {
        return m_projectSettingsListModel;
    }

    vector<int> screenNumber() const
    {
        return m_screenNumber;
    }

    QString projectPath() const
    {
        return m_projectPath;
    }

    QString previewImage() const
    {
        return m_previewImage;
    }

    QString appID() const
    {
        return m_appID;
    }

    QString type() const
    {
        return m_type;
    }

signals:
    void screenNumberChanged(vector<int> screenNumber);
    void projectPathChanged(QString projectPath);
    void previewImageChanged(QString previewImage);
    void appIDChanged(QString appID);
    void typeChanged(QString type);

public slots:
    void setScreenNumber(vector<int> screenNumber)
    {
        if (m_screenNumber == screenNumber)
            return;

        m_screenNumber = screenNumber;
        emit screenNumberChanged(m_screenNumber);
    }

    void setProjectPath(QString projectPath)
    {
        if (m_projectPath == projectPath)
            return;

        m_projectPath = projectPath;
        emit projectPathChanged(m_projectPath);
    }

    void setPreviewImage(QString previewImage)
    {
        if (m_previewImage == previewImage)
            return;

        m_previewImage = previewImage;
        emit previewImageChanged(m_previewImage);
    }

    void setAppID(QString appID)
    {
        if (m_appID == appID)
            return;

        m_appID = appID;
        emit appIDChanged(m_appID);
    }

    void setType(QString type)
    {
        if (m_type == type)
            return;

        m_type = type;
        emit typeChanged(m_type);
    }

private:
    QProcess m_process;
    shared_ptr<ProjectSettingsListModel> m_projectSettingsListModel;
    vector<int> m_screenNumber;
    QString m_projectPath;
    QString m_previewImage;
    QString m_appID;
    QString m_type;
    const shared_ptr<Settings> m_settings;
};
}
