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
    explicit MacWindow(
            const QVector<int>& activeScreensList,
            const QString& projectPath,
            const QString& id,
            const QString& volume,
            const QString& fillmode);

signals:

public slots:
    void setVisible(bool show) override;
    void destroyThis() override;
private:
    QQuickView m_window;
};

