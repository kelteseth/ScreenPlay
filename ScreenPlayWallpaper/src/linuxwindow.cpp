#include "linuxwindow.h"


LinuxWindow::LinuxWindow(QVector<int> activeScreensList, QString projectPath, QString id, QString volume, const QString fillmode, const bool checkWallpaperVisible, QObject* parent)
    : BaseWindow(projectPath, activeScreensList, checkWallpaperVisible)
{
    m_window.setWidth(1920);
    m_window.setHeight(1080);

    m_window.show();

    setAppID(id);

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
    m_window.rootContext()->setContextProperty("window", this);
    //m_window.rootContext()->setContextProperty("desktopProperties", &m_windowsDesktopProperties);
    // Instead of setting "renderType: Text.NativeRendering" every time
    // we can set it here once :)

    m_window.setTextRenderType(QQuickWindow::TextRenderType::NativeTextRendering);
    m_window.setSource(QUrl("qrc:/mainWindow.qml"));



    // WARNING: Setting Window flags must be called *here*!
    Qt::WindowFlags flags = m_window.flags();
    m_window.setFlags(flags | Qt::FramelessWindowHint | Qt::Desktop);

    m_window.setResizeMode(QQuickView::ResizeMode::SizeRootObjectToView);
    m_window.rootContext()->setContextProperty("window", this);
    // Instead of setting "renderType: Text.NativeRendering" every time
    // we can set it here once :)
    m_window.setTextRenderType(QQuickWindow::TextRenderType::NativeTextRendering);
    m_window.setSource(QUrl("qrc:/Wallpaper.qml"));
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
