#include "mainwindow.h"

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

MainWindow::MainWindow(QScreen* parent)
    : QWindow(parent)
{
    //    for (int var = 0; var < QApplication::screens().count(); ++var) {
    //        QScreen* screen = QApplication::screens().at(i);
    //    }

    setOpacity(0);

    this->m_hwnd = (HWND)this->winId();

    QScreen* screen = QApplication::screens().at(0);

    setScreen(screen);

    HWND progman_hwnd = FindWindowW(L"Progman", L"Program Manager");

    // Spawn new worker window below desktop (using some undocumented Win32 magic)
    // See
    // https://www.codeproject.com/articles/856020/draw-behind-desktop-icons-in-windows
    // for more details
    const DWORD WM_SPAWN_WORKER = 0x052C;
    SendMessageTimeoutW(progman_hwnd, WM_SPAWN_WORKER, 0xD, 0x1, SMTO_NORMAL,
        1000, nullptr);

    EnumWindows(SearchForWorkerWindow, reinterpret_cast<LPARAM>(&m_worker_hwnd));
    ShowWindow(m_worker_hwnd, SW_HIDE);
    ShowWindow(m_hwnd, SW_HIDE);
    SetParent(m_hwnd, m_worker_hwnd);

    SetWindowPos(m_worker_hwnd, HWND_BOTTOM, screen->geometry().x(), screen->geometry().y(), screen->geometry().width(), screen->geometry().height(), SWP_SHOWWINDOW);
    SetWindowPos(m_hwnd, HWND_BOTTOM, screen->geometry().x(), screen->geometry().y(), screen->size().width(), screen->size().height(), SWP_SHOWWINDOW);

    SetWindowLongPtr(m_hwnd, GWL_STYLE,
        WS_CAPTION | WS_THICKFRAME | WS_MINIMIZE | WS_MAXIMIZE | WS_SYSMENU);
    SetWindowLongPtr(m_hwnd, GWL_EXSTYLE,
        WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOACTIVATE | WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW);

    Qt::WindowFlags flags = this->flags();
    this->setFlags(flags | Qt::FramelessWindowHint | Qt::WindowStaysOnBottomHint);

    m_quickRenderer = QSharedPointer<QQuickView>(new QQuickView(this));
    //m_quickRenderer.data()->engine()->addImportPath("C:/msys64/mingw64/share/qt5/qml");
    //m_quickRenderer.data()->engine()->addImportPath("C:/msys64/mingw64/share/qt5");

    m_quickRenderer.data()->rootContext()->setContextProperty("mainwindow", this);

    m_quickRenderer.data()->setGeometry(screen->geometry());
    m_quickRenderer.data()->setResizeMode(QQuickView::ResizeMode::SizeRootObjectToView);
    m_quickRenderer.data()->setSource(QUrl("qrc:/main.qml"));

    QPropertyAnimation* animation = new QPropertyAnimation(this, "opacity");
    animation->setDuration(250);
    animation->setEasingCurve(QEasingCurve::OutCubic);
    animation->setStartValue(0);
    animation->setEndValue(1);

    m_quickRenderer.data()->show();
    show();

    ShowWindow(m_worker_hwnd, SW_SHOWDEFAULT);
    ShowWindow(m_hwnd, SW_SHOWDEFAULT);

    animation->start();
}

void MainWindow::destroyThis()
{
    ShowWindow(m_worker_hwnd, SW_HIDE);
    ShowWindow(m_hwnd, SW_HIDE);
    QCoreApplication::quit();
}

MainWindow::~MainWindow()
{
}
