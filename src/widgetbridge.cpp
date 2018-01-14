#include "widgetbridge.h"

WidgetBridge::WidgetBridge(QObject *parent) : QObject(parent)
{
    m_server = QSharedPointer<QLocalServer>(new QLocalServer(this));
    QObject::connect(m_server.data(), &QLocalServer::newConnection,this, &WidgetBridge::newConnection);

    if (!m_server.data()->listen("ScreenPlay")) {
        qDebug() << "SERVER: Server could not start";
    } else {
        qDebug() << "SERVER: Server started!";
    }
}

void WidgetBridge::newConnection()
{
    qDebug() << "SERVER: Client connected";

    QLocalSocket* socket = m_server.data()->nextPendingConnection();
    QObject::connect(socket,&QLocalSocket::disconnected,
                     socket,&QObject::deleteLater);

    socket->write("Hello client");
    socket->flush();

    socket->waitForBytesWritten(3000);
}
