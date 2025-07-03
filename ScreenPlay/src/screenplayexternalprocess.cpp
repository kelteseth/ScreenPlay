// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#include "ScreenPlay/screenplayexternalprocess.h"
#include "ScreenPlayCore/util.h"

#include <QDebug>

namespace ScreenPlay {

ScreenPlayExternalProcess::ScreenPlayExternalProcess(
    const QString& appID,
    const std::shared_ptr<GlobalVariables>& globalVariables,
    const QString& absolutePath,
    const QString& previewImage,
    const ContentTypes::InstalledType type,
    QObject* parent)
    : QObject(parent)
    , m_globalVariables(globalVariables)
    , m_appID(appID)
    , m_absolutePath(absolutePath)
    , m_previewImage(previewImage)
    , m_type(type)
{
    QObject::connect(&m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
        this, &ScreenPlayExternalProcess::processExit);
    QObject::connect(&m_process, &QProcess::errorOccurred,
        this, &ScreenPlayExternalProcess::handleProcessError);
    
    // Setup restart delay timer
    m_restartDelayTimer.setSingleShot(true);
    QObject::connect(&m_restartDelayTimer, &QTimer::timeout, this, [this]() {
        if (!attemptRestart()) {
            // If restart attempt failed, emit failure signal
            QString errorMsg = QString("Failed to restart %1 (attempt %2 of %3). Process could not be started.")
                .arg(m_appID)
                .arg(m_retryCount)
                .arg(MAX_RESTART_ATTEMPTS);
            qCritical() << errorMsg;
            emit restartFailed(m_appID, errorMsg);
        }
    });
    
    // Setup stability timer - only reset retry count after process is stable
    m_stabilityTimer.setSingleShot(true);
    QObject::connect(&m_stabilityTimer, &QTimer::timeout, this, [this]() {
        if (m_isConnected && m_retryCount > 0) {
            qInfo() << "Process" << m_appID << "has been stable for" << STABILITY_PERIOD_MS << "ms, resetting retry count";
            m_retryCount = 0;
        }
    });
}

void ScreenPlayExternalProcess::setSDKConnection(std::unique_ptr<SDKConnection> connection)
{
    m_connection = std::move(connection);
    setIsConnected(true);
    
    // Don't reset retry count immediately - start stability timer instead
    if (m_retryCount > 0) {
        qInfo() << "Connection established for" << m_appID << "after" << m_retryCount << "restart attempts, starting stability timer";
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
        setState(ScreenPlayEnums::AppState::Timeout);
        m_pingAliveTimer.stop();
        qInfo() << "App:" << m_connection->appID() << "disconnected";
        handleTimeoutOrCrash();
    });

    // Setup ping alive monitoring
    QTimer::singleShot(4000, this, [this]() {
        QObject::connect(&m_pingAliveTimer, &QTimer::timeout, this, [this]() {
            std::optional<bool> running = m_processManager.isRunning(m_processID);
            if (running.has_value()) {
                // Process is running
            } else {
                qInfo() << "INVALID PID:" << m_processID;
                handleTimeoutOrCrash();
            }
        });
        m_pingAliveTimer.start(GlobalVariables::contentPingAliveIntervalMS);
    });
}

void ScreenPlayExternalProcess::handleProcessError(QProcess::ProcessError error)
{
    qWarning() << "QProcess error occurred for app" << m_appID << ":" << error;

    switch (error) {
    case QProcess::FailedToStart:
        qCritical() << "Process failed to start for app" << m_appID;
        setState(ScreenPlay::ScreenPlayEnums::AppState::StartingFailed);
        break;
    case QProcess::Crashed:
        qCritical() << "Process crashed for app" << m_appID;
        setState(ScreenPlay::ScreenPlayEnums::AppState::Crashed);
        handleTimeoutOrCrash();
        break;
    case QProcess::Timedout:
        qWarning() << "Process timeout for app" << m_appID;
        setState(ScreenPlay::ScreenPlayEnums::AppState::Timeout);
        handleTimeoutOrCrash();
        break;
    case QProcess::ReadError:
    case QProcess::WriteError:
    case QProcess::UnknownError:
        qWarning() << "Process I/O or unknown error for app" << m_appID;
        setState(ScreenPlay::ScreenPlayEnums::AppState::ErrorOccurred);
        break;
    }
}

void ScreenPlayExternalProcess::processExit(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (exitCode != 0) {
        qCritical() << "ERROR: App closed with appID:" << m_appID << "EXIT CODE:" << exitCode << exitStatus;
        setState(ScreenPlay::ScreenPlayEnums::AppState::Crashed);
        handleTimeoutOrCrash();
        return;
    }
    setState(ScreenPlay::ScreenPlayEnums::AppState::ClosedGracefully);
    qDebug() << "App closed with appID:" << m_appID;
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

void ScreenPlayExternalProcess::setAbsolutePath(QString absolutePath)
{
    if (m_absolutePath == absolutePath)
        return;
    m_absolutePath = absolutePath;
    emit absolutePathChanged(m_absolutePath);
}

void ScreenPlayExternalProcess::setPreviewImage(QString previewImage)
{
    if (m_previewImage == previewImage)
        return;
    m_previewImage = previewImage;
    emit previewImageChanged(m_previewImage);
}

void ScreenPlayExternalProcess::setType(ContentTypes::InstalledType type)
{
    if (m_type == type)
        return;
    m_type = type;
    emit typeChanged(m_type);
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
    
    // Prevent multiple concurrent restart attempts
    if (m_isRestartingInProgress) {
        qDebug() << "Restart already in progress for" << m_appID;
        return;
    }
    
    // Only attempt restart if we haven't exceeded max attempts
    if (m_retryCount < MAX_RESTART_ATTEMPTS) {
        m_isRestartingInProgress = true;
        qInfo() << "Process" << m_appID << "failed, initiating restart attempt" << (m_retryCount + 1) << "of" << MAX_RESTART_ATTEMPTS;
        
        // Start restart delay timer
        m_restartDelayTimer.start(RESTART_DELAY_MS);
    } else {
        // Max attempts reached, emit failure signal
        m_isRestartingInProgress = false;
        QString errorMsg = QString("Process %1 failed after %2 restart attempts. Type: %3")
            .arg(m_appID)
            .arg(MAX_RESTART_ATTEMPTS)
            .arg(static_cast<int>(m_type));
        qCritical() << errorMsg;
        emit restartFailed(m_appID, errorMsg);
    }
}

bool ScreenPlayExternalProcess::attemptRestart()
{
    // Increment retry count at the start of actual restart attempt
    m_retryCount++;
    qInfo() << "Attempting to restart process for appID:" << m_appID << "- Attempt" << m_retryCount << "of" << MAX_RESTART_ATTEMPTS;
    
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
        qInfo() << "Successfully restarted process for appID:" << m_appID;
        // Don't reset retry count yet - wait for successful connection
        setState(ScreenPlay::ScreenPlayEnums::AppState::Starting);
    } else {
        qWarning() << "Failed to restart process for appID:" << m_appID;
        setState(ScreenPlay::ScreenPlayEnums::AppState::StartingFailed);
    }
    
    // Reset the restart flag regardless of success/failure
    m_isRestartingInProgress = false;
    
    return success;
}

}

#include "moc_screenplayexternalprocess.cpp"
