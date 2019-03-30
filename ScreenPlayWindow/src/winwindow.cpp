#include "winwindow.h"

BOOL WINAPI SearchForWorkerWindow(HWND hwnd, LPARAM lparam)
{
    // 0xXXXXXXX "" WorkerW
    //   ...
    //   0xXXXXXXX "" SHELLDLL_DefView
    //     0xXXXXXXXX "FolderView" SysListView32
    // 0xXXXXXXXX "" WorkerW                           <---- We want this one
    // 0xXXXXXXXX "Program Manager" Progman
    if (FindWindowExW(hwnd, nullptr, L"SHELLDLL_DefView", nullptr))
        *reinterpret_cast<HWND*>(lparam) = FindWindowExW(nullptr, hwnd, L"WorkerW", nullptr);
    return TRUE;
}

WinWindow::WinWindow(QVector<int>& activeScreensList, QString projectPath, QString id, QString volume)
    : BaseWindow(projectPath)
{
    m_windowHandle = reinterpret_cast<HWND>(m_window.winId());

    if(!IsWindow(m_windowHandle)){
        qFatal("Could not get a valid window handle!");
    }
    setAppID(id);

    bool ok = false;
    float volumeParsed = volume.toFloat(&ok);
    if (!ok) {
        qFatal("Could not parse volume");
    }
    setVolume(volumeParsed);

    if (!searchWorkerWindowToParentTo()) {
        qFatal("No worker window found");
    }

    // WARNING: Setting Window flags must be called *here*!
    Qt::WindowFlags flags = m_window.flags();
    m_window.setFlags(flags | Qt::FramelessWindowHint);
    SetWindowLongPtr(m_windowHandle, GWL_STYLE, WS_CHILDWINDOW);
    SetWindowLongPtr(m_windowHandle, GWL_EXSTYLE, WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOACTIVATE);

    // Windows coordante system begins at 0x0 at the
    // main monitors upper left and not at the most left top monitor
    calcOffsets();

    // Ether for one Screen or for all
    if ((QApplication::screens().length() == activeScreensList.length()) && (activeScreensList.length() != 1)) {
        setupWallpaperForAllScreens();
    } else if (activeScreensList.length() == 1) {
        setupWallpaperForOneScreen(activeScreensList.at(0));
    }

    m_window.setResizeMode(QQuickView::ResizeMode::SizeRootObjectToView);
    m_window.rootContext()->setContextProperty("window", this);
    m_window.rootContext()->setContextProperty("desktopProperties", &m_windowsDesktopProperties);
    // Instead of setting "renderType: Text.NativeRendering" every time
    // we can set it here once :)
    m_window.setTextRenderType(QQuickWindow::TextRenderType::NativeTextRendering);
    m_window.setSource(QUrl("qrc:/mainWindow.qml"));

    // Let QML decide when were are read to show the window
    ShowWindow(m_windowHandle, SW_HIDE);

}

void WinWindow::setVisible(bool show)
{

    if (show) {
        ShowWindow(m_windowHandle, SW_SHOW);
        if(!IsWindowVisible(m_windowHandle)){
            qFatal("Could net set window visible!");
        }

    } else {
        ShowWindow(m_windowHandle, SW_HIDE);
    }
}

void WinWindow::destroyThis()
{
    ShowWindow(m_windowHandle, SW_HIDE);

    // Force refresh so that we display the regular
    // desktop wallpaper again
    ShowWindow(m_windowHandleWorker, SW_HIDE);
    ShowWindow(m_windowHandleWorker, SW_SHOW);

    QCoreApplication::quit();
}

void WinWindow::messageReceived(QString key, QString value)
{
}

void WinWindow::calcOffsets()
{
    for (int i = 0; i < QApplication::screens().count(); i++) {
        QScreen* screen = QApplication::screens().at(i);
        if (screen->availableGeometry().x() < 0) {
            m_windowOffsetX += (screen->availableGeometry().x() * -1);
        }
        if (screen->availableGeometry().y() < 0) {
            m_windowOffsetY += (screen->availableGeometry().y() * -1);
        }
    }
}

void WinWindow::setupWallpaperForOneScreen(int activeScreen)
{
    QScreen* screen = QApplication::screens().at(activeScreen);
    QRect screenRect = screen->geometry();
    m_window.setHeight(screenRect.height());
    m_window.setWidth(screenRect.width());
    if (!SetWindowPos(m_windowHandle, nullptr, screenRect.x() + m_windowOffsetX, screenRect.y() + m_windowOffsetY, screenRect.width(), screenRect.height(), SWP_SHOWWINDOW)) {
        qFatal("Could not set window pos: ");
    }
    if (SetParent(m_windowHandle, m_windowHandleWorker) == nullptr) {
        qFatal("Could not attach to parent window");
    }
}

void WinWindow::setupWallpaperForAllScreens()
{
    QRect rect;
    for (int i = 0; i < QApplication::screens().count(); i++) {
        QScreen* screenTmp = QApplication::screens().at(i);
        rect.setWidth(rect.width() + screenTmp->geometry().width());
        rect.setHeight(rect.height() + screenTmp->geometry().height());
    }
    m_window.setHeight(rect.height());
    m_window.setWidth(rect.width());
    if (!SetWindowPos(m_windowHandle, nullptr, 0, 0, rect.width(), rect.height(), SWP_SHOWWINDOW)) {
        qFatal("Could not set window pos: ");
    }
    if (SetParent(m_windowHandle, m_windowHandleWorker) == nullptr) {
        qFatal("Could not attach to parent window");
    }
}

bool WinWindow::searchWorkerWindowToParentTo()
{
    HWND progman_hwnd = FindWindowW(L"Progman", L"Program Manager");
    const DWORD WM_SPAWN_WORKER = 0x052C;
    SendMessageTimeoutW(progman_hwnd, WM_SPAWN_WORKER, 0xD, 0x1, SMTO_NORMAL,
        10000, nullptr);

    return EnumWindows(SearchForWorkerWindow, reinterpret_cast<LPARAM>(&m_windowHandleWorker));
}
