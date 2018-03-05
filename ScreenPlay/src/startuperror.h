#pragma once

#include <QObject>
#include <QQmlApplicationEngine>

class StartupError : public QObject {
    Q_OBJECT
public:
    explicit StartupError(QQmlApplicationEngine* main, QQmlApplicationEngine* err, QObject* parent = nullptr);

signals:

public slots:
    void startScreenPlayAnyway();

private:
    QQmlApplicationEngine *m_mainWindow, *m_errorWindow;
};
