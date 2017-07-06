#include "wallpaper.h"

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

Wallpaper::Wallpaper(QWindow* parent)
    : QWindow(parent)
{
}

Wallpaper::Wallpaper(Profile profile)
{

    m_profile = profile;
    this->setX(m_profile.m_rect.x());
    this->setY(m_profile.m_rect.y());
    this->setWidth(m_profile.m_rect.width());
    this->setHeight(m_profile.m_rect.height());
    this->m_hwnd = (HWND)this->winId();


    HWND progman_hwnd = FindWindowW(L"Progman", L"Program Manager");

    // Spawn new worker window below desktop (using some undocumented Win32 magic)
    // See
    // https://www.codeproject.com/articles/856020/draw-behind-desktop-icons-in-windows
    // for more details
    const DWORD WM_SPAWN_WORKER = 0x052C;
    SendMessageTimeoutW(progman_hwnd, WM_SPAWN_WORKER, 0xD, 0x1, SMTO_NORMAL,
        1000, nullptr);

    EnumWindows(SearchForWorkerWindow, reinterpret_cast<LPARAM>(&m_worker_hwnd));
    ShowWindow(m_worker_hwnd, SW_SHOWDEFAULT);

    SetParent(m_hwnd, m_worker_hwnd);
    SetWindowLongPtr(m_hwnd, GWL_STYLE,
        WS_CAPTION | WS_THICKFRAME | WS_MINIMIZE | WS_MAXIMIZE | WS_SYSMENU);
    SetWindowLongPtr(m_hwnd, GWL_EXSTYLE,
        WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOACTIVATE | WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW);

    Qt::WindowFlags flags = this->flags();
    this->setFlags(flags | Qt::FramelessWindowHint | Qt::WindowStaysOnBottomHint);
    this->show();
    ShowWindow(m_hwnd, SW_SHOWDEFAULT);
    m_quickRenderer = new QQuickView(this);
    m_quickRenderer->setWidth(this->width());
    m_quickRenderer->setHeight(this->height());
    m_quickRenderer->setResizeMode(QQuickView::ResizeMode::SizeRootObjectToView);
    m_quickRenderer->setSource(QUrl("qrc:/qml/Components/Screens/ScreenVideo.qml"));
    m_context = m_quickRenderer->rootContext();
    m_quickRenderer->show();
}
}
