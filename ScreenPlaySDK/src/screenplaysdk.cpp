// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#include "ScreenPlaySDK/screenplaysdk.h"

#include <QJsonDocument>
#include <QJsonParseError>
#include <QLoggingCategory>
#include <QVariant>

#include <atomic>
#include <cstdio>

Q_LOGGING_CATEGORY(screenPlaySDK, "screenplay.sdk")

/*!
    \module ScreenPlaySDK
    \title ScreenPlaySDK
    \brief Namespace for ScreenPlaySDK.
*/

// Static member – replaces the old file-scope global_sdkPtr.
// Only one ScreenPlaySDK instance per process is expected; the atomic
// makes the pointer safe to read from the message-handler thread.
std::atomic<ScreenPlaySDK*> ScreenPlaySDK::s_instance { nullptr };

/*!
    \class ScreenPlaySDK
    \inmodule ScreenPlaySDK
    \brief  .
*/

ScreenPlaySDK::ScreenPlaySDK(const QString& appID, const QString& type, QObject* parent)
    : QObject(parent)
    , m_type { type }
    , m_appID { appID }
{
}

void ScreenPlaySDK::start()
{
    // Redirect all Qt messages from this process to the main ScreenPlay app.
    s_instance.store(this, std::memory_order_release);
    qInstallMessageHandler(ScreenPlaySDK::messageHandler);

    connect(&m_socket, &QLocalSocket::connected, this, &ScreenPlaySDK::connected);
    connect(&m_socket, &QLocalSocket::disconnected, this, &ScreenPlaySDK::disconnected);
    connect(&m_socket, &QLocalSocket::readyRead, this, &ScreenPlaySDK::readyRead);
    connect(&m_socket, &QLocalSocket::errorOccurred, this, [this]() {
        disconnected();
    });

    m_socket.connectToServer("ScreenPlay");
    if (!m_socket.waitForConnected(1000)) {
        disconnected();
    }
}

ScreenPlaySDK::~ScreenPlaySDK()
{
    if (s_instance.load(std::memory_order_acquire) == this) {
        qInstallMessageHandler(nullptr);
        s_instance.store(nullptr, std::memory_order_release);
    }
    m_socket.disconnectFromServer();
}

void ScreenPlaySDK::sendMessage(const QJsonObject& obj)
{
    QJsonDocument doc(obj);
    m_socket.write({ doc.toJson(QJsonDocument::Compact) });
    m_socket.waitForBytesWritten();
}

void ScreenPlaySDK::connected()
{
    if (m_appID.isEmpty() || m_type.isEmpty()) {
        qCCritical(screenPlaySDK) << "Unable to connect with empty appID/type:" << m_appID << m_type;
        disconnected();
        return;
    }

    QByteArray welcomeMessage = QString("appID=" + m_appID + "," + m_type + ";").toUtf8();
    m_socket.write(welcomeMessage);
    if (!m_socket.waitForBytesWritten()) {
        disconnected();
        return;
    }

    QObject::disconnect(&m_pingAliveTimer, &QTimer::timeout, this, &ScreenPlaySDK::pingAlive);
    QObject::connect(&m_pingAliveTimer, &QTimer::timeout, this, &ScreenPlaySDK::pingAlive);
    m_pingAliveTimer.start(1000);

    setIsConnected(true);
    emit sdkConnected();
}

void ScreenPlaySDK::disconnected()
{
    // Guard against multiple calls — socket errors, explicit disconnects, and
    // the QLocalSocket::disconnected signal can all race here.
    if (!m_isConnected)
        return;

    m_pingAliveTimer.stop();
    QObject::disconnect(&m_pingAliveTimer, &QTimer::timeout, this, &ScreenPlaySDK::pingAlive);
    setIsConnected(false);
    emit sdkDisconnected();
}

void ScreenPlaySDK::readyRead()
{
    // Use the same semicolon-delimited protocol as the server side
    // (SDKConnection::readyRead) so multi-message packets are handled.
    const QString raw = m_socket.readAll();
    const QStringList messages = raw.split(";");

    for (const QString& msg : messages) {
        if (msg.isEmpty())
            continue;

        // Try to parse as JSON first (commands from the main app arrive as JSON).
        QJsonParseError err {};
        auto doc = QJsonDocument::fromJson(msg.toUtf8(), &err);

        if (err.error != QJsonParseError::NoError) {
            // Not JSON — might be a plain-text frame we don't understand.
            emit incomingMessageError(err.errorString());
            continue;
        }

        QJsonObject obj = doc.object();

        // Handle "quit" command
        if (obj.value("command").toString() == "quit") {
            disconnected();
            return;
        }

        // Handle "replace" command
        if (obj.value("command").toString() == "replace") {
            const QString type = obj.value("type").toString();
            const QString fillMode = obj.value("fillMode").toString();
            const QString absolutePath = obj.value("absolutePath").toString();
            const QString file = obj.value("file").toString();
            const QJsonObject properties = obj.value("properties").toObject();
            const bool checkWallpaperVisible = obj.value("checkWallpaperVisible").toBool();

            if (type.isEmpty()
                || fillMode.isEmpty()
                || absolutePath.isEmpty()
                || file.isEmpty()
                || !obj.contains("volume")) {
                qCWarning(screenPlaySDK) << "Command replace with incomplete message received:"
                                         << type << fillMode << absolutePath << file;
                continue;
            }

            bool volumeParsedOK = false;
            float volumeParsed = obj.value("volume").toVariant().toFloat(&volumeParsedOK);
            if (!volumeParsedOK) {
                qCWarning(screenPlaySDK) << "Command replace contained unparseable volume, defaulting to 1.0";
                volumeParsed = 1.0f;
            } else if (volumeParsed < 0.0f || volumeParsed > 1.0f) {
                qCWarning(screenPlaySDK) << "Command replace contained out-of-range volume value:" << volumeParsed;
                volumeParsed = qBound(0.0f, volumeParsed, 1.0f);
            }

            qCInfo(screenPlaySDK) << type << fillMode << volumeParsed
                                  << absolutePath << file << properties;

            emit replaceWallpaper(
                absolutePath,
                file,
                volumeParsed,
                fillMode,
                type,
                checkWallpaperVisible,
                properties);
            continue;
        }

        // Generic key-value messages
        for (auto it = obj.constBegin(); it != obj.constEnd(); ++it) {
            emit incomingMessage(it.key(), it.value().toVariant().toString());
        }
    }
}

void ScreenPlaySDK::redirectMessage(const QByteArray& msg)
{
    if (isConnected()) {
        m_socket.write(msg);
        m_socket.waitForBytesWritten();
    }
}

void ScreenPlaySDK::pingAlive()
{
    m_socket.write("ping;");
    if (!m_socket.waitForBytesWritten(500)) {
        qCInfo(screenPlaySDK) << "Cannot ping to main application. Closing!";
        emit sdkDisconnected();
        return;
    }

    if (m_socket.state() != QLocalSocket::ConnectedState) {
        qCInfo(screenPlaySDK) << "Socket no longer connected. Closing!";
        emit sdkDisconnected();
        return;
    }

    if (m_mainAppPID != 0) {
        std::optional<bool> running = m_processManager.isRunning(m_mainAppPID);
        if (running.has_value()) {
            const bool isMainAppRunning = running.value();
            if (isMainAppRunning)
                return;
        }
        emit sdkDisconnected();
    }
}

/*!
    \brief Installed via qInstallMessageHandler to redirect all Qt log output
    from this wallpaper/widget process to the main ScreenPlay application.

    IMPORTANT: This function must NOT use qCDebug / qCWarning / qCInfo / etc.
    because those macros call back into the installed message handler, causing
    infinite recursion. All local output uses fprintf(stderr, ...) instead.
*/
void ScreenPlaySDK::messageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
    // Re-entrancy guard — in case fprintf or socket writes somehow trigger
    // another Qt message (should not happen, but defence in depth).
    static thread_local bool inHandler = false;
    if (inHandler)
        return;
    inHandler = true;

    auto* sdk = s_instance.load(std::memory_order_acquire);
    if (sdk) {
        QByteArray localMsg = msg.toLocal8Bit();
        localMsg += "\nFile: " + QByteArray(context.file ? context.file : "unknown") + ", ";
        localMsg += "\nin line " + QByteArray::number(context.line);
        sdk->redirectMessage(localMsg);
    }

    // Write to stderr directly — do NOT use qC* macros (would re-enter this handler).
    const QByteArray local = msg.toLocal8Bit();
    switch (type) {
    case QtDebugMsg:
        fprintf(stderr, "[debug] %s\n", local.constData());
        break;
    case QtInfoMsg:
        fprintf(stderr, "[info] %s\n", local.constData());
        break;
    case QtWarningMsg:
        fprintf(stderr, "[warning] %s\n", local.constData());
        break;
    case QtCriticalMsg:
        fprintf(stderr, "[critical] %s\n", local.constData());
        break;
    case QtFatalMsg:
        fprintf(stderr, "[fatal] %s\n", local.constData());
        break;
    }

    inHandler = false;
}

void ScreenPlaySDK::setType(const QString& type)
{
    if (m_type == type)
        return;
    m_type = type;
    emit typeChanged(m_type);
}

void ScreenPlaySDK::setIsConnected(bool isConnected)
{
    if (m_isConnected == isConnected)
        return;
    m_isConnected = isConnected;
    emit isConnectedChanged(m_isConnected);
}

void ScreenPlaySDK::setAppID(const QString& appID)
{
    if (m_appID == appID)
        return;
    m_appID = appID;
    emit appIDChanged(m_appID);
}

qint64 ScreenPlaySDK::mainAppPID() const
{
    return m_mainAppPID;
}

void ScreenPlaySDK::setMainAppPID(qint64 mainAppPID)
{
    if (m_mainAppPID == mainAppPID)
        return;
    m_mainAppPID = mainAppPID;
    emit mainAppPIDChanged(m_mainAppPID);
}
