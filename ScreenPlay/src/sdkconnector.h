#pragma once

#include <QObject>
#include <QLocalSocket>
#include <QLocalServer>
#include <QSharedPointer>
#include <QVector>
#include <QTimer>

class SDKConnector : public QObject
{
    Q_OBJECT
public:
    explicit SDKConnector(QObject *parent = nullptr);

signals:

public slots:
    void newConnection();
    void closeAllWallpapers();

private:
    QSharedPointer<QLocalServer> m_server;
    QVector<QLocalSocket*> m_clients;

};

