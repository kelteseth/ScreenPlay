// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#include "linuxwaylandwindow.h"
#include <QScreen>
#include <QGuiApplication>

#include <LayerShellQt/Window>
#include <LayerShellQt/Shell>

ScreenPlay::WallpaperExitCode LinuxWaylandWindow::start()
{

    if (!debugMode()) {
        connect(m_sdk.get(), &ScreenPlaySDK::sdkDisconnected, this, &LinuxWaylandWindow::destroyThis);
    }

    qmlRegisterSingletonInstance<LinuxWaylandWindow>("ScreenPlayWallpaper", 1, 0, "Wallpaper", this);

    QDir workingDir(QGuiApplication::instance()->applicationDirPath());
    m_window.engine()->addImportPath(workingDir.path() + "/qml");
    m_window.setResizeMode(QQuickView::ResizeMode::SizeRootObjectToView);
    m_window.setSource(QUrl("qrc:/qml/ScreenPlayWallpaper/qml/Wallpaper.qml"));

    // Get the Wayland display
    if (QGuiApplication::platformName() == "wayland") {
        QPlatformNativeInterface *native = QGuiApplication::platformNativeInterface();
        

    auto *layerShell = LayerShellQt::Window::get(&m_window);
    layerShell->setLayer(LayerShellQt::Window::LayerBackground);
    layerShell->setAnchors(static_cast<QFlags<LayerShellQt::Window::Anchor>>(
        LayerShellQt::Window::Anchor::AnchorTop |
        LayerShellQt::Window::Anchor::AnchorBottom |
        LayerShellQt::Window::Anchor::AnchorLeft |
        LayerShellQt::Window::Anchor::AnchorRight
    ));


   
    }

    m_window.show();
    return ScreenPlay::WallpaperExitCode::Ok;
}

void LinuxWaylandWindow::setupWallpaperForOneScreen(int activeScreen)
{
}

void LinuxWaylandWindow::setupWallpaperForAllScreens()
{
}

void LinuxWaylandWindow::setupWallpaperForMultipleScreens(const QVector<int>& activeScreensList)
{
}

void LinuxWaylandWindow::setVisible(bool show)
{
    m_window.setVisible(show);
}

void LinuxWaylandWindow::destroyThis()
{
    QCoreApplication::quit();
}

void LinuxWaylandWindow::terminate()
{
    QCoreApplication::quit();
}