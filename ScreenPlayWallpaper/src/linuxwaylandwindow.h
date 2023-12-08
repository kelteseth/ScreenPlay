// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#pragma once

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

namespace ScreenPlay {
class LinuxWaylandWindow : public BaseWindow {
    Q_OBJECT

public:
    WallpaperExit::Code start() override;

signals:

public slots:
    void setVisible(bool show) override;
    void destroyThis() override;
    void terminate() override;

private:
    QQuickView m_window;
    void setupWallpaperForOneScreen(int activeScreen);
    void setupWallpaperForAllScreens();
    void setupWallpaperForMultipleScreens(const QVector<int>& activeScreensList);
};
}