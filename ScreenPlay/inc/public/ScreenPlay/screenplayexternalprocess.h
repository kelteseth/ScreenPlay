// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#pragma once

#include <QJsonObject>
#include <QObject>
#include <QProcess>
#include <QString>
#include <QStringList>
#include <QTimer>
#include <memory>

#include "ScreenPlay/globalvariables.h"
#include "ScreenPlay/projectsettingslistmodel.h"
#include "ScreenPlay/sdkconnection.h"
#include "ScreenPlayCore/globalenums.h"
#include "ScreenPlayCore/processmanager.h"
#include "ScreenPlayCore/util.h"

namespace ScreenPlay {

class ScreenPlayExternalProcess : public QObject {
    Q_OBJECT
    Q_CLASSINFO("RegisterEnumClassesUnscoped", "false")

    // Q_PROPERTY declarations for common app properties
    Q_PROPERTY(QString appID READ appID WRITE setAppID NOTIFY appIDChanged FINAL)
    Q_PROPERTY(qint64 processID READ processID WRITE setProcessID NOTIFY processIDChanged FINAL)
    Q_PROPERTY(bool isConnected READ isConnected WRITE setIsConnected NOTIFY isConnectedChanged FINAL)
    Q_PROPERTY(QString absolutePath READ absolutePath WRITE setAbsolutePath NOTIFY absolutePathChanged FINAL)
    Q_PROPERTY(QString previewImage READ previewImage WRITE setPreviewImage NOTIFY previewImageChanged FINAL)
    Q_PROPERTY(ContentTypes::InstalledType type READ type WRITE setType NOTIFY typeChanged FINAL)
    Q_PROPERTY(ScreenPlay::ScreenPlayEnums::AppState state READ state WRITE setState NOTIFY stateChanged FINAL)

public:
    explicit ScreenPlayExternalProcess(
        const QString& appID,
        const std::shared_ptr<GlobalVariables>& globalVariables,
        const QString& absolutePath,
        const QString& previewImage,
        const ContentTypes::InstalledType type,
        QObject* parent = nullptr);

    virtual ~ScreenPlayExternalProcess() = default;

    virtual bool start() = 0;
    virtual QCoro::Task<Result> close() = 0;

    void setSDKConnection(std::unique_ptr<SDKConnection> connection);
    std::shared_ptr<ProjectSettingsListModel> getProjectSettingsListModel() { return m_projectSettingsListModel; }

    // Common getters
    QString appID() const { return m_appID; }
    qint64 processID() const { return m_processID; }
    bool isConnected() const { return m_isConnected; }
    QString absolutePath() const { return m_absolutePath; }
    QString previewImage() const { return m_previewImage; }
    ContentTypes::InstalledType type() const { return m_type; }
    ScreenPlay::ScreenPlayEnums::AppState state() const { return m_state; }

signals:
    void appIDChanged(QString appID);
    void processIDChanged(qint64 processID);
    void isConnectedChanged(bool isConnected);
    void absolutePathChanged(QString absolutePath);
    void previewImageChanged(QString previewImage);
    void typeChanged(ContentTypes::InstalledType type);
    void stateChanged(ScreenPlay::ScreenPlayEnums::AppState state);
    void requestClose(const QString& appID);
    void error(const QString& msg);

public slots:
    void processExit(int exitCode, QProcess::ExitStatus exitStatus);
    void setAppID(QString appID);
    void setProcessID(qint64 processID);
    void setIsConnected(bool isConnected);
    void setAbsolutePath(QString absolutePath);
    void setPreviewImage(QString previewImage);
    void setType(ContentTypes::InstalledType type);
    void setState(ScreenPlay::ScreenPlayEnums::AppState state);

protected:
    virtual void setupSDKConnection();
    virtual void handleProcessError(QProcess::ProcessError error);

    const std::shared_ptr<GlobalVariables> m_globalVariables;
    std::unique_ptr<SDKConnection> m_connection;
    std::shared_ptr<ProjectSettingsListModel> m_projectSettingsListModel;

    ProcessManager m_processManager;
    QProcess m_process;
    QTimer m_pingAliveTimer;
    QStringList m_appArgumentsList;

    QString m_appID;
    QString m_absolutePath;
    QString m_previewImage;
    ContentTypes::InstalledType m_type { ContentTypes::InstalledType::Unknown };
    ScreenPlay::ScreenPlayEnums::AppState m_state = ScreenPlay::ScreenPlayEnums::AppState::NotSet;

    bool m_isConnected { false };
    qint64 m_processID { 0 };
};
}
