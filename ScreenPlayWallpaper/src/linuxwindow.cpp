// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#include "linuxwindow.h"


ScreenPlay::WallpaperExitCode LinuxWindow::start()
{
        
    if(!debugMode()){
        connect(m_sdk.get(), &ScreenPlaySDK::sdkDisconnected, this, &LinuxWindow::destroyThis);
    }

    Qt::WindowFlags flags = m_window.flags();
    m_window.setFlags(flags | Qt::FramelessWindowHint | Qt::Desktop);
    auto* screen = QGuiApplication::screens().at(activeScreensList().at(0));
    m_window.setGeometry(screen->geometry());
    m_window.show();

    m_window.setResizeMode(QQuickView::ResizeMode::SizeRootObjectToView);
    qmlRegisterSingletonInstance<LinuxWindow>("ScreenPlayWallpaper", 1, 0, "Wallpaper", this);
    m_window.setTextRenderType(QQuickWindow::TextRenderType::NativeTextRendering);
    m_window.setSource(QUrl("qrc:/ScreenPlayWallpaper/qml/Wallpaper.qml"));
    return ScreenPlay::WallpaperExitCode::Ok;
}

void LinuxWindow::setupWallpaperForOneScreen(int activeScreen)
{
}

void LinuxWindow::setupWallpaperForAllScreens()
{
}

void LinuxWindow::setupWallpaperForMultipleScreens(const QVector<int>& activeScreensList)
{
}

void LinuxWindow::setVisible(bool show)
{
    m_window.setVisible(show);
}

void LinuxWindow::destroyThis()
{
    QCoreApplication::quit();
}
