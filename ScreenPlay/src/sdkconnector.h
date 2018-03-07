#pragma once

#include <QObject>
#include <QLocalSocket>
#include <QLocalServer>
#include <QSharedPointer>
#include <QVector>
#include <QTimer>

/*!
    \class SDKConnector
    \brief Used for every Wallpaper, Scene or Widget communication via Windows pipes/QLocalSocket

*/

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

