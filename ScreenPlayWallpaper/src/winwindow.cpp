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

HHOOK mouseHook;

QQuickView* winGlobalHook = nullptr;

LRESULT __stdcall MouseHookCallback(int nCode, WPARAM wParam, LPARAM lParam)
{
    Qt::MouseButton mouseButton {};
    Qt::MouseButtons mouseButtons {};
    Qt::KeyboardModifier keyboardModifier {};
    QMouseEvent::Type type { QMouseEvent::Type::MouseMove };

    if (nCode >= 0) {
        switch (wParam) {
        case WM_LBUTTONDOWN:
            mouseButton = Qt::MouseButton::LeftButton;
            mouseButtons.setFlag(Qt::LeftButton);
            type = QMouseEvent::Type::MouseButtonPress;
            break;
        case WM_LBUTTONUP:
            mouseButton = Qt::MouseButton::LeftButton;
            mouseButtons.setFlag(Qt::LeftButton);
            type = QMouseEvent::Type::MouseButtonRelease;
            break;
        case WM_RBUTTONDOWN:
            mouseButton = Qt::MouseButton::RightButton;
            mouseButtons.setFlag(Qt::RightButton);
            type = QMouseEvent::Type::MouseButtonPress;
            break;
        }
    }

    POINT p {};
    QPoint point { 0, 0 };
    if (GetCursorPos(&p)) {
        point.setX(p.x);
        point.setY(p.y);
    }

    auto event = QMouseEvent(type, point, mouseButton, mouseButtons, keyboardModifier);

    auto* app = QApplication::instance();

    app->sendEvent(winGlobalHook, &event);

    return CallNextHookEx(mouseHook, nCode, wParam, lParam);
}

WinWindow::WinWindow(const QVector<int> &activeScreensList, QString projectPath, QString id, QString volume, const QString fillmode)
    : BaseWindow(projectPath)
{
    m_windowHandle = reinterpret_cast<HWND>(m_window.winId());

    if (!IsWindow(m_windowHandle)) {
        qFatal("Could not get a valid window handle!");
    }
    setAppID(id);

    bool ok = false;
    float volumeParsed = volume.toFloat(&ok);
    if (!ok) {
        qFatal("Could not parse volume");
    }
    setVolume(volumeParsed);
    setFillMode(fillmode);

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
        qDebug()  << "setupWallpaperForAllScreens()";
    } else if (activeScreensList.length() == 1) {
        setupWallpaperForOneScreen(activeScreensList.at(0));
        setCanFade(true);
        qDebug()  << "setupWallpaperForOneScreen()";
    }  else if (activeScreensList.length() == 1) {
        setupWallpaperForMultipleScreens(activeScreensList);
        qDebug()  << "setupWallpaperForMultipleScreens()";
    }

    m_window.setResizeMode(QQuickView::ResizeMode::SizeRootObjectToView);
    m_window.rootContext()->setContextProperty("window", this);
    m_window.rootContext()->setContextProperty("desktopProperties", &m_windowsDesktopProperties);
    // Instead of setting "renderType: Text.NativeRendering" every time
    // we can set it here once :)
    m_window.setTextRenderType(QQuickWindow::TextRenderType::NativeTextRendering);
    m_window.setSource(QUrl("qrc:/mainWindow.qml"));

    // MUST be called before setting hook for events!
    if(type() == BaseWindow::WallpaperType::Qml){
        winGlobalHook = &m_window;
        if (!(mouseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseHookCallback, nullptr, 0))) {
            qDebug() << "Faild to install mouse hook!";
        }
    }

    // FIXME WORKAROUND:
    // There is a strange bug when we open the ScreenPlayWallpaper project on its one
    // that if we set ShowWindow(m_windowHandle, SW_HIDE); we can no longer set
    // the window visible via set Visible.
    if (projectPath != "test") {
        // Let QML decide when were are read to show the window
       // ShowWindow(m_windowHandle, SW_HIDE);
    }
}

void WinWindow::setVisible(bool show)
{
    if (show) {
        if (!ShowWindow(m_windowHandle, SW_SHOW)) {
            qDebug() << "Cannot set window handle visible";
        }
        m_window.show();
    } else {
        if (!ShowWindow(m_windowHandle, SW_HIDE)) {
            qDebug() << "Cannot set window handle hidden";
        }
    }
}

void WinWindow::destroyThis()
{
    emit qmlExit();
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

void WinWindow::setupWallpaperForMultipleScreens(const QVector<int> &activeScreensList)
{
    qDebug() << "######## setupWallpaperForMultipleScreens ########";

}

bool WinWindow::searchWorkerWindowToParentTo()
{
    HWND progman_hwnd = FindWindowW(L"Progman", L"Program Manager");
    const DWORD WM_SPAWN_WORKER = 0x052C;
    SendMessageTimeoutW(progman_hwnd, WM_SPAWN_WORKER, 0xD, 0x1, SMTO_NORMAL,
        10000, nullptr);

    return EnumWindows(SearchForWorkerWindow, reinterpret_cast<LPARAM>(&m_windowHandleWorker));
}

void WinWindow::terminate()
{
    ShowWindow(m_windowHandle, SW_HIDE);

    // Force refresh so that we display the regular
    // desktop wallpaper again
    ShowWindow(m_windowHandleWorker, SW_HIDE);
    ShowWindow(m_windowHandleWorker, SW_SHOW);

    QCoreApplication::quit();
}
