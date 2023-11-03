// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#include "winwindow.h"
#include "windowsintegration.h"
#include <QGuiApplication>
#include <QtQml>
#include <shellscalingapi.h>
#include <vector>
#include <windows.h>

/*!
    \class WinWindow
    \inmodule ScreenPlayWallpaper
    \brief  ScreenPlayWindow used for the Windows implementation.
*/

ScreenPlay::WallpaperExitCode WinWindow::start()
{
    connect(
        &m_window, &QQuickView::statusChanged, this, [this](auto status) {
            if (status == QQuickView::Status::Error) {
                destroyThis();
            }
        },
        Qt::QueuedConnection);
    auto* guiAppInst = dynamic_cast<QGuiApplication*>(QGuiApplication::instance());
    if (!debugMode()) {
        connect(m_sdk.get(), &ScreenPlaySDK::sdkDisconnected, this, &WinWindow::destroyThis);
    }
    connect(guiAppInst, &QGuiApplication::screenAdded, this, &WinWindow::configureWindowGeometry);
    connect(guiAppInst, &QGuiApplication::screenRemoved, this, &WinWindow::configureWindowGeometry);
    connect(guiAppInst, &QGuiApplication::primaryScreenChanged, this, &WinWindow::configureWindowGeometry);
    connect(this, &BaseWindow::reloadQML, this, &WinWindow::clearComponentCache);
    connect(&m_checkForFullScreenWindowTimer, &QTimer::timeout, this, &WinWindow::checkForFullScreenWindow);

    const auto screens = QGuiApplication::screens();
    for (const auto& screen : screens) {
        connect(screen, &QScreen::geometryChanged, this, &WinWindow::configureWindowGeometry);
    }

    m_windowsDesktopProperties = std::make_unique<WindowsDesktopProperties>();
    m_windowsIntegration.setWindowHandle(reinterpret_cast<HWND>(m_window.winId()));
    if (!IsWindow(m_windowsIntegration.windowHandle())) {
        qCritical("Could not get a valid window handle!");
        return ScreenPlay::WallpaperExitCode::Invalid_Start_Windows_HandleError;
    }
    qRegisterMetaType<WindowsDesktopProperties*>();
    qRegisterMetaType<WinWindow*>();
    qmlRegisterSingletonInstance<WinWindow>("ScreenPlayWallpaper", 1, 0, "Wallpaper", this);

    m_window.setResizeMode(QQuickView::ResizeMode::SizeRootObjectToView);
    configureWindowGeometry();

    // We do not support autopause for multi monitor wallpaper and
    // wallpaper than contain audio, see BaseWindow::setup().
    if (activeScreensList().length() == 1) {
        if (checkWallpaperVisible()) {
            m_checkForFullScreenWindowTimer.start(10);
        }
    }

    qInfo() << "Setup " << width() << height();
    m_window.setSource(QUrl("qrc:/qml/ScreenPlayWallpaper/qml/Wallpaper.qml"));
    m_window.show();

    QTimer::singleShot(1000, this, [&]() {
        m_windowsIntegration.setupWindowMouseHook();
        // Set up the mouse event handler
        m_windowsIntegration.setMouseEventHandler([this](DWORD mouseButton, UINT type, POINT p) {
            Qt::MouseButton button = Qt::NoButton;
            QEvent::Type eventType = QEvent::None;

            // Determine the Qt mouse event type and button
            switch (type) {
            case WM_LBUTTONDOWN:
                eventType = QEvent::MouseButtonPress;
                button = Qt::LeftButton;
                break;
            case WM_LBUTTONUP:
                eventType = QEvent::MouseButtonRelease;
                button = Qt::LeftButton;
                break;
            case WM_RBUTTONDOWN:
                eventType = QEvent::MouseButtonPress;
                button = Qt::RightButton;
                break;
            case WM_RBUTTONUP:
                eventType = QEvent::MouseButtonRelease;
                button = Qt::RightButton;
                break;
            case WM_MOUSEMOVE:
                eventType = QEvent::MouseMove;
                button = Qt::NoButton;
                break;
                // Add more cases as needed
            }

            // Convert POINT to global position
            QPoint globalPos(p.x, p.y);

            // Convert global position to local position within the target widget
            QPoint localPos = m_window.mapFromGlobal(globalPos);

            // Create the QMouseEvent
            QMouseEvent* qEvent = new QMouseEvent(eventType, localPos, globalPos, button, button, QGuiApplication::keyboardModifiers());

            // Handle the mouse event
            // For example, logging the mouse button and position
            qDebug() << "Mouse event: Button=" << mouseButton << ", Type=" << type
                     << ", Position=(" << p.x << ", " << p.y << ")" << globalPos << localPos << button;
            // Post the event to the target widget
            QCoreApplication::postEvent(&m_window, qEvent);
        });

        // Inside your main application or somewhere appropriate
        m_windowsIntegration.setupWindowKeyboardHook();
        // Set up the keyboard event handler
        m_windowsIntegration.setKeyboardEventHandler([this](UINT vkCode, bool keyDown) {
            QEvent::Type eventType = keyDown ? QEvent::KeyPress : QEvent::KeyRelease;

            // Map the vkCode to Qt key code if necessary
            auto [qtKey, text] = mapVirtualKeyToQtKey(vkCode); // Implement this function based on your needs

            // Create the QKeyEvent
            QKeyEvent* qEvent = new QKeyEvent(eventType, qtKey, Qt::NoModifier, text);

            // Handle the keyboard event
            // For example, logging the key press
            qDebug() << "Keyboard event: Key=" << vkCode << ", Type=" << (keyDown ? "KeyDown" : "KeyUp") << qEvent;

            // Post the event to the target widget
            QCoreApplication::postEvent(&m_window, qEvent);
        });
    });
    return ScreenPlay::WallpaperExitCode::Ok;
}

/*!
  \brief Calls ShowWindow function to set the main window in/visible.
*/
void WinWindow::setVisible(bool show)
{
    if (show) {
        if (!ShowWindow(m_windowsIntegration.windowHandle(), SW_SHOW)) {
            qDebug() << "Cannot set window handle SW_SHOW";
        }

    } else {
        if (!ShowWindow(m_windowsIntegration.windowHandle(), SW_HIDE)) {
            qDebug() << "Cannot set window handle SW_HIDE";
        }
    }
}

/*!
  \brief This function fires a qmlExit() signal to the UI for it to handle
         nice fade out animation first. Then the UI is responsible for calling
         WinWindow::terminate().
*/
void WinWindow::destroyThis()
{
    emit qmlExit();
}

void WinWindow::setupWallpaperForOneScreen(int activeScreen)
{
    auto updateWindowSize = [this](const int width, const int height) {
        setWidth(width);
        setHeight(height);
        m_window.setWidth(width);
        m_window.setHeight(height);
    };
    WindowsIntegration::MonitorResult monitor = m_windowsIntegration.setupWallpaperForOneScreen(activeScreen, updateWindowSize);
}

/*!
  \brief This method is called if the user want to have one wallpaper on all windows.
*/
void WinWindow::setupWallpaperForAllScreens()
{
    WindowsIntegration::SpanResult span = m_windowsIntegration.setupWallpaperForAllScreens();
    setWidth(span.width);
    setHeight(span.height);
    m_window.setWidth(width());
    m_window.setHeight(height());
}

/*!
  \brief This method is called if the user want to have one wallpaper on multiple windows.
*/
void WinWindow::setupWallpaperForMultipleScreens(const QVector<int>& activeScreensList)
{
    std::vector<int> activeScreens(activeScreensList.begin(), activeScreensList.end());
    WindowsIntegration::SpanResult span = m_windowsIntegration.setupWallpaperForMultipleScreens(activeScreens);
    setWidth(span.width);
    setHeight(span.height);
    m_window.setWidth(width());
    m_window.setHeight(height());
}

/*!
   \brief Sets the size and the parent to the worker handle to be displayed behind the
          desktop icons.
*/
void WinWindow::configureWindowGeometry()
{
    if (!m_windowsIntegration.searchWorkerWindowToParentTo()) {
        qFatal("No worker window found");
    }
    if (!IsWindow(m_windowsIntegration.windowHandleWorker())) {
        qCritical("Could not get a valid window handle wroker!");
        return;
    }

    // WARNING: Setting Window flags must be called *here*!
    SetWindowLongPtr(m_windowsIntegration.windowHandle(), GWL_EXSTYLE, WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT);
    SetWindowLongPtr(m_windowsIntegration.windowHandle(), GWL_STYLE, WS_POPUP);

    // Ether for one Screen or for all
    if ((QGuiApplication::screens().length() == activeScreensList().length()) && (activeScreensList().length() != 1)) {
        setupWallpaperForAllScreens();
    } else if (activeScreensList().length() == 1) {
        setupWallpaperForOneScreen(activeScreensList().at(0));
        setCanFade(true);
    } else if (activeScreensList().length() > 1) {
        setupWallpaperForMultipleScreens(activeScreensList());
    }
    m_window.show();
}

std::tuple<int, QString> WinWindow::mapVirtualKeyToQtKey(UINT vkCode)
{
    switch (vkCode) {
    case VK_BACK:
        return { Qt::Key_Backspace, QString() };
    case VK_TAB:
        return { Qt::Key_Tab, QString("\t") };
    case VK_CLEAR:
        return { Qt::Key_Clear, QString() };
    case VK_RETURN:
        return { Qt::Key_Return, QString("\r") };
    // ... rest of the cases ...
    case VK_SPACE:
        return { Qt::Key_Space, QString(" ") };
    // Extra keys
    case 0x21: // '!'
        return { Qt::Key_Exclam, QString("!") };
    case 0x40: // '@'
        return { Qt::Key_At, QString("@") };
    case 0x23: // '#'
        return { Qt::Key_NumberSign, QString("#") };
    case 0x24: // '$'
        return { Qt::Key_Dollar, QString("$") };
    case 0x25: // '%'
        return { Qt::Key_Percent, QString("%") };
    case 0x5E: // '^'
        return { Qt::Key_AsciiCircum, QString("^") };
    case 0x26: // '&'
        return { Qt::Key_Ampersand, QString("&") };
    case 0x2A: // '*'
        return { Qt::Key_Asterisk, QString("*") };
    case 0x28: // '('
        return { Qt::Key_ParenLeft, QString("(") };
    case 0x29: // ')'
        return { Qt::Key_ParenRight, QString(")") };
    case 0x5F: // '_'
        return { Qt::Key_Underscore, QString("_") };
    case 0x2B: // '+'
        return { Qt::Key_Plus, QString("+") };
    case 0x2D: // '-'
        return { Qt::Key_Minus, QString("-") };
    case 0x3D: // '='
        return { Qt::Key_Equal, QString("=") };
    case 0x5B: // '['
        return { Qt::Key_BracketLeft, QString("[") };
    case 0x5D: // ']'
        return { Qt::Key_BracketRight, QString("]") };
    case 0x7B: // '{'
        return { Qt::Key_BraceLeft, QString("{") };
    case 0x7D: // '}'
        return { Qt::Key_BraceRight, QString("}") };
    case 0x3B: // ';'
        return { Qt::Key_Semicolon, QString(";") };
    case 0x27: // '''
        return { Qt::Key_Apostrophe, QString("'") };
    case 0x3A: // ':'
        return { Qt::Key_Colon, QString(":") };
    case 0x22: // '"'
        return { Qt::Key_QuoteDbl, QString("\"") };
    case 0x2F: // '/'
        return { Qt::Key_Slash, QString("/") };
    case 0x2E: // '.'
        return { Qt::Key_Period, QString(".") };
    case 0x3E: // '>'
        return { Qt::Key_Greater, QString(">") };
    case 0x2C: // ','
        return { Qt::Key_Comma, QString(",") };
    case 0x3F: // '?'
        return { Qt::Key_Question, QString("?") };
    // Numeric keys
    case 0x30:
        return { Qt::Key_0, QString("0") };
    case 0x31:
        return { Qt::Key_1, QString("1") };
    case 0x32:
        return { Qt::Key_2, QString("2") };
    case 0x33:
        return { Qt::Key_3, QString("3") };
    case 0x34:
        return { Qt::Key_4, QString("4") };
    case 0x35:
        return { Qt::Key_5, QString("5") };
    case 0x36:
        return { Qt::Key_6, QString("6") };
    case 0x37:
        return { Qt::Key_7, QString("7") };
    case 0x38:
        return { Qt::Key_8, QString("8") };
    case 0x39:
        return { Qt::Key_9, QString("9") };

        // Alphabet keys
    case 0x41:
        return { Qt::Key_A, QString("a") };
    case 0x42:
        return { Qt::Key_B, QString("b") };
    case 0x43:
        return { Qt::Key_C, QString("c") };
    case 0x44:
        return { Qt::Key_D, QString("d") };
    case 0x45:
        return { Qt::Key_E, QString("e") };
    case 0x46:
        return { Qt::Key_F, QString("f") };
    case 0x47:
        return { Qt::Key_G, QString("g") };
    case 0x48:
        return { Qt::Key_H, QString("h") };
    case 0x49:
        return { Qt::Key_I, QString("i") };
    case 0x4A:
        return { Qt::Key_J, QString("j") };
    case 0x4B:
        return { Qt::Key_K, QString("k") };
    case 0x4C:
        return { Qt::Key_L, QString("l") };
    case 0x4D:
        return { Qt::Key_M, QString("m") };
    case 0x4E:
        return { Qt::Key_N, QString("n") };
    case 0x4F:
        return { Qt::Key_O, QString("o") };
    case 0x50:
        return { Qt::Key_P, QString("p") };
    case 0x51:
        return { Qt::Key_Q, QString("q") };
    case 0x52:
        return { Qt::Key_R, QString("r") };
    case 0x53:
        return { Qt::Key_S, QString("s") };
    case 0x54:
        return { Qt::Key_T, QString("t") };
    case 0x55:
        return { Qt::Key_U, QString("u") };
    case 0x56:
        return { Qt::Key_V, QString("v") };
    case 0x57:
        return { Qt::Key_W, QString("w") };
    case 0x58:
        return { Qt::Key_X, QString("x") };
    case 0x59:
        return { Qt::Key_Y, QString("y") };
    case 0x5A:
        return { Qt::Key_Z, QString("z") };
    default:
        return { Qt::Key_unknown, QString() };
    }
}

/*!
  \brief Custom exit method to force a redraw of the window so that
         the default desktop wallpaper can be seen agian.
*/
void WinWindow::terminate()
{
    using ScreenPlay::InstalledType::InstalledType;

    ShowWindow(m_windowsIntegration.windowHandle(), SW_HIDE);

    // Force refresh so that we display the regular
    // desktop wallpaper again
    ShowWindow(m_windowsIntegration.windowHandleWorker(), SW_HIDE);
    ShowWindow(m_windowsIntegration.windowHandleWorker(), SW_SHOW);

    m_windowsIntegration.unhookKeyboard();
    m_windowsIntegration.unhookMouse();

    QGuiApplication::quit();
}

/*!
  \brief Call the qml engine clearComponentCache. This function is used for
         refreshing wallpaper when the content has changed. For example this
         is needed for live editing when the content is chached.
*/
void WinWindow::clearComponentCache()
{
    m_window.engine()->clearComponentCache();
}

void WinWindow::checkForFullScreenWindow()
{
    bool hasFullscreenWindow = m_windowsIntegration.checkForFullScreenWindow(m_windowsIntegration.windowHandle());

    setVisualsPaused(hasFullscreenWindow);
}

#include "moc_winwindow.cpp"
