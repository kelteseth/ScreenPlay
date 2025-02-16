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
class LinuxX11Window : public BaseWindow {
    Q_OBJECT

public:
    WallpaperExit::Code start() override;

signals:

public slots:
    void setVisible(bool show) override;
    void destroyThis() override;
    void terminate() override;

private:
    void setupWallpaperForOneScreen(int activeScreen);
    void setupWallpaperForAllScreens();
    void setupWallpaperForMultipleScreens(const QVector<int>& activeScreensList);
};

}