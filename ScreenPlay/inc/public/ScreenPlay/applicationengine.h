#pragma once

#include <QByteArray>
#include <QGuiApplication>
#include <QLocalSocket>
#include <QObject>
#include <QQmlApplicationEngine>

namespace ScreenPlay {

class ApplicationEngine : public QQmlApplicationEngine {
    Q_OBJECT
public:
    explicit ApplicationEngine(QObject* parent = nullptr);
    bool isAnotherScreenPlayInstanceRunning();
    void init();
};
}
