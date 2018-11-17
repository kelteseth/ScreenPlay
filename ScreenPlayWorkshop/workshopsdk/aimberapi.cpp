#include "aimberapi.h"

AimberAPI::AimberAPI(QObject* parent)
    : QObject(parent)
{
}

void AimberAPI::openConnection()
{
    connect(&m_webSocket, &QWebSocket::connected, this, &AimberAPI::onConnected);
    connect(&m_webSocket, &QWebSocket::disconnected, this, &AimberAPI::closed);
    connect(&m_webSocket, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error),
        [=](QAbstractSocket::SocketError error){ qDebug() << "ConnectionError"; });

    QString url;
    url = "ws://localhost:16395";
    m_webSocket.open(QUrl(url));

    qDebug() << "OpenConnection";
}

void AimberAPI::onConnected()
{
    qDebug() << "Conneceted";

    connect(&m_webSocket, &QWebSocket::textMessageReceived,
            this, &AimberAPI::onTextMessageReceived);
    m_webSocket.sendTextMessage(QStringLiteral("Hello, world!"));
}

void AimberAPI::closed()
{
    qDebug() << "Closed";
}

void AimberAPI::onTextMessageReceived()
{
    m_webSocket.close();
}
