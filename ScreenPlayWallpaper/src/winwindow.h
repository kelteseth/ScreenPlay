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
#include <QTimer>
#include <QVector>
#include <memory>

#include <qt_windows.h>

#include "basewindow.h"
#include "windowsdesktopproperties.h"

class WinWindow : public BaseWindow {
    Q_OBJECT

    Q_PROPERTY(WindowsDesktopProperties* windowsDesktopProperties READ windowsDesktopProperties WRITE setWindowsDesktopProperties NOTIFY windowsDesktopPropertiesChanged)

public:
    WindowsDesktopProperties* windowsDesktopProperties() const { return m_windowsDesktopProperties.get(); }

    ScreenPlay::WallpaperExitCode start() override;

public slots:
    void setVisible(bool show) override;
    void destroyThis() override;
    void terminate();
    void clearComponentCache() override;

    void setWindowsDesktopProperties(WindowsDesktopProperties* windowsDesktopProperties)
    {
        if (m_windowsDesktopProperties.get() == windowsDesktopProperties)
            return;

        m_windowsDesktopProperties.reset(windowsDesktopProperties);
        emit windowsDesktopPropertiesChanged(m_windowsDesktopProperties.get());
    }

signals:
    void windowsDesktopPropertiesChanged(WindowsDesktopProperties* windowsDesktopProperties);

private:
    void calcOffsets();
    void setupWallpaperForOneScreen(int activeScreen);
    void setupWallpaperForAllScreens();
    void setupWallpaperForMultipleScreens(const QVector<int>& activeScreensList);
    void setupWindowMouseHook();
    bool searchWorkerWindowToParentTo();
    void configureWindowGeometry();
    float getScaling(const int monitorIndex);
    bool hasWindowScaling();

private slots:
    void checkForFullScreenWindow();

private:
    QPoint m_zeroPoint {};
    QQuickView m_window;
    HWND m_windowHandle {};
    HWND m_windowHandleWorker {};
    QTimer m_checkForFullScreenWindowTimer;
    QTimer m_reconfigureTimer;
    std::unique_ptr<WindowsDesktopProperties> m_windowsDesktopProperties;
};
