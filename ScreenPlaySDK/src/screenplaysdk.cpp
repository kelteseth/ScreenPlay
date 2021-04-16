#include "screenplaysdk.h"

// USE THIS ONLY FOR redirectMessageOutputToMainWindow
static ScreenPlaySDK* global_sdkPtr = nullptr;

ScreenPlaySDK::ScreenPlaySDK(QQuickItem* parent)
    : QQuickItem(parent)
{
    init();
}

ScreenPlaySDK::ScreenPlaySDK(
    const QString& appID,
    const QString& type,
    QQuickItem* parent)
    : QQuickItem(parent)
    , m_type { type }
    , m_appID { appID }
{
    init();
}

void ScreenPlaySDK::init()
{
    // Redirect all messages from this to ScreenPlay
    global_sdkPtr = this;
    qInstallMessageHandler(ScreenPlaySDK::redirectMessageOutputToMainWindow);

    m_socket.setServerName("ScreenPlay");
    connect(&m_socket, &QLocalSocket::connected, this, &ScreenPlaySDK::connected);
    connect(&m_socket, &QLocalSocket::disconnected, this, &ScreenPlaySDK::disconnected);
    connect(&m_socket, &QLocalSocket::readyRead, this, &ScreenPlaySDK::readyRead);
    connect(&m_socket, QOverload<QLocalSocket::LocalSocketError>::of(&QLocalSocket::error), this, &ScreenPlaySDK::error);
    m_socket.connectToServer();

    // If the wallpaper never connects it will never get the
    // disconnect event. We can savely assume no connection will
    // be made after 1 second timeout.
    QTimer::singleShot(1000, this, [this]() {
        if (m_socket.state() != QLocalSocket::ConnectedState) {
            m_socket.disconnectFromServer();
            emit sdkDisconnected();
        }

        QObject::connect(&m_pingAliveTimer, &QTimer::timeout, this, &ScreenPlaySDK::pingAlive);
        m_pingAliveTimer.start(1000);
    });
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
    QByteArray welcomeMessage = QString(m_appID + "," + m_type).toUtf8();
    m_socket.write(welcomeMessage);
    m_socket.waitForBytesWritten();

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

            qWarning()
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
    if (socketError == QLocalSocket::LocalSocketError::ConnectionRefusedError) {
        QCoreApplication::quit();
    }
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
    QByteArray file = "File: " + QByteArray(context.file) + ", ";
    QByteArray line = "in line " + QByteArray::number(context.line) + ", ";

    global_sdkPtr->redirectMessage(localMsg);
}
