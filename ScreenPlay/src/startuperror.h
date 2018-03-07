#pragma once

#include <QObject>
#include <QQmlApplicationEngine>

/*!
    \class Startup Error
    \brief Helper to show the user that steam is not running. When the user decides to run ScreenPlay anyway
    it destructs the error window and creates the main window.

*/

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
