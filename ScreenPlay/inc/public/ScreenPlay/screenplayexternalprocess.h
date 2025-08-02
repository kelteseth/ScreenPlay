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
#include "ScreenPlay/installedcontentdata.h"
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
    Q_PROPERTY(QString absolutePath READ absolutePath NOTIFY absolutePathChanged FINAL)
    Q_PROPERTY(QString previewImage READ previewImage NOTIFY previewImageChanged FINAL)
    Q_PROPERTY(ContentTypes::InstalledType type READ type NOTIFY typeChanged FINAL)
    Q_PROPERTY(ScreenPlay::ScreenPlayEnums::AppState state READ state WRITE setState NOTIFY stateChanged FINAL)

public:
    explicit ScreenPlayExternalProcess(
        const QString& appID,
        const std::shared_ptr<GlobalVariables>& globalVariables,
        const InstalledContentData& contentData,
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
    QString absolutePath() const { return m_contentData.absolutePath(); }
    QString previewImage() const { return m_contentData.previewImage(); }
    ContentTypes::InstalledType type() const { return m_contentData.type(); }
    ScreenPlay::ScreenPlayEnums::AppState state() const { return m_state; }

    // Access to content data for error reporting
    const InstalledContentData& contentData() const { return m_contentData; }

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

    /*!
        \brief Emitted when the external process fails to restart after multiple attempts.

        \param appID The application ID of the failed process
        \param message A descriptive error message for the user

        This signal is emitted when the process has crashed or timed out and all
        restart attempts have failed. The message should be displayed to the user
        to inform them of the issue.
    */
    void restartFailed(const QString& appID, const QString& message);

public slots:
    void processExit(int exitCode, QProcess::ExitStatus exitStatus);
    void setAppID(QString appID);
    void setProcessID(qint64 processID);
    void setIsConnected(bool isConnected);
    void setState(ScreenPlay::ScreenPlayEnums::AppState state);

protected:
    virtual void setupSDKConnection();
    virtual void handleProcessError(QProcess::ProcessError error);

    /*!
        \brief Handles timeout or crash events by attempting to restart the process.

        This method is called when the external process times out or crashes. It will
        attempt to restart the process up to MAX_RESTART_ATTEMPTS times. If all attempts
        fail, it emits the restartFailed signal.
    */
    virtual void handleTimeoutOrCrash();

    /*!
        \brief Attempts to restart the external process.

        \return true if the restart was successful, false otherwise.

        This method cleans up the existing process and attempts to start a new one.
        If successful, it resets the retry count and updates the state.
    */
    virtual bool attemptRestart();

    const std::shared_ptr<GlobalVariables> m_globalVariables;
    std::unique_ptr<SDKConnection> m_connection;
    std::shared_ptr<ProjectSettingsListModel> m_projectSettingsListModel;

    ProcessManager m_processManager;
    QProcess m_process;
    QTimer m_pingAliveTimer;
    QTimer m_restartDelayTimer;
    QTimer m_stabilityTimer;
    QStringList m_appArgumentsList;

    QString m_appID;
    InstalledContentData m_contentData;
    ScreenPlay::ScreenPlayEnums::AppState m_state = ScreenPlay::ScreenPlayEnums::AppState::NotSet;

    bool m_isConnected { false };
    qint64 m_processID { 0 };
    int m_retryCount { 0 };
    bool m_isRestartingInProgress { false };

    // Maximum number of restart attempts before giving up
    static constexpr int MAX_RESTART_ATTEMPTS = 2;
    // Delay between restart attempts in milliseconds
    static constexpr int RESTART_DELAY_MS = 2000;
    // Time to wait before considering the process stable and resetting retry count
    static constexpr int STABILITY_PERIOD_MS = 30000; // 30 seconds
};
}
