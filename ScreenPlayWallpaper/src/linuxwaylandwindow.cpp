// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#include "linuxwaylandwindow.h"
#include <QGuiApplication>
#include <QScreen>

#include <LayerShellQt/Shell>
#include <LayerShellQt/Window>

namespace ScreenPlay {
WallpaperExit::Code LinuxWaylandWindow::start()
{

    // if (QGuiApplication::platformName() != "wayland") {
    //     qFatal("Invalid");
    // }

    // if (!debugMode()) {
    //     connect(m_sdk.get(), &ScreenPlaySDK::sdkDisconnected, this, &LinuxWaylandWindow::destroyThis);
    // }
       // LinuxWaylandWindow* singleton = singletonInstance<LinuxWaylandWindow*>("ScreenPlayWallpaper", "Wallpaper");
    // QDir workingDir(QGuiApplication::instance()->applicationDirPath());
    // // m_quickView->engine()->addImportPath(workingDir.path() + "/qml");
     m_quickView->setResizeMode(QQuickView::ResizeMode::SizeRootObjectToView);
    //  m_quickView->loadFromModule("ScreenPlayWallpaper", "ScreenPlayWallpaperMain");
    //m_quickView->setGeometry(0,0,100,100);

    // Get the Wayland display
    // QPlatformNativeInterface* native = QGuiApplication::platformNativeInterface();

     auto* layerShell = LayerShellQt::Window::get(m_quickView.get());
     layerShell->setLayer(LayerShellQt::Window::LayerBackground);
     layerShell->setAnchors(static_cast<QFlags<LayerShellQt::Window::Anchor>>(
         LayerShellQt::Window::Anchor::AnchorTop | LayerShellQt::Window::Anchor::AnchorBottom | LayerShellQt::Window::Anchor::AnchorLeft | LayerShellQt::Window::Anchor::AnchorRight));


    m_quickView->show();
     return WallpaperExit::Code::Ok;
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
    // m_quickView->setVisible(show);
}

void LinuxWaylandWindow::destroyThis()
{
    QCoreApplication::quit();
}

void LinuxWaylandWindow::terminate()
{
    QCoreApplication::quit();
}
}
