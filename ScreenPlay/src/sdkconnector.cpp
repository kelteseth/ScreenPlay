#include "sdkconnector.h"

SDKConnector::SDKConnector(QObject* parent)
    : QObject(parent)
{
    m_server = QSharedPointer<QLocalServer>(new QLocalServer(this));
    connect(m_server.data(), &QLocalServer::newConnection, this, &SDKConnector::newConnection);

    if (!m_server.data()->listen("ScreenPlay")) {
        qDebug() << "SERVER: Server could not start";
    } else {
        qDebug() << "SERVER: Server started!";
    }
}

void SDKConnector::newConnection()
{
    QLocalSocket* socket = m_server.data()->nextPendingConnection();
    m_clients.append(socket);

    connect(socket,&QLocalSocket::readyRead, [&]() {
        qDebug() << socket->readAll();
    });

//    connect(socket,&QLocalSocket::stateChanged, [&]() {
//        switch (socket->state()) {
//        case QLocalSocket::UnconnectedState:;
//            break;
//        case QLocalSocket::ConnectingState:;
//            break;
//        case QLocalSocket::ConnectedState:;
//            break;
//        case QLocalSocket::ClosingState:;
//            break;
//        }
//    });
}



