// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#include "ScreenPlaySDK/screenplaysdk.h"

/*!
    \module ScreenPlaySDK
    \title ScreenPlaySDK
    \brief Namespace for ScreenPlaySDK.
*/

// USE THIS ONLY FOR redirectMessageOutputToMainWindow
static ScreenPlaySDK* global_sdkPtr = nullptr;

/*!
    \class ScreenPlaySDK
    \inmodule ScreenPlaySDK
    \brief  .
*/

ScreenPlaySDK::ScreenPlaySDK(const QString& appID, const QString& type)
    : QObject(nullptr)
    , m_type { type }
    , m_appID { appID }
{
}

void ScreenPlaySDK::start()
{
    // Redirect all messages from this to ScreenPlay
    global_sdkPtr = this;
    qInstallMessageHandler(ScreenPlaySDK::redirectMessageOutputToMainWindow);

    connect(&m_socket, &QLocalSocket::connected, this, &ScreenPlaySDK::connected);
    connect(&m_socket, &QLocalSocket::disconnected, this, &ScreenPlaySDK::disconnected);
    connect(&m_socket, &QLocalSocket::readyRead, this, &ScreenPlaySDK::readyRead);
    connect(&m_socket, &QLocalSocket::errorOccurred, this, [this]() {
        emit disconnected();
    });

    m_socket.connectToServer("ScreenPlay");
    if (!m_socket.waitForConnected(1000)) {
        emit disconnected();
    }
}

ScreenPlaySDK::~ScreenPlaySDK()
{
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
        qCritical() << "Unable to connect with empyt: " << m_appID << m_type;
        emit disconnected();
        return;
    }

    QByteArray welcomeMessage = QString("appID=" + m_appID + "," + m_type).toUtf8();
    m_socket.write(welcomeMessage);
    if (!m_socket.waitForBytesWritten()) {
        disconnected();
        return;
    }

    QObject::connect(&m_pingAliveTimer, &QTimer::timeout, this, &ScreenPlaySDK::pingAlive);
    m_pingAliveTimer.start(1000);

    setIsConnected(true);
    emit sdkConnected();
}

void ScreenPlaySDK::disconnected()
{
    setIsConnected(false);
    emit sdkDisconnected();
}

void ScreenPlaySDK::readyRead()
{
    QString tmp = m_socket.readAll();
    QJsonParseError err {};
    auto doc = QJsonDocument::fromJson(QByteArray::fromStdString(tmp.toStdString()), &err);

    if (!(err.error == QJsonParseError::NoError)) {
        emit incommingMessageError(err.errorString());
        return;
    }
    QJsonObject obj = doc.object();

    if (obj.size() == 1) {
        if (obj.contains("command")) {
            if (obj.value("command") == "quit") {
                emit sdkDisconnected();
                return;
            }
        }
    }

    if (obj.contains("command")) {
        if (obj.value("command") == "replace") {
            QString type = obj.value("type").toString();
            QString fillMode = obj.value("fillMode").toString();
            QString absolutePath = obj.value("absolutePath").toString();
            QString file = obj.value("file").toString();
            bool checkWallpaperVisible = obj.value("checkWallpaperVisible").toBool();
            if (type.isEmpty()
                || fillMode.isEmpty()
                || absolutePath.isEmpty()
                || file.isEmpty()
                || (!obj.contains("volume"))) {
                qWarning() << "Command replace with incompile message received: "
                           << type
                           << fillMode
                           << absolutePath
                           << file;
                return;
            }

            bool volumeParsedOK = false;
            float volumeParsed = QVariant(obj.value("volume").toVariant()).toFloat(&volumeParsedOK);
            if (!volumeParsedOK && (volumeParsed > 0.0 && volumeParsed <= 1.0)) {
                qWarning() << "Command replaced contained bad volume float value: " << volumeParsed;
            }

            qInfo()
                << type
                << fillMode
                << volumeParsed
                << absolutePath
                << file;

            emit replaceWallpaper(absolutePath, file, volumeParsed, fillMode, type, checkWallpaperVisible);
            return;
        }
    }

    QJsonObject::iterator iterator;
    for (iterator = obj.begin(); iterator != obj.end(); iterator++) {
        emit incommingMessage(iterator.key(), obj.value(iterator.key()).toString());
    }
}

void ScreenPlaySDK::error(QLocalSocket::LocalSocketError socketError)
{
    Q_UNUSED(socketError)
    emit sdkDisconnected();
}

void ScreenPlaySDK::redirectMessage(QByteArray& msg)
{
    if (isConnected()) {
        m_socket.write(msg);
        m_socket.waitForBytesWritten();
    }
}

void ScreenPlaySDK::pingAlive()
{
    m_socket.write("ping");
    if (!m_socket.waitForBytesWritten(500)) {
        qInfo() << "Cannot ping to main application. Closing!";
        emit sdkDisconnected();
    }
}

void ScreenPlaySDK::ScreenPlaySDK::redirectMessageOutputToMainWindow(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
    Q_UNUSED(type)

    if (global_sdkPtr == nullptr)
        return;

    QByteArray localMsg = msg.toLocal8Bit();
    localMsg += "\nFile: " + QByteArray(context.file) + ", ";
    localMsg += "\nin line " + QByteArray::number(context.line) + ", ";

    global_sdkPtr->redirectMessage(localMsg);

    // Also redirect to regular output if we debug
    // wallpaper or widgets directly
    switch (type) {
    case QtDebugMsg:
        qDebug() << msg;
        break;
    case QtWarningMsg:
        qWarning() << msg;
        break;
    case QtCriticalMsg:
    case QtFatalMsg:
        qCritical() << msg;
        break;
    case QtInfoMsg:
        qInfo() << msg;
        break;
    default:
        break;
    }
}
