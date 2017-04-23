#include "screenplay.h"

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

ScreenPlay::ScreenPlay(QWindow* parent)
    : QWindow(parent)
{
}

ScreenPlay::ScreenPlay(int width, int height)
{
    this->quickRenderer = new QQuickView(this);
    _context = quickRenderer->rootContext();

    this->setHeight(height);
    this->setWidth(width);

    this->hwnd = (HWND)this->winId();

    HWND progman_hwnd = FindWindowW(L"Progman", L"Program Manager");

    // Spawn new worker window below desktop (using some undocumented Win32 magic)
    // See
    // https://www.codeproject.com/articles/856020/draw-behind-desktop-icons-in-windows
    // for more details
    const DWORD WM_SPAWN_WORKER = 0x052C;
    SendMessageTimeoutW(progman_hwnd, WM_SPAWN_WORKER, 0xD, 0x1, SMTO_NORMAL,
        1000, nullptr);

    // Get window handle to the worker window and hide it

    EnumWindows(SearchForWorkerWindow, reinterpret_cast<LPARAM>(&worker_hwnd));
    ShowWindow(worker_hwnd, SW_SHOWDEFAULT);

    SetParent(hwnd, worker_hwnd);
    SetWindowLongPtr(hwnd, GWL_STYLE,
        WS_CAPTION | WS_THICKFRAME | WS_MINIMIZE | WS_MAXIMIZE | WS_SYSMENU);
    SetWindowLongPtr(hwnd, GWL_EXSTYLE,
        WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOACTIVATE | WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW);


    Qt::WindowFlags flags = this->flags();
    this->setFlags(flags | Qt::FramelessWindowHint | Qt::WindowStaysOnBottomHint);
    this->show();

    //ShowWindow(hwnd, SW_SHOWDEFAULT);

    // We do not want to display anything initially
    setVisible(false);
}

ScreenPlay::~ScreenPlay()
{
    ShowWindow(worker_hwnd, SW_HIDE);
}

void ScreenPlay::loadQQuickView(QUrl path)
{
    quickRenderer->setSource(path);
}

void ScreenPlay::showQQuickView(int width, int height)
{
    quickRenderer->setWidth(width);
    quickRenderer->setHeight(height);
    quickRenderer->show();
}


void ScreenPlay::setVisible(bool visible)
{
    if (visible)
        ShowWindow(hwnd, SW_SHOWDEFAULT);
    else
        ShowWindow(hwnd, SW_HIDE);
}

QQmlContext *ScreenPlay::context() const
{
    return _context;
}

