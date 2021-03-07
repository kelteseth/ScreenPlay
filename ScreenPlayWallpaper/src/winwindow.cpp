#include "winwindow.h"
#include "WinUser.h"
#include "qqml.h"

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

HHOOK g_mouseHook;
QPoint g_LastMousePosition { 0, 0 };
QQuickView* g_winGlobalHook = nullptr;

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
        default:
            type = QMouseEvent::Type::MouseMove;
        }
    }

    POINT p {};
    if (GetCursorPos(&p)) {
        g_LastMousePosition.setX(p.x);
        g_LastMousePosition.setY(p.y);
    } else {
        return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
    }

    auto event = QMouseEvent(type, g_LastMousePosition, mouseButton, mouseButtons, keyboardModifier);

    QApplication::sendEvent(g_winGlobalHook, &event);

    if (type == QMouseEvent::Type::MouseButtonPress) {
        QTimer::singleShot(100, [=]() {
            auto eventPress = QMouseEvent(QMouseEvent::Type::MouseButtonPress, g_LastMousePosition, mouseButton, mouseButtons, {});
            qInfo() << mouseButton << QApplication::sendEvent(g_winGlobalHook, &eventPress);
            auto eventRelease = QMouseEvent(QMouseEvent::Type::MouseButtonRelease, g_LastMousePosition, mouseButton, mouseButtons, {});
            qInfo() << mouseButton << QApplication::sendEvent(g_winGlobalHook, &eventRelease);
        });
    }

    return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
}

void WinWindow::setupWindowMouseHook()
{
    using ScreenPlay::InstalledType::InstalledType;

    // MUST be called before setting hook for events!
    if (type() != InstalledType::VideoWallpaper && type() != InstalledType::GifWallpaper) {
        qInfo() << "Enable mousehook";
        g_winGlobalHook = &m_window;

        HINSTANCE hInstance = GetModuleHandle(NULL);
        if (!(g_mouseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseHookCallback, hInstance, 0))) {
            qInfo() << "Faild to install mouse hook!";
            return;
        }
        qInfo() << "Setup mousehook";
    }
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

    qRegisterMetaType<WindowsDesktopProperties*>();
    qRegisterMetaType<WinWindow*>();
    m_windowsDesktopProperties = std::make_unique<WindowsDesktopProperties>();
    m_windowHandle = reinterpret_cast<HWND>(m_window.winId());

    if (!IsWindow(m_windowHandle)) {
        qFatal("Could not get a valid window handle!");
    }
    ShowWindow(m_windowHandleWorker, SW_HIDE);
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

    qmlRegisterSingletonInstance<WinWindow>("ScreenPlayWallpaper", 1, 0, "Wallpaper", this);

    // Instead of setting "renderType: Text.NativeRendering" every time
    // we can set it here once :)
    m_window.setTextRenderType(QQuickWindow::TextRenderType::NativeTextRendering);
    m_window.setResizeMode(QQuickView::ResizeMode::SizeRootObjectToView);
    m_window.setSource(QUrl("qrc:/Wallpaper.qml"));
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

struct sEnumInfo {
    int iIndex;
    HMONITOR hMonitor;
};

BOOL CALLBACK GetMonitorByIndex(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
{
    auto* info = (sEnumInfo*)dwData;
    if (--info->iIndex < 0) {
        info->hMonitor = hMonitor;
        return FALSE;
    }
    return TRUE;
}

void WinWindow::calcOffsets()
{

    MonitorRects monitors;

    qInfo() << "You have " << monitors.rcMonitors.size() << " monitors connected.";

    for (int i = 0; i < QApplication::screens().count(); i++) {
        QScreen* screen = QApplication::screens().at(i);
        if (screen->availableGeometry().x() < 0) {
            m_windowOffsetX += (screen->availableGeometry().x() * -1);
        }
        if (screen->availableGeometry().y() < 0) {
            m_windowOffsetY += (screen->availableGeometry().y() * -1);
        }
        auto monitor = monitors.rcMonitors.at(i);
        qInfo() << i << monitor;

        //        sEnumInfo info{};
        //        info.iIndex = i;
        //        info.hMonitor = nullptr;

        //        EnumDisplayMonitors(nullptr, nullptr, GetMonitorByIndex, (LPARAM)&info);
        //        if (info.hMonitor != nullptr) {
        //            //LPMONITORINFO monitorInfo;
        //            //bool success = GetMonitorInfoA(info.hMonitor, monitorInfo);
        //            qInfo() << i << info.iIndex ;
        //        }
    }
}

void WinWindow::setupWallpaperForOneScreen(int activeScreen)
{
    const QRect screenRect = QApplication::screens().at(activeScreen)->geometry();

    MonitorRects monitors;

    auto monitor = monitors.rcMonitors.at(2);
    qInfo() << activeScreen<<"monitorRect " << monitor.width() << monitor.height() << monitor.x() << monitor.y();

    if (!SetWindowPos(m_windowHandle, nullptr, monitor.x() + m_windowOffsetX - 1, monitor.y() + m_windowOffsetY - 1, monitor.width() + 2, monitor.height() + 2, SWP_HIDEWINDOW)) {
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

BOOL CALLBACK GetMonitorByHandle(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
{
    Q_UNUSED(hdcMonitor)
    Q_UNUSED(lprcMonitor)

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

    QApplication::quit();
}

void WinWindow::clearComponentCache()
{
    m_window.engine()->clearComponentCache();
}
