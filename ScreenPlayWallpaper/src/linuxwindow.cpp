#include "linuxwindow.h"

LinuxWindow::LinuxWindow(
    const QVector<int>& activeScreensList,
    const QString& projectFilePath,
    const QString& appID,
    const QString& volume,
    const QString& fillmode,
    const QString& type,
    const bool checkWallpaperVisible,
    const bool debugMode,
    QObject* parent)
    : BaseWindow(
        activeScreensList,
        projectFilePath,
        type,
        checkWallpaperVisible,
        appID,
        debugMode)
{
    m_window.setWidth(1920);
    m_window.setHeight(1080);

    m_window.show();

    bool ok = false;
    float volumeParsed = volume.toFloat(&ok);
    if (!ok) {
        qFatal("Could not parse volume");
    }

    setVolume(volumeParsed);
    setFillMode(fillmode);

    // Ether for one Screen or for all
    if ((QApplication::screens().length() == activeScreensList.length()) && (activeScreensList.length() != 1)) {
        setupWallpaperForAllScreens();
    } else if (activeScreensList.length() == 1) {
        setupWallpaperForOneScreen(activeScreensList.at(0));
        setCanFade(true);
    } else if (activeScreensList.length() > 1) {
        setupWallpaperForMultipleScreens(activeScreensList);
    }

    setWidth(m_window.width());
    setHeight(m_window.height());

    m_window.setResizeMode(QQuickView::ResizeMode::SizeRootObjectToView);
    qmlRegisterSingletonInstance<LinuxWindow>("ScreenPlayWallpaper", 1, 0, "Wallpaper", this);
    m_window.setTextRenderType(QQuickWindow::TextRenderType::NativeTextRendering);
    m_window.setSource(QUrl("qrc:/ScreenPlayWallpaper/qml/Wallpaper.qml"));
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
