#pragma once

#include <QApplication>
#include <QDebug>
#include <QObject>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickView>
#include <QScreen>
#include <QVector>
#include <memory>

#include "windowsdesktopproperties.h"
#include <QSettings>
#include <QString>

#include <qt_windows.h>

#include "basewindow.h"

class WinWindow : public BaseWindow {
    Q_OBJECT

public:
    explicit WinWindow(QVector<int>& activeScreensList, QString projectPath, QString id, QString volume);

public slots:
    void setVisible(bool show) override;
    void destroyThis() override;
    void messageReceived(QString key, QString value) override;

private:
    void calcOffsets();
    void setupWallpaperForOneScreen(int activeScreen);
    void setupWallpaperForAllScreens();
    bool searchWorkerWindowToParentTo();

private:
    int m_windowOffsetX = 0;
    int m_windowOffsetY = 0;
    QQuickView m_window;
    HWND m_windowHandle;
    HWND m_windowHandleWorker;
    WindowsDesktopProperties m_windowsDesktopProperties;
};
