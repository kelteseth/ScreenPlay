#include "wallpaper.h"

#include <QPropertyAnimation>

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

Wallpaper::Wallpaper(ProjectFile project, Monitor monitor)
{
    m_monitor = monitor;
    m_project = project;
    QString tmp = m_project.m_absoluteStoragePath.toString() + "/" + m_project.m_file.toString();
    tmp.replace("/", "\\\\");
    setAbsoluteFilePath(tmp);

    this->setX(m_monitor.m_availableGeometry.x());
    this->setY(m_monitor.m_availableGeometry.y());
    this->setWidth(m_monitor.m_availableGeometry.width());
    this->setHeight(m_monitor.m_availableGeometry.height());
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

    SetParent(m_hwnd, m_worker_hwnd);
    SetLayeredWindowAttributes(m_hwnd,RGB(255,0,0),0,LWA_ALPHA);
    SetWindowLongPtr(m_hwnd, GWL_STYLE,
        WS_CAPTION | WS_THICKFRAME | WS_MINIMIZE | WS_MAXIMIZE | WS_SYSMENU);
    SetWindowLongPtr(m_hwnd, GWL_EXSTYLE,
        WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOACTIVATE | WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW);

    Qt::WindowFlags flags = this->flags();

    this->setFlags(flags | Qt::FramelessWindowHint | Qt::WindowStaysOnBottomHint);
//    this->setOpacity(0.0f);
    this->show();


    m_quickRenderer = QSharedPointer<QQuickView>(new QQuickView(this));

//    QSurfaceFormat surfaceFormat;
//    surfaceFormat.setAlphaBufferSize(8);
//    surfaceFormat.setRenderableType(QSurfaceFormat::OpenGL);
//    m_quickRenderer.data()->setFormat(surfaceFormat);

    //m_quickRenderer.data()->setColor(QColor(Qt::transparent));
    m_quickRenderer.data()->setWidth(this->width());
    m_quickRenderer.data()->setHeight(this->height());
    //m_quickRenderer.data()->setClearBeforeRendering(true);

    m_context = m_quickRenderer.data()->rootContext();
    m_context->setContextProperty("wallpaper", this);
    m_quickRenderer.data()->setResizeMode(QQuickView::ResizeMode::SizeRootObjectToView);
    m_quickRenderer.data()->setSource(QUrl("qrc:/qml/Screens/ScreenVideo.qml"));
    m_quickRenderer.data()->showFullScreen();
    setVisible(true);
}

Wallpaper::~Wallpaper()
{
    ShowWindow(m_hwnd, SW_HIDE);
    ShowWindow(m_worker_hwnd, SW_HIDE);
    CloseWindow(m_hwnd);
    DestroyWindow(m_worker_hwnd);
    DestroyWindow(m_hwnd);
}

void Wallpaper::setupWindow()
{
        QPropertyAnimation* anim = new QPropertyAnimation(this, "opacity");
        anim->setDuration(2000);
        anim->setStartValue(0);
        anim->setEndValue(1);
        anim->start(QAbstractAnimation::DeleteWhenStopped);
}

Monitor Wallpaper::monitor() const
{
    return m_monitor;
}

void Wallpaper::setVisible(bool visible)
{
    if (visible){
        ShowWindow(m_worker_hwnd, SW_SHOWDEFAULT);
        ShowWindow(m_hwnd, SW_SHOWDEFAULT);
    }
    else{
        ShowWindow(m_worker_hwnd, SW_HIDE);
        ShowWindow(m_hwnd, SW_HIDE);
    }
}

void Wallpaper::setMonitor(const Monitor& monitor)
{
    m_monitor = monitor;
}
