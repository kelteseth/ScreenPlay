#pragma once

#include <QDebug>
#include <QFile>
#include <QObject>
#include <QString>
#include <QVector>
#include <QList>
#include <QtCore/QByteArray>
#include <QtNetwork/QSslCertificate>
#include <QtNetwork/QSslError>
#include <QtNetwork/QSslKey>
#include <QtWebSockets/QWebSocketServer>
#include <QtWebSockets/QWebSocket>
//#include "Source/ProtocolBufferSub/GenCode/container.pb.h"
//#include "Source/ProtocolBufferSub/GenCode/account.pb.h"

class AimberServer : public QObject
{
    Q_OBJECT
public:
    explicit AimberServer(int port, QObject* parent = nullptr);
    ~AimberServer();

signals:

private slots:
    void onNewConnection();
    void processTextMessage(QString message);
    void processBinaryMessage(QByteArray message);
    void socketDisconnected();
    void onSslErrors(const QList<QSslError>& errors);

public slots:

private:
    QWebSocketServer* m_pWebSocketServer = nullptr;
    QVector<QWebSocket*> m_clients;

};
