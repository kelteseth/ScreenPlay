#include "macwindow.h"

MacWindow::MacWindow(
        const QVector<int> &activeScreensList,
        const QString &projectPath,
        const QString &id,
        const QString &volume,
        const QString &fillmode)
    : BaseWindow(projectPath, activeScreensList, false)
{
    setAppID(id);
    bool ok = false;
    float volumeParsed = volume.toFloat(&ok);
    if (!ok) {
        qFatal("Could not parse volume");
    }
    setVolume(volumeParsed);

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



    // WARNING: Setting Window flags must be called *here*!
    Qt::WindowFlags flags = m_window.flags();
    m_window.setFlags(flags | Qt::FramelessWindowHint | Qt::Desktop);

    m_window.setResizeMode(QQuickView::ResizeMode::SizeRootObjectToView);
    m_window.rootContext()->setContextProperty("window", this);
    // Instead of setting "renderType: Text.NativeRendering" every time

    // we can set it here once :)
    m_window.setTextRenderType(QQuickWindow::TextRenderType::NativeTextRendering);
    m_window.setSource(QUrl("qrc:/mainWindow.qml"));

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
