// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#include "macwindow.h"
#include "ScreenPlayCore/macutils.h"

#include <QGuiApplication>

namespace ScreenPlay {
WallpaperExit::Code MacWindow::start()
{

    MacUtils::showDockIcon(false);
    auto* screen = QGuiApplication::screens().at(activeScreensList().at(0));
    m_quickView->setGeometry(screen->geometry());

    // qmlRegisterSingletonInstance<MacWindow>("ScreenPlayWallpaper", 1, 0, "Wallpaper", this);

    if (!debugMode()) {
        connect(m_sdk.get(), &ScreenPlaySDK::sdkDisconnected, this, &MacWindow::destroyThis);
    }

    // OSX Development workaround:
    // This folder needs then to be copied into the .app/Contents/MacOS/
    // for the deploy version.
    m_quickView->engine()->addImportPath(QGuiApplication::instance()->applicationDirPath() + "/qml");

    // WARNING: Setting Window flags must be called *here*!
    Qt::WindowFlags flags = m_quickView->flags();
    m_quickView->setFlags(flags | Qt::FramelessWindowHint | Qt::Desktop);
    m_quickView->setResizeMode(QQuickView::ResizeMode::SizeRootObjectToView);
    // m_quickView->loadFromModule("ScreenPlayWallpaper", "Wallpaper");

    MacIntegration* macIntegration = new MacIntegration(this);
    macIntegration->SetBackgroundLevel(&m_window);

    return WallpaperExit::Code::Ok;
}

void MacWindow::setVisible(bool show)
{
    m_quickView->setVisible(show);
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
    m_quickView->engine()->clearComponentCache();
}
}
#include "moc_macwindow.cpp"
