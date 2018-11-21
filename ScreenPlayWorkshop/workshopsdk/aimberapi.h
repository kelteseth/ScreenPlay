#pragma once

#include <QDebug>
#include <QObject>
#include <QtWebSockets/QWebSocket>
//#include "../../Common/ProtocolBuffer/GenCode/account.pb.h"

class AimberAPI : public QObject {
    Q_OBJECT
public:
    explicit AimberAPI(QObject* parent = nullptr);
    void openConnection();
signals:

public slots:

    void onConnected();
    void closed();

    void onTextMessageReceived();
private:
    QWebSocket m_webSocket;
};
