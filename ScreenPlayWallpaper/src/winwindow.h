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
#include <QApplication>
#include <memory>

#include <qt_windows.h>

#include "basewindow.h"
#include "windowsdesktopproperties.h"

class WinWindow : public BaseWindow {
    Q_OBJECT


public:
    explicit WinWindow(const QVector<int>& activeScreensList, QString projectPath, QString id, QString volume, const QString fillmode);

public slots:
    void setVisible(bool show) override;
    void destroyThis() override;
    void terminate();

private:
    void calcOffsets();
    void setupWallpaperForOneScreen(int activeScreen);
    void setupWallpaperForAllScreens();
    void setupWallpaperForMultipleScreens(const QVector<int>& activeScreensList);
    void setupWindowMouseHook();
    bool searchWorkerWindowToParentTo();


private:
    int m_windowOffsetX = 0;
    int m_windowOffsetY = 0;
    QQuickView m_window;
    HWND m_windowHandle;
    HWND m_windowHandleWorker;
    WindowsDesktopProperties m_windowsDesktopProperties;
};
