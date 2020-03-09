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
#include <QTimer>
#include <QVector>
#include <memory>

#include <qt_windows.h>

#include "basewindow.h"
#include "windowsdesktopproperties.h"



class WinWindow : public BaseWindow {
    Q_OBJECT

public:
    explicit WinWindow(const QVector<int>& activeScreensList, const QString& projectPath, const QString& id, const QString& volume, const QString& fillmode, const bool checkWallpaperVisible);

public slots:
    void setVisible(bool show) override;
    void destroyThis() override;
    void terminate();
    void clearComponentCache();

private:
    void calcOffsets();
    void setupWallpaperForOneScreen(int activeScreen);
    void setupWallpaperForAllScreens();
    void setupWallpaperForMultipleScreens(const QVector<int>& activeScreensList);
    void setupWindowMouseHook();
    bool searchWorkerWindowToParentTo();

private slots:
    void checkForFullScreenWindow();

private:
    int m_windowOffsetX = 0;
    int m_windowOffsetY = 0;
    QQuickView m_window;
    HWND m_windowHandle;
    HWND m_windowHandleWorker;
    QTimer m_checkForFullScreenWindowTimer;
    WindowsDesktopProperties m_windowsDesktopProperties;
};
