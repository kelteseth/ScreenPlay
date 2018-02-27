#pragma once

#include <QObject>
#include <QLocalSocket>
#include <QLocalServer>
#include <QSharedPointer>

class WidgetBridge : public QObject {
    Q_OBJECT
public:
    explicit WidgetBridge(QObject* parent = nullptr);

signals:

public slots:
    void newConnection();

private:
    QSharedPointer<QLocalServer> m_server;
};

