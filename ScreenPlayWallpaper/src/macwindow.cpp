// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#include "macwindow.h"

#include <QGuiApplication>

ScreenPlay::WallpaperExitCode MacWindow::start()
{
    auto* screen = QGuiApplication::screens().at(activeScreensList().at(0));
    m_window.setGeometry(screen->geometry());

    qmlRegisterSingletonInstance<MacWindow>("ScreenPlayWallpaper", 1, 0, "Wallpaper", this);

    if (!debugMode()) {
        connect(m_sdk.get(), &ScreenPlaySDK::sdkDisconnected, this, &MacWindow::destroyThis);
    }

    QDir workingDir(QGuiApplication::instance()->applicationDirPath());
    workingDir.cdUp();
    workingDir.cdUp();
    workingDir.cdUp();
    // OSX Development workaround:
    // This folder needs then to be copied into the .app/Contents/MacOS/
    // for the deploy version.
    m_window.engine()->addImportPath(workingDir.path() + "/qml");

    // WARNING: Setting Window flags must be called *here*!
    Qt::WindowFlags flags = m_window.flags();
    m_window.setFlags(flags | Qt::FramelessWindowHint | Qt::Desktop);
    m_window.setResizeMode(QQuickView::ResizeMode::SizeRootObjectToView);
    m_window.setTextRenderType(QQuickWindow::TextRenderType::NativeTextRendering);
    m_window.setSource(QUrl("qrc:/qml/ScreenPlayWallpaper/qml/Wallpaper.qml"));

    MacIntegration* macIntegration = new MacIntegration(this);
    macIntegration->SetBackgroundLevel(&m_window);

    return ScreenPlay::WallpaperExitCode::Ok;
}

void MacWindow::setVisible(bool show)
{
    m_window.setVisible(show);
}

void MacWindow::destroyThis()
{
    QCoreApplication::quit();
}

void MacWindow::terminate()
{
    destroyThis();
}

void MacWindow::clearComponentCache()
{
    m_window.engine()->clearComponentCache();
}

#include "moc_macwindow.cpp"
