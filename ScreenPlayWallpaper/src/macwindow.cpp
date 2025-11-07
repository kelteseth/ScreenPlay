// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#include "macwindow.h"
#include "ScreenPlayCore/macutils.h"
#include "macintegration.h"

#include <QGuiApplication>

namespace ScreenPlay {
WallpaperExit::Code MacWindow::start()
{

    MacUtils::showDockIcon(false);
    auto* screen = QGuiApplication::screens().at(activeScreensList().at(0));
    m_quickView->setGeometry(screen->geometry());
    qRegisterMetaType<MacWindow*>();

    // OSX Development workaround:
    // This folder needs then to be copied into the .app/Contents/MacOS/
    // for the deploy version.
    m_quickView->engine()->addImportPath(QGuiApplication::instance()->applicationDirPath() + "/qml");

    // WARNING: Setting Window flags must be called *here*!
    Qt::WindowFlags flags = m_quickView->flags();
    m_quickView->setFlags(flags | Qt::FramelessWindowHint | Qt::Desktop);
    m_quickView->setResizeMode(QQuickView::ResizeMode::SizeRootObjectToView);

    // MacIntegration::enableSpaceChangeReapply(m_reapplySpacesEnabled);
    // Qt exposes the native NSView*/NSWindow* handle via winId(), so passing it through as void* is safe here.
    void* cocoaObject = reinterpret_cast<void*>(m_quickView->winId());
    MacIntegration::setBackgroundLevel(cocoaObject);

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

void MacWindow::setReapplySpacesEnabled(bool enabled)
{
    m_reapplySpacesEnabled = enabled;
    MacIntegration::enableSpaceChangeReapply(m_reapplySpacesEnabled);
}
}
#include "moc_macwindow.cpp"
