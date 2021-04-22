#include "macwindow.h"

MacWindow::MacWindow(
        const QVector<int>& activeScreensList,
        const QString& projectFilePath,
        const QString& appID,
        const QString& volume,
        const QString& fillmode,
        const QString& type,
        const bool checkWallpaperVisible,
        const bool debugMode)
    : BaseWindow(
          activeScreensList,
              projectFilePath,
              type,
              checkWallpaperVisible,
              appID,
              debugMode)
{
    setAppID(appID);
    bool ok = false;
    float volumeParsed = volume.toFloat(&ok);
    if (!ok) {
        qFatal("Could not parse volume");
    }
    setVolume(volumeParsed);
    setFillMode(fillmode);

    // Ether for one Screen or for all
    if ((QApplication::screens().length() == activeScreensList.length()) && (activeScreensList.length() != 1)) {
        //setupWallpaperForAllScreens();
    } else if (activeScreensList.length() == 1) {
        //setupWallpaperForOneScreen(activeScreensList.at(0));
        auto* screen = QGuiApplication::screens().at(0);
        m_window.setWidth(screen->geometry().width());
        m_window.setHeight(screen->geometry().height());
    } else if (activeScreensList.length() > 1) {
        //setupWallpaperForMultipleScreens(activeScreensList);
    }

    qmlRegisterSingletonInstance<MacWindow>("ScreenPlayWallpaper", 1, 0, "Wallpaper", this);

    // WARNING: Setting Window flags must be called *here*!
    Qt::WindowFlags flags = m_window.flags();
    m_window.setFlags(flags | Qt::FramelessWindowHint | Qt::Desktop);
    m_window.setResizeMode(QQuickView::ResizeMode::SizeRootObjectToView);
    m_window.setTextRenderType(QQuickWindow::TextRenderType::NativeTextRendering);
    m_window.setSource(QUrl("qrc:/Wallpaper.qml"));

    MacIntegration* macIntegration = new MacIntegration(this);
    macIntegration->SetBackgroundLevel(&m_window);
}

void MacWindow::setVisible(bool show)
{
    m_window.setVisible(show);
}

void MacWindow::destroyThis()
{
    QCoreApplication::quit();
}
