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
}

void ScreenPlayExternalProcess::setSDKConnection(std::unique_ptr<SDKConnection> connection)
{
    m_connection = std::move(connection);
    setIsConnected(true);
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
    });

    // Setup ping alive monitoring
    QTimer::singleShot(4000, this, [this]() {
        QObject::connect(&m_pingAliveTimer, &QTimer::timeout, this, [this]() {
            std::optional<bool> running = m_processManager.isRunning(m_processID);
            if (running.has_value()) {
                // Process is running
            } else {
                qInfo() << "INVALID PID:" << m_processID;
                emit requestClose(m_appID);
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
        break;
    case QProcess::Timedout:
        qWarning() << "Process timeout for app" << m_appID;
        setState(ScreenPlay::ScreenPlayEnums::AppState::Timeout);
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

}

#include "moc_screenplayexternalprocess.cpp"
