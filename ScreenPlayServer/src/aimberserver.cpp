#include "aimberserver.h"
//#include <google/protobuf/message_lite.h>
//#include <google/protobuf/any.pb.h>.


AimberServer::AimberServer(int port, QObject* parent) : QObject(parent)
{
    /** --------- SSL --------- **/
/*
    QSslConfiguration sslConfiguration;

    // Create a cert & key
    // https://developer.salesforce.com/blogs/developer-relations/2011/05/generating-valid-self-signed-certificates.html
    QFile certFile(QStringLiteral("server.crt"));
    QFile keyFile(QStringLiteral("server.key"));

    certFile.open(QIODevice::ReadOnly);
    keyFile.open(QIODevice::ReadOnly);

    QSslCertificate certificate(&certFile, QSsl::Pem);

    if(certificate.isNull())
    {
        qDebug() << "Invalid certificate!";
    }

    QSslKey sslKey(&keyFile, QSsl::Rsa,QSsl::Pem, QSsl::PrivateKey, QByteArray("1234"));
    if(sslKey.isNull())
    {
        qDebug() << "Invalid key!";
    }

    certFile.close();
    keyFile.close();
    sslConfiguration.setPeerVerifyMode(QSslSocket::VerifyNone);
    sslConfiguration.setLocalCertificate(certificate);
    sslConfiguration.setPrivateKey(sslKey);
    sslConfiguration.setProtocol(QSsl::SecureProtocols);

*/
    /** --------- SocketServer --------- **/

    m_pWebSocketServer = new QWebSocketServer(QStringLiteral("Community Login Server"), QWebSocketServer::NonSecureMode, this);

    //m_pWebSocketServer->setSslConfiguration(sslConfiguration);

    if (m_pWebSocketServer->listen(QHostAddress::LocalHost, port))
    {
        qDebug() << "SSL Echo Server listening on port" << port;
        connect(m_pWebSocketServer, &QWebSocketServer::newConnection, this, &AimberServer::onNewConnection);
        connect(m_pWebSocketServer, &QWebSocketServer::sslErrors, this, &AimberServer::onSslErrors);
    }

    //community::Login Login;
    //Login.set_password("test");
}

AimberServer::~AimberServer()
{
    m_pWebSocketServer->close();
    qDeleteAll(m_clients.begin(), m_clients.end());
}

void AimberServer::onNewConnection()
{
    QWebSocket* pSocket = m_pWebSocketServer->nextPendingConnection();

    qDebug() << "Client connected:" << pSocket->peerName() << pSocket->origin();

    connect(pSocket, &QWebSocket::textMessageReceived, this, &AimberServer::processTextMessage);
    connect(pSocket, &QWebSocket::binaryMessageReceived, this, &AimberServer::processBinaryMessage);
    connect(pSocket, &QWebSocket::disconnected, this, &AimberServer::socketDisconnected);

    m_clients << pSocket;
}

void AimberServer::processTextMessage(QString message)
{
    qDebug() << message;

    //google::protobuf::MessageLite m;
/*
    community::MessageContainer messageContainer;
    messageContainer.ParseFromString(message.toStdString());

    for (const google::protobuf::Any& cmessage : messageContainer.message())
    {
      if (cmessage.Is<community::Login>())
      {
        //NetworkErrorDetails network_error;
        //detail.UnpackTo(&network_error);
        //... processing network_error ...
      }
    }


    */
}

void AimberServer::processBinaryMessage(QByteArray message)
{
    qDebug() << "Binary Message Received";
}

void AimberServer::socketDisconnected()
{
    qDebug() << "Client disconnected";
    QWebSocket* pClient = qobject_cast<QWebSocket*>(sender());

    if (pClient)
    {
        m_clients.removeAll(pClient);
        pClient->deleteLater();
    }
}

void AimberServer::onSslErrors(const QList<QSslError>& errors)
{
    qDebug() << "SSL errors occurred: \n";

    for (int i = 0; i < errors.length(); ++i)
    {
        qDebug() << errors.at(i).errorString();
    }
}
