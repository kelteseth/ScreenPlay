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

    if (type == QMouseEvent::Type::MouseButtonPress) {
        QTimer::singleShot(100, []() {
            Qt::MouseButton mouseButton {};
            Qt::MouseButtons mouseButtons {};
            Qt::KeyboardModifier keyboardModifier {};
            auto eventRelease = QMouseEvent(QMouseEvent::Type::MouseButtonRelease, { 0, 0 }, mouseButton, mouseButtons, keyboardModifier);

            auto* app = QApplication::instance();

            app->sendEvent(winGlobalHook, &eventRelease);
        });
    }

    return CallNextHookEx(mouseHook, nCode, wParam, lParam);
}

WinWindow::WinWindow(
    const QVector<int>& activeScreensList,
    const QString& projectPath,
    const QString& id,
    const QString& volume,
    const QString& fillmode,
    const bool checkWallpaperVisible)
    : BaseWindow(projectPath, activeScreensList, checkWallpaperVisible)
{

    m_window.hide();
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
    SetWindowLongPtr(m_windowHandle, GWL_EXSTYLE, WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT);
    SetWindowLongPtr(m_windowHandle, GWL_STYLE, WS_POPUPWINDOW);

    // Windows coordante system begins at 0x0 at the
    // main monitors upper left and not at the most left top monitor
    calcOffsets();

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
    m_window.rootContext()->setContextProperty("desktopProperties", &m_windowsDesktopProperties);
    // Instead of setting "renderType: Text.NativeRendering" every time
    // we can set it here once :)

    m_window.setTextRenderType(QQuickWindow::TextRenderType::NativeTextRendering);
    m_window.setSource(QUrl("qrc:/mainWindow.qml"));
    m_window.hide();

    QObject::connect(&m_checkForFullScreenWindowTimer, &QTimer::timeout, this, &WinWindow::checkForFullScreenWindow);

    // We do not support autopause for multi monitor wallpaper
    if (this->activeScreensList().length() == 1) {
        if (checkWallpaperVisible) {
            m_checkForFullScreenWindowTimer.start(10);
        }
    }

    QTimer::singleShot(1000, [this]() {
        setupWindowMouseHook();
    });
}

void WinWindow::setVisible(bool show)
{
    if (show) {
        if (!ShowWindow(m_windowHandle, SW_SHOW)) {
            qDebug() << "Cannot set window handle SW_SHOW";
        }
    } else {
        if (!ShowWindow(m_windowHandle, SW_HIDE)) {
            qDebug() << "Cannot set window handle SW_HIDE";
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
    if (!SetWindowPos(m_windowHandle, nullptr, screenRect.x() + m_windowOffsetX - 1, screenRect.y() + m_windowOffsetY - 1, screenRect.width() + 2, screenRect.height() + 2, SWP_SHOWWINDOW)) {
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
    if (!SetWindowPos(m_windowHandle, HWND_TOPMOST, 0, 0, rect.width(), rect.height(), SWP_NOSIZE | SWP_NOMOVE)) {
        qFatal("Could not set window pos: ");
    }
    if (SetParent(m_windowHandle, m_windowHandleWorker) == nullptr) {
        qFatal("Could not attach to parent window");
    }
}

void WinWindow::setupWallpaperForMultipleScreens(const QVector<int>& activeScreensList)
{
    QRect rect;
    QScreen* upperLeftScreen { nullptr };
    // Check for the upper left screen first so we get x and y positions
    for (const int screen : activeScreensList) {
        QScreen* screenTmp = QApplication::screens().at(screen);
        if (upperLeftScreen != nullptr) {
            if (screenTmp->geometry().x() < upperLeftScreen->geometry().x() || screenTmp->geometry().y() < upperLeftScreen->geometry().y()) {
                upperLeftScreen = screenTmp;
            }
        } else {
            upperLeftScreen = screenTmp;
        }
        rect.setWidth(screenTmp->geometry().width() + rect.width());
        rect.setHeight(screenTmp->geometry().height() + rect.height());
    }

    rect.setX(upperLeftScreen->geometry().x());
    rect.setY(upperLeftScreen->geometry().y());

    if (!SetWindowPos(m_windowHandle, nullptr, rect.x() + m_windowOffsetX, rect.y() + m_windowOffsetY, rect.width(), rect.height(), SWP_SHOWWINDOW)) {
        qFatal("Could not set window pos: ");
    }
    if (SetParent(m_windowHandle, m_windowHandleWorker) == nullptr) {
        qFatal("Could not attach to parent window");
    }
}

void WinWindow::setupWindowMouseHook()
{
    // MUST be called before setting hook for events!
    if (type() == BaseWindow::WallpaperType::Qml) {
        winGlobalHook = &m_window;
        if (!(mouseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseHookCallback, nullptr, 0))) {
            qDebug() << "Faild to install mouse hook!";
        }
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

QString printWindowNameByhWnd(HWND hWnd)
{
    std::wstring title(GetWindowTextLength(hWnd) + 1, L'\0');
    GetWindowTextW(hWnd, &title[0], title.size());
    return QString::fromStdWString(title);
}

BOOL CALLBACK FindTheDesiredWnd(HWND hWnd, LPARAM lParam)
{
    DWORD dwStyle = (DWORD)GetWindowLong(hWnd, GWL_STYLE);
    if ((dwStyle & WS_MAXIMIZE) != 0) {
        *(reinterpret_cast<HWND*>(lParam)) = hWnd;
        return false; // stop enumerating
    }
    return true; // keep enumerating
}

struct sEnumInfo {
    int iIndex = 0;
    HMONITOR hMonitor = NULL;
};

BOOL CALLBACK GetMonitorByHandle(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
{
    auto info = (sEnumInfo*)dwData;
    if (info->hMonitor == hMonitor)
        return FALSE;
    ++info->iIndex;
    return TRUE;
}

int GetMonitorIndex(HMONITOR hMonitor)
{
    sEnumInfo info;
    info.hMonitor = hMonitor;

    if (EnumDisplayMonitors(NULL, NULL, GetMonitorByHandle, (LPARAM)&info))
        return -1;

    return info.iIndex;
}

void WinWindow::checkForFullScreenWindow()
{

    HWND hFoundWnd = nullptr;
    EnumWindows(&FindTheDesiredWnd, reinterpret_cast<LPARAM>(&hFoundWnd));

    // True if one window has WS_MAXIMIZE
    if (hFoundWnd != nullptr) {
        DWORD dwFlags = 0;
        HMONITOR monitor = MonitorFromWindow(hFoundWnd, dwFlags);
        HMONITOR wallpaper = MonitorFromWindow(m_windowHandle, dwFlags);
        int monitorIndex = GetMonitorIndex(monitor);
        int wallpaperIndex = GetMonitorIndex(wallpaper);
       // qDebug() << monitorIndex << wallpaperIndex;

        // If the window that has WS_MAXIMIZE is at the same monitor as this wallpaper
        if (monitorIndex == wallpaperIndex) {

            setVisualsPaused(true);
        } else {
            setVisualsPaused(false);
        }

    } else {
        setVisualsPaused(false);
    }
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

void WinWindow::clearComponentCache()
{
    m_window.engine()->clearComponentCache();
}
