// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

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

class LinuxX11Window : public BaseWindow {
    Q_OBJECT

public:
    ScreenPlay::WallpaperExitCode start() override;

signals:

public slots:
    void setVisible(bool show) override;
    void destroyThis() override;

private:
    QQuickView m_window;
    void setupWallpaperForOneScreen(int activeScreen);
    void setupWallpaperForAllScreens();
    void setupWallpaperForMultipleScreens(const QVector<int>& activeScreensList);
};
