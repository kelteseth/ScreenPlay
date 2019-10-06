#pragma once

#include <QJsonObject>
#include <QObject>
#include <QProcess>

#include <memory>

#include "globalvariables.h"
#include "projectsettingslistmodel.h"

namespace ScreenPlay {

using std::shared_ptr,
    std::make_shared;

class ScreenPlayWallpaper : public QObject {
    Q_OBJECT

    Q_PROPERTY(QVector<int> screenNumber READ screenNumber WRITE setScreenNumber NOTIFY screenNumberChanged)

    Q_PROPERTY(QString file READ file WRITE setFile NOTIFY fileChanged)
    Q_PROPERTY(QString fillMode READ fillMode WRITE setFillMode NOTIFY fillModeChanged)
    Q_PROPERTY(QString absolutePath READ absolutePath WRITE setAbsolutePath NOTIFY absolutePathChanged)
    Q_PROPERTY(QString previewImage READ previewImage WRITE setPreviewImage NOTIFY previewImageChanged)
    Q_PROPERTY(QString appID READ appID WRITE setAppID NOTIFY appIDChanged)
    Q_PROPERTY(QString type READ type WRITE setType NOTIFY typeChanged)
    Q_PROPERTY(QJsonObject profileJsonObject READ profileJsonObject WRITE setProfileJsonObject NOTIFY profileJsonObjectChanged)

public:
    explicit ScreenPlayWallpaper(
        const QVector<int>& screenNumber,
        const shared_ptr<GlobalVariables>& globalVariables,
        const QString& appID,
        const QString& absolutePath,
        const QString& previewImage,
        const float volume,
        const QString& fillMode,
        const QString& type,
        QObject* parent = nullptr);

    explicit ScreenPlayWallpaper(
        const QVector<int>& screenNumber,
        const shared_ptr<GlobalVariables>& globalVariables,
        const QString& appID,
        const QString& absolutePath,
        const QString& previewImage,
        const QString& type,
        const QJsonObject& profileJsonObject,
        QObject* parent = nullptr);


    const shared_ptr<ProjectSettingsListModel>& projectSettingsListModel() const
    {
        return m_projectSettingsListModel;
    }

    QVector<int> screenNumber() const
    {
        return m_screenNumber;
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

    QString file() const
    {
        return m_file;
    }

    QString fillMode() const
    {
        return m_fillMode;
    }

    QString absolutePath() const
    {
        return m_absolutePath;
    }

    QJsonObject profileJsonObject() const
    {
        return m_profileJsonObject;
    }

signals:
    void screenNumberChanged(QVector<int> screenNumber);
    void previewImageChanged(QString previewImage);
    void appIDChanged(QString appID);
    void typeChanged(QString type);
    void fileChanged(QString file);
    void fillModeChanged(QString fillMode);
    void absolutePathChanged(QString absolutePath);
    void profileJsonObjectChanged(QJsonObject profileJsonObject);

public slots:
    void processExit(int exitCode, QProcess::ExitStatus exitStatus);
    void processError(QProcess::ProcessError error);

    void setScreenNumber(QVector<int> screenNumber)
    {
        if (m_screenNumber == screenNumber)
            return;

        m_screenNumber = screenNumber;
        emit screenNumberChanged(m_screenNumber);
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

    void setFile(QString file)
    {
        if (m_file == file)
            return;

        m_file = file;
        emit fileChanged(m_file);
    }

    void setFillMode(QString fillMode)
    {
        if (m_fillMode == fillMode)
            return;

        m_fillMode = fillMode;
        emit fillModeChanged(m_fillMode);
    }

    void setAbsolutePath(QString absolutePath)
    {
        if (m_absolutePath == absolutePath)
            return;

        m_absolutePath = absolutePath;
        emit absolutePathChanged(m_absolutePath);
    }

    void setProfileJsonObject(QJsonObject profileJsonObject)
    {
        if (m_profileJsonObject == profileJsonObject)
            return;

        m_profileJsonObject = profileJsonObject;
        emit profileJsonObjectChanged(m_profileJsonObject);
    }

private:
    QProcess m_process;

    shared_ptr<ProjectSettingsListModel> m_projectSettingsListModel;
    const shared_ptr<GlobalVariables>& m_globalVariables;

    QVector<int> m_screenNumber;

    QString m_previewImage;
    QString m_type;
    QString m_appID;
    QString m_file;
    QString m_fillMode;
    QString m_absolutePath;
    QJsonObject m_profileJsonObject;
};
}
