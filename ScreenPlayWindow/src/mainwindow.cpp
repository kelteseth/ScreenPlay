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

MainWindow::MainWindow(int i, QString projectPath, QScreen* parent)
    : QWindow(parent)
{

    setOpacity(0);
    m_projectPath = projectPath;

    QFile configTmp;
    QJsonDocument configJsonDocument;
    QJsonParseError parseError;

    configTmp.setFileName(projectPath + "/project.json");
    configTmp.open(QIODevice::ReadOnly | QIODevice::Text);
    QString config = configTmp.readAll();
    configJsonDocument = QJsonDocument::fromJson(config.toUtf8(), &parseError);

    if (!(parseError.error == QJsonParseError::NoError)) {
        qWarning("Settings Json Parse Error ");
        QApplication::exit(-4);
    }

    m_project = configJsonDocument.object();

    //Some settings dont have a file type
    if (!m_project.contains("type")) {
        if (m_project.contains("file")) {
            QString fileEnding = m_project.value("file").toString();
            if (fileEnding.endsWith(".mp4") || fileEnding.endsWith(".vp9") || fileEnding.endsWith(".webm")) {
                m_project.insert("type", "video");
            }
        }
    }

    if (m_project.contains("file"))
        m_projectFile = m_project.value("file").toString();

    // Recalculate window coordiantes because of point (0,0)
    // Is at the origin monitor or the most left
    QScreen* screen = QApplication::screens().at(i);
    int offsetX = 0;
    int offsetY = 0;

    for (int i = 0; i < QApplication::screens().count(); i++) {
        QScreen* screen = QApplication::screens().at(i);
        if (screen->availableGeometry().x() < 0) {
            offsetX += (screen->availableGeometry().x() * -1);
        }
        if (screen->availableGeometry().y() < 0) {
            offsetY += (screen->availableGeometry().y() * -1);
        }
    }

    m_hwnd = (HWND)this->winId();
    HWND progman_hwnd = FindWindowW(L"Progman", L"Program Manager");

    // Spawn new worker window below desktop (using some undocumented Win32 magic)
    // See
    // https://www.codeproject.com/articles/856020/draw-behind-desktop-icons-in-windows
    // for more details
    const DWORD WM_SPAWN_WORKER = 0x052C;
    SendMessageTimeoutW(progman_hwnd, WM_SPAWN_WORKER, 0xD, 0x1, SMTO_NORMAL,
        1000, nullptr);

    bool foundWorker = EnumWindows(SearchForWorkerWindow, reinterpret_cast<LPARAM>(&m_worker_hwnd));

    if (!foundWorker) {
        qDebug() << "No worker window found";
    }

    //Hide first to avoid flickering

    ShowWindow(m_worker_hwnd, SW_HIDE);
    ShowWindow(m_hwnd, SW_HIDE);
    MoveWindow(m_hwnd, screen->geometry().x() + offsetX, screen->geometry().y() + offsetY, screen->size().width(), screen->size().height(), true);
    SetParent(m_hwnd, m_worker_hwnd);

    SetWindowLongPtr(m_hwnd, GWL_STYLE, WS_CHILDWINDOW | WS_CAPTION | WS_THICKFRAME | WS_MINIMIZE | WS_MAXIMIZE | WS_SYSMENU | WS_POPUP);
    SetWindowLongPtr(m_hwnd, GWL_EXSTYLE, WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOACTIVATE | WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW);

    Qt::WindowFlags flags = this->flags();
    this->setFlags(flags | Qt::FramelessWindowHint | Qt::WindowStaysOnBottomHint);

    m_quickRenderer = QSharedPointer<QQuickView>(new QQuickView(this));
    m_quickRenderer.data()->rootContext()->setContextProperty("mainwindow", this);
    m_quickRenderer.data()->setSource(QUrl("qrc:/main.qml"));
    m_quickRenderer.data()->setGeometry(0, 0, width(), height());
    m_quickRenderer.data()->setResizeMode(QQuickView::ResizeMode::SizeRootObjectToView);
    m_quickRenderer.data()->setFlags(flags | Qt::FramelessWindowHint | Qt::WindowStaysOnBottomHint);

    if (m_project.contains("type")) {
        if (m_project.value("type") == "video") {
            QString tmpPath = m_projectPath.toString() + "/" + m_projectFile;
            emit playVideo(tmpPath);
        } else if (m_project.value("type") == "scene") {
            return;
        }
    }
}
void MainWindow::init()
{
    ShowWindow(m_worker_hwnd, SW_SHOWDEFAULT);
    ShowWindow(m_hwnd, SW_SHOWDEFAULT);
    m_quickRenderer.data()->show();
    this->setVisible(true);
    setOpacity(0);
    QPropertyAnimation* animation = new QPropertyAnimation(this, "opacity");
    animation->setDuration(250);
    animation->setEasingCurve(QEasingCurve::OutCubic);
    animation->setStartValue(0);
    animation->setEndValue(1);
    animation->start();
}
void MainWindow::destroyThis()
{
    QPropertyAnimation* animation = new QPropertyAnimation(this, "opacity");
    animation->setDuration(500);
    animation->setStartValue(1);
    animation->setEndValue(0);
    animation->start();

    QObject::connect(animation, &QPropertyAnimation::finished, [&]() {
        ShowWindow(m_worker_hwnd, SW_HIDE);
        ShowWindow(m_hwnd, SW_HIDE);
        QCoreApplication::quit();
    });
}

QUrl MainWindow::projectPath() const
{
    return m_projectPath;
}

void MainWindow::setProjectPath(const QUrl& projectPath)
{
    m_projectPath = projectPath;
}

MainWindow::~MainWindow()
{
}