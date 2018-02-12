#include "screenplaysdk.h"

ScreenPlaySDK::ScreenPlaySDK(QQuickItem *parent):
    QQuickItem(parent)
{
    // By default, QQuickItem does not draw anything. If you subclass
    // QQuickItem to create a visual item, you will need to uncomment the
    // following line and re-implement updatePaintNode()

    // setFlag(ItemHasContents, true);
    m_socket = QSharedPointer<QLocalSocket>(new QLocalSocket());
    m_socket.data()->setServerName("ScreenPlay");
    QObject::connect(m_socket.data(), &QLocalSocket::connected, this, &ScreenPlaySDK::connected);
    QObject::connect(m_socket.data(), &QLocalSocket::disconnected, this, &ScreenPlaySDK::disconnected);
    QObject::connect(m_socket.data(), &QLocalSocket::bytesWritten, this, &ScreenPlaySDK::bytesWritten);
    QObject::connect(m_socket.data(), &QLocalSocket::readyRead, this, &ScreenPlaySDK::readyRead);
    QObject::connect(m_socket.data(), QOverload<QLocalSocket::LocalSocketError>::of(&QLocalSocket::error), this, &ScreenPlaySDK::error);
    m_socket.data()->connectToServer();
}

ScreenPlaySDK::~ScreenPlaySDK()
{
}

void ScreenPlaySDK::connected()
{
    setIsConnected(true);
    emit sdkConnected();
}

void ScreenPlaySDK::disconnected()
{
    setIsConnected(false);
    emit sdkDisconnected();
}

void ScreenPlaySDK::bytesWritten(qint64 bytes)
{

}

void ScreenPlaySDK::readyRead()
{
    QString tmp = m_socket.data()->readAll();
    QJsonParseError err;
    QJsonDocument jdoc = QJsonDocument::fromJson(QByteArray::fromStdString(tmp.toStdString()),&err);

    if (!(err.error == QJsonParseError::NoError)) {
        emit incommingMessageError(tmp);
        return;
    }
    emit incommingMessage(tmp);
}

void ScreenPlaySDK::error(QLocalSocket::LocalSocketError socketError)
{
    emit sdkSocketError("Error");

    if(socketError == QLocalSocket::LocalSocketError::ConnectionRefusedError){
        //QCoreApplication::quit();
    }
}
