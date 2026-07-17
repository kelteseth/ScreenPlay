// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#include "ScreenPlay/screenplayexternalprocess.h"
#include "ScreenPlayCore/util.h"

#include <QDebug>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(screenPlayExternalProcess, "screenplay.screenplayexternalprocess")

namespace ScreenPlay {

ScreenPlayExternalProcess::ScreenPlayExternalProcess(
    const QString& appID,
    const std::shared_ptr<GlobalVariables>& globalVariables,
    const InstalledContentData& contentData,
    QObject* parent)
    : QObject(parent)
    , m_globalVariables(globalVariables)
    , m_appID(appID)
    , m_contentData(contentData)
{
    QObject::connect(&m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
        this, &ScreenPlayExternalProcess::processExit);
    QObject::connect(&m_process, &QProcess::errorOccurred,
        this, &ScreenPlayExternalProcess::handleProcessError);

    // Setup restart delay timer
    m_restartDelayTimer.setSingleShot(true);
    QObject::connect(&m_restartDelayTimer, &QTimer::timeout, this, [this]() {
        if (!attemptRestart()) {
            // If restart attempt failed, emit failure signal with user-friendly message
            QString contentTitle = m_contentData.title().isEmpty()
                ? tr("Untitled Content")
                : m_contentData.title();
            QString contentTypeStr = ContentTypes::toString(m_contentData.type());

            // Include path info for better identification if title is generic
            QString locationInfo;
            if (!m_contentData.absolutePath().isEmpty()) {
                QFileInfo pathInfo(m_contentData.absolutePath());
                locationInfo = tr(" from '%1'").arg(pathInfo.fileName());
            }

            QString errorMsg = tr("'%1' (%2)%3 could not be restarted (attempt %4 of %5).")
                                   .arg(contentTitle)
                                   .arg(contentTypeStr)
                                   .arg(locationInfo)
                                   .arg(m_retryCount)
                                   .arg(MAX_RESTART_ATTEMPTS);

            qCCritical(screenPlayExternalProcess) << "Restart attempt failed for" << m_appID << ":" << errorMsg;
            emit restartFailed(m_appID, errorMsg);
        }
    });

    // Setup stability timer - only reset retry count after process is stable
    m_stabilityTimer.setSingleShot(true);
    QObject::connect(&m_stabilityTimer, &QTimer::timeout, this, [this]() {
        if (m_isConnected && m_retryCount > 0) {
            qCInfo(screenPlayExternalProcess) << "Process" << m_appID << "has been stable for" << STABILITY_PERIOD_MS << "ms, resetting retry count";
            m_retryCount = 0;
        }
    });

    // Connect the ping-alive handler exactly once. setupSDKConnection() runs
    // again on every crash-restart reconnect - connecting there accumulated
    // one duplicate handler per restart.
    QObject::connect(&m_pingAliveTimer, &QTimer::timeout, this, [this]() {
        const std::optional<bool> running = m_processManager.isRunning(m_processID);
        // nullopt means the PID itself is invalid; a contained `false` means
        // the process exited. Both are a dead wallpaper/widget.
        if (!running.value_or(false)) {
            qCInfo(screenPlayExternalProcess) << "Process" << m_processID << "is gone (pid valid:" << running.has_value() << ")";
            handleTimeoutOrCrash();
        }
    });
}

bool ScreenPlayExternalProcess::terminate()
{
    m_pingAliveTimer.stop();
    m_restartDelayTimer.stop();
    m_stabilityTimer.stop();

    if (m_processID <= 0)
        return true;

    if (m_processManager.getProcessState(m_processID) != ProcessManager::ProcessState::Running)
        return true;

    qCInfo(screenPlayExternalProcess) << "Force-terminating process" << m_processID << "for" << m_appID;
    if (!m_processManager.terminateProcess(m_processID)) {
        qCWarning(screenPlayExternalProcess) << "Failed to terminate process" << m_processID;
        return false;
    }
    return true;
}

void ScreenPlayExternalProcess::setSDKConnection(std::unique_ptr<SDKConnection> connection)
{
    m_connection = std::move(connection);
    setIsConnected(true);

    // Don't reset retry count immediately - start stability timer instead
    if (m_retryCount > 0) {
        qCInfo(screenPlayExternalProcess) << "Connection established for" << m_appID << "after" << m_retryCount << "restart attempts, starting stability timer";
        m_stabilityTimer.start(STABILITY_PERIOD_MS);
    }
    m_isRestartingInProgress = false;

    setupSDKConnection();
}

void ScreenPlayExternalProcess::setupSDKConnection()
{
    if (!m_connection) {
        return;
    }

    QObject::connect(m_connection.get(), &SDKConnection::disconnected, this, [this]() {
        setIsConnected(false);
        m_pingAliveTimer.stop();
        qCInfo(screenPlayExternalProcess) << "App:" << m_connection->appID() << "disconnected";

        // Only treat as timeout/crash if we're not in a closing state
        if (m_state != ScreenPlay::ScreenPlayEnums::AppState::Closing && m_state != ScreenPlay::ScreenPlayEnums::AppState::ClosingFailed && m_state != ScreenPlay::ScreenPlayEnums::AppState::ClosedGracefully) {
            setState(ScreenPlayEnums::AppState::Timeout);
            handleTimeoutOrCrash();
        } else {
            qCDebug(screenPlayExternalProcess) << "Connection closed during intentional shutdown for" << m_connection->appID();
        }
    });

    // Start ping alive monitoring after a grace period. The handler itself is
    // connected once in the constructor.
    QTimer::singleShot(1000, this, [this]() {
        if (m_connection)
            m_pingAliveTimer.start(GlobalVariables::contentPingAliveIntervalMS);
    });
}

void ScreenPlayExternalProcess::handleProcessError(QProcess::ProcessError error)
{
    qCWarning(screenPlayExternalProcess) << "QProcess error occurred for app" << m_appID << ":" << error;

    switch (error) {
    case QProcess::FailedToStart:
        qCCritical(screenPlayExternalProcess) << "Process failed to start for app" << m_appID;
        setState(ScreenPlay::ScreenPlayEnums::AppState::StartingFailed);
        break;
    case QProcess::Crashed:
        qCCritical(screenPlayExternalProcess) << "Process crashed for app" << m_appID;
        setState(ScreenPlay::ScreenPlayEnums::AppState::Crashed);
        handleTimeoutOrCrash();
        break;
    case QProcess::Timedout:
        qCWarning(screenPlayExternalProcess) << "Process timeout for app" << m_appID;
        setState(ScreenPlay::ScreenPlayEnums::AppState::Timeout);
        handleTimeoutOrCrash();
        break;
    case QProcess::ReadError:
    case QProcess::WriteError:
    case QProcess::UnknownError:
        qCWarning(screenPlayExternalProcess) << "Process I/O or unknown error for app" << m_appID;
        setState(ScreenPlay::ScreenPlayEnums::AppState::ErrorOccurred);
        break;
    }
}

void ScreenPlayExternalProcess::processExit(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (exitCode != 0) {
        qCCritical(screenPlayExternalProcess) << "ERROR: App closed with appID:" << m_appID << "EXIT CODE:" << exitCode << exitStatus;
        setState(ScreenPlay::ScreenPlayEnums::AppState::Crashed);
        // Only attempt restart if we're not in a closing state
        if (m_state != ScreenPlay::ScreenPlayEnums::AppState::Closing && m_state != ScreenPlay::ScreenPlayEnums::AppState::ClosingFailed) {
            handleTimeoutOrCrash();
        }
        return;
    }
    setState(ScreenPlay::ScreenPlayEnums::AppState::ClosedGracefully);
    qCDebug(screenPlayExternalProcess) << "App closed with appID:" << m_appID;
}

void ScreenPlayExternalProcess::setAppID(QString appID)
{
    if (m_appID == appID)
        return;
    m_appID = appID;
    emit appIDChanged(m_appID);
}

void ScreenPlayExternalProcess::setProcessID(qint64 processID)
{
    if (m_processID == processID)
        return;
    m_processID = processID;
    emit processIDChanged(m_processID);
}

void ScreenPlayExternalProcess::setIsConnected(bool isConnected)
{
    if (m_isConnected == isConnected)
        return;
    m_isConnected = isConnected;
    emit isConnectedChanged(m_isConnected);
}

void ScreenPlayExternalProcess::setState(ScreenPlay::ScreenPlayEnums::AppState state)
{
    if (m_state == state)
        return;
    m_state = state;
    emit stateChanged(m_state);
}

void ScreenPlayExternalProcess::handleTimeoutOrCrash()
{
    // Stop timers to prevent further checks
    m_pingAliveTimer.stop();
    m_stabilityTimer.stop(); // Stop stability timer as process is no longer stable

    // Don't restart if we're in a closing state - this is intentional termination
    if (m_state == ScreenPlay::ScreenPlayEnums::AppState::Closing || m_state == ScreenPlay::ScreenPlayEnums::AppState::ClosingFailed || m_state == ScreenPlay::ScreenPlayEnums::AppState::ClosedGracefully) {
        qCDebug(screenPlayExternalProcess) << "Process" << m_appID << "is in closing state (" << static_cast<int>(m_state) << "), not attempting restart";
        return;
    }

    // Prevent multiple concurrent restart attempts
    if (m_isRestartingInProgress) {
        qCDebug(screenPlayExternalProcess) << "Restart already in progress for" << m_appID;
        return;
    }

    // Only attempt restart if we haven't exceeded max attempts
    if (m_retryCount < MAX_RESTART_ATTEMPTS) {
        m_isRestartingInProgress = true;
        qCInfo(screenPlayExternalProcess) << "Process" << m_appID << "failed, initiating restart attempt" << (m_retryCount + 1) << "of" << MAX_RESTART_ATTEMPTS;

        // Start restart delay timer
        m_restartDelayTimer.start(RESTART_DELAY_MS);
    } else {
        // Max attempts reached, emit failure signal with user-facing message
        m_isRestartingInProgress = false;

        // Create user-friendly error message with title and content type
        QString contentTitle = m_contentData.title().isEmpty()
            ? tr("Untitled Content")
            : m_contentData.title();
        QString contentTypeStr = ContentTypes::toString(m_contentData.type());

        QString errorMsg = tr("'%1' (%2) failed to restart after %3 attempts and has been stopped.")
                               .arg(contentTitle)
                               .arg(contentTypeStr)
                               .arg(MAX_RESTART_ATTEMPTS);

        qCCritical(screenPlayExternalProcess) << "Restart failed for" << m_appID << ":" << errorMsg;
        emit restartFailed(m_appID, errorMsg);
    }
}

bool ScreenPlayExternalProcess::attemptRestart()
{
    // Increment retry count at the start of actual restart attempt
    m_retryCount++;
    qCInfo(screenPlayExternalProcess) << "Attempting to restart process for appID:" << m_appID << "- Attempt" << m_retryCount << "of" << MAX_RESTART_ATTEMPTS;

    // Clean up existing process
    if (m_process.state() != QProcess::NotRunning) {
        m_process.kill();
        m_process.waitForFinished(3000);
    }

    // Reset connection state
    setIsConnected(false);
    m_connection.reset();

    // Attempt to restart
    bool success = start();

    if (success) {
        qCInfo(screenPlayExternalProcess) << "Successfully restarted process for appID:" << m_appID;
        // Don't reset retry count yet - wait for successful connection
        setState(ScreenPlay::ScreenPlayEnums::AppState::Starting);
    } else {
        qCWarning(screenPlayExternalProcess) << "Failed to restart process for appID:" << m_appID;
        setState(ScreenPlay::ScreenPlayEnums::AppState::StartingFailed);
    }

    // Reset the restart flag regardless of success/failure
    m_isRestartingInProgress = false;

    return success;
}

}

#include "moc_screenplayexternalprocess.cpp"
