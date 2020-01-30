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

class LinuxWindow : public BaseWindow
{
    Q_OBJECT
public:
    explicit LinuxWindow(QVector<int> activeScreensList, QString projectPath, QString id, QString volume, const QString fillmode,QObject *parent = nullptr);

signals:

public slots:
    void setVisible(bool show) override;
    void destroyThis() override;
private:
    QQuickView m_window;
};

