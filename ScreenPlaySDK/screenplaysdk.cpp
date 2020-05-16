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
    QTimer::singleShot(10000,[this](){
        if(m_socket.state() != QLocalSocket::ConnectedState){
           m_socket.disconnectFromServer();
           emit sdkDisconnected();
        }
    });

}

ScreenPlaySDK::~ScreenPlaySDK()
{
    m_socket.disconnectFromServer();
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
    QJsonParseError err;
    auto doc = QJsonDocument::fromJson(QByteArray::fromStdString(tmp.toStdString()), &err);

    if (!(err.error == QJsonParseError::NoError)) {
        emit incommingMessageError(err.errorString());
        return;
    }
    QJsonObject ob = doc.object();
    QJsonObject::iterator iterator;
    for (iterator = ob.begin(); iterator != ob.end(); iterator++) {
        emit incommingMessage(iterator.key(), ob.value(iterator.key()).toString());
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
