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

class LinuxWindow : public BaseWindow {
    Q_OBJECT
public:
    explicit LinuxWindow(
        const QVector<int>& activeScreensList,
        const QString& projectFilePath,
        const QString& appID,
        const QString& volume,
        const QString& fillmode,
        const QString& type,
        const bool checkWallpaperVisible,
        const bool debugMode,
        QObject* parent = nullptr);

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
