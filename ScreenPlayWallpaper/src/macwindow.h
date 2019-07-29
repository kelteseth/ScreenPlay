#pragma once

#include <QApplication>
#include <QDebug>
#include <QObject>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickView>
#include <QScreen>
#include <QSettings>
#include <QString>
#include <QVector>

#include "basewindow.h"

class MacWindow : public BaseWindow
{
    Q_OBJECT
public:
    explicit MacWindow(QVector<int>& activeScreensList, QString projectPath, QString id, QString volume,QObject *parent = nullptr);

signals:

public slots:
    void setVisible(bool show) override;
    void destroyThis() override;
    void messageReceived(QString key, QString value) override;
private:
    QQuickView m_window;
};

