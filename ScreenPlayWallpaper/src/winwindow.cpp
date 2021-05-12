#include "winwindow.h"
#include "WinUser.h"
#include "qqml.h"
#include <ShellScalingApi.h>
#include <algorithm>
#include <iostream>
#include <vector>
#include <windows.h>

struct WinMonitorStats {
    std::vector<int> iMonitors;
    std::vector<HMONITOR> hMonitors;
    std::vector<HDC> hdcMonitors;
    std::vector<RECT> rcMonitors;
    std::vector<DEVICE_SCALE_FACTOR> scaleFactor;
    std::vector<std::pair<UINT, UINT>> sizes;

    static BOOL CALLBACK MonitorEnum(HMONITOR hMon, HDC hdc, LPRECT lprcMonitor,
        LPARAM pData)
    {
        WinMonitorStats* pThis = reinterpret_cast<WinMonitorStats*>(pData);
        auto scaleFactor = DEVICE_SCALE_FACTOR::DEVICE_SCALE_FACTOR_INVALID;
        GetScaleFactorForMonitor(hMon, &scaleFactor);

        UINT x = 0;
        UINT y = 0;
        GetDpiForMonitor(hMon, MONITOR_DPI_TYPE::MDT_RAW_DPI, &x, &y);
        pThis->sizes.push_back({ x, y });
        pThis->scaleFactor.push_back(scaleFactor);
        pThis->hMonitors.push_back(hMon);
        pThis->hdcMonitors.push_back(hdc);
        pThis->rcMonitors.push_back(*lprcMonitor);
        pThis->iMonitors.push_back(pThis->hdcMonitors.size());
        return TRUE;
    }

    WinMonitorStats() { EnumDisplayMonitors(0, 0, MonitorEnum, (LPARAM)this); }
};

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
QPoint g_globalOffset { 0, 0 };
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
    }
    QTimer::singleShot(100, [&]() {
        //auto eventPress = QMouseEvent(QMouseEvent::Type::MouseButtonPress, g_LastMousePosition, mouseButton, mouseButtons, {});
        //qInfo() << mouseButton << QApplication::sendEvent(g_winGlobalHook, &eventPress) << g_globalOffset.x() << g_globalOffset.y();
        auto eventRelease = QMouseEvent(QMouseEvent::Type::MouseButtonRelease, g_LastMousePosition, mouseButton, mouseButtons, {});
        QApplication::sendEvent(g_winGlobalHook, &eventRelease);
    });

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
    }
}

WinWindow::WinWindow(
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
    auto* guiAppInst = dynamic_cast<QApplication*>(QApplication::instance());
    connect(this, &BaseWindow::reloadQML, this, [this]() {
        clearComponentCache();
    });
    connect(guiAppInst, &QApplication::screenAdded, this, &WinWindow::configureWindowGeometry);
    connect(guiAppInst, &QApplication::screenRemoved, this, &WinWindow::configureWindowGeometry);
    connect(guiAppInst, &QApplication::primaryScreenChanged, this, &WinWindow::configureWindowGeometry);
    connect(sdk(), &ScreenPlaySDK::sdkDisconnected, this, &WinWindow::destroyThis);
    connect(sdk(), &ScreenPlaySDK::incommingMessage, this, &WinWindow::messageReceived);
    connect(sdk(), &ScreenPlaySDK::replaceWallpaper, this, &WinWindow::replaceWallpaper);
    connect(&m_checkForFullScreenWindowTimer, &QTimer::timeout, this, &WinWindow::checkForFullScreenWindow);

    const auto screens = QApplication::screens();
    for (const auto& screen : screens) {
        connect(screen, &QScreen::geometryChanged, this, &WinWindow::configureWindowGeometry);
    }

    m_windowsDesktopProperties = std::make_unique<WindowsDesktopProperties>();
    m_windowHandle = reinterpret_cast<HWND>(m_window.winId());
    if (!IsWindow(m_windowHandle)) {
        qFatal("Could not get a valid window handle!");
    }
    qRegisterMetaType<WindowsDesktopProperties*>();
    qRegisterMetaType<WinWindow*>();

    bool ok = false;
    float volumeParsed = volume.toFloat(&ok);
    if (!ok) {
        qFatal("Could not parse volume");
    }

    setVolume(volumeParsed);
    setFillMode(fillmode);

    qmlRegisterSingletonInstance<WinWindow>("ScreenPlayWallpaper", 1, 0, "Wallpaper", this);

    configureWindowGeometry();
    bool hasWindowScaling = false;
    for (int i = 0; i < screens.count(); i++) {
        if (getScaling(i) != 1) {
            hasWindowScaling = true;
            break;
        }
    }
    if (hasWindowScaling) {
        qInfo() << "Monitor with scaling detected!";
        configureWindowGeometry();
    }

    // We do not support autopause for multi monitor wallpaper
    if (this->activeScreensList().length() == 1) {
        if (checkWallpaperVisible) {
            m_checkForFullScreenWindowTimer.start(10);
        }
    }

    QTimer::singleShot(1000, this, [&]() {
        setupWindowMouseHook();
    });

    if (!debugMode)
        sdk()->start();
}

void WinWindow::setVisible(bool show)
{
    if (show) {
        if (!ShowWindow(m_windowHandle, SW_SHOW))
            qDebug() << "Cannot set window handle SW_SHOW";

    } else {
        if (!ShowWindow(m_windowHandle, SW_HIDE))
            qDebug() << "Cannot set window handle SW_HIDE";
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

void WinWindow::setupWallpaperForOneScreen(int activeScreen)
{
    const QRect screenRect = QApplication::screens().at(activeScreen)->geometry();

    const float scaling = 1; //getScaling(activeScreen);
    for (int i = 0; i < QApplication::screens().count(); i++) {
        //   qInfo() << i << "scaling " << getScaling(i) << QApplication::screens().at(i)->geometry();
    }

    // qInfo() << activeScreen << "Monitor 0" << getScaling(0) << QApplication::screens().at(0)->geometry();
    // qInfo() << activeScreen << "Monitor 1" << getScaling(1) << QApplication::screens().at(1)->geometry();

    WinMonitorStats Monitors;
    int width = std::abs(Monitors.rcMonitors[activeScreen].right - Monitors.rcMonitors[activeScreen].left);
    int height = std::abs(Monitors.rcMonitors[activeScreen].top - Monitors.rcMonitors[activeScreen].bottom);
    //qInfo() << "scaling " << scaling << width << height << " activeScreen " << activeScreen;
    //qInfo() << "scaling " << scaling << screenRect.width() << screenRect.height() << " activeScreen " << activeScreen;

    const int boderWidth = 2;
    const int borderOffset = -1;
    // Needs to be set like to this work. I do not know why...
    if (!SetWindowPos(
            m_windowHandle,
            nullptr,
            Monitors.rcMonitors[activeScreen].left + borderOffset,
            Monitors.rcMonitors[activeScreen].top + borderOffset,
            width + boderWidth,
            height + boderWidth,
            SWP_HIDEWINDOW)) {
        qFatal("Could not set window pos");
    }

    if (!SetWindowPos(
            m_windowHandle,
            nullptr,
            screenRect.x() + m_zeroPoint.x() + borderOffset,
            screenRect.y() + m_zeroPoint.y() + borderOffset,
            screenRect.width() * scaling + boderWidth,
            screenRect.height() * scaling + boderWidth,
            SWP_HIDEWINDOW)) {
        qFatal("Could not set window pos");
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

    if (!SetWindowPos(m_windowHandle, nullptr, rect.x() + m_zeroPoint.x(), rect.y() + m_zeroPoint.y(), rect.width(), rect.height(), SWP_SHOWWINDOW)) {
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

float WinWindow::getScaling(const int monitorIndex)
{
    // screen->logicalDotsPerInch()
    // 100% - 96
    // 125% - 120
    // 150% - 144
    // 175% - 168
    // 200% - 192
    QScreen* screen = QApplication::screens().at(monitorIndex);
    const int factor = screen->logicalDotsPerInch();
    switch (factor) {
    case 96:
        return 1;
    case 120:
        return 1.25;
    case 144:
        return 1.5;
    case 168:
        return 1.75;
    case 192:
        return 2;
    case 216:
        return 2.25;
    case 240:
        return 2.5;
    case 288:
        return 3;
    case 336:
        return 3.5;
    default:
        qWarning() << "Monitor with factor: " << factor << " detected! This is not supported!";
        return 1;
    }
}

void WinWindow::configureWindowGeometry()
{
    ShowWindow(m_windowHandle, SW_HIDE);

    if (!searchWorkerWindowToParentTo()) {
        qFatal("No worker window found");
    }

    RECT rect {};
    if (!GetWindowRect(m_windowHandleWorker, &rect)) {
        qFatal("Unable to get WindoeRect from worker");
    }

    // Windows coordante system begins at 0x0 at the
    // main monitors upper left and not at the most left top monitor.
    // This can be easily read from the worker window.
    m_zeroPoint = { std::abs(rect.left), std::abs(rect.top) };
    g_globalOffset = m_zeroPoint;

    // WARNING: Setting Window flags must be called *here*!
    SetWindowLongPtr(m_windowHandle, GWL_EXSTYLE, WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT);
    SetWindowLongPtr(m_windowHandle, GWL_STYLE, WS_POPUPWINDOW);

    // Ether for one Screen or for all
    if ((QApplication::screens().length() == activeScreensList().length()) && (activeScreensList().length() != 1)) {
        setupWallpaperForAllScreens();
    } else if (activeScreensList().length() == 1) {
        setupWallpaperForOneScreen(activeScreensList().at(0));
        setCanFade(true);
    } else if (activeScreensList().length() > 1) {
        setupWallpaperForMultipleScreens(activeScreensList());
    }

    setWidth(m_window.width());
    setHeight(m_window.height());

    // Instead of setting "renderType: Text.NativeRendering" every time
    // we can set it here once :)
    m_window.setTextRenderType(QQuickWindow::TextRenderType::NativeTextRendering);
    m_window.setResizeMode(QQuickView::ResizeMode::SizeRootObjectToView);
    m_window.setSource(QUrl("qrc:/Wallpaper.qml"));
    m_window.hide();
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
