#include "widgetwindow.h"

#include <QCoreApplication>

#include "ScreenPlayUtil/contenttypes.h"
#include "ScreenPlayUtil/util.h"

/*!
    \module ScreenPlayWidget
    \title ScreenPlayWidget
    \brief Module for ScreenPlayWidget.
*/

/*!
    \class WidgetWindow
    \inmodule ScreenPlayWidget
    \brief  .
*/

WidgetWindow::WidgetWindow(
    const QString& projectPath,
    const QString& appID,
    const QString& type,
    const QPoint& position)
    : QObject(nullptr)
    , m_appID { appID }
    , m_type { type }
    , m_position { position }
{

    m_sdk = std::make_unique<ScreenPlaySDK>(appID, type);

    QObject::connect(m_sdk.get(), &ScreenPlaySDK::sdkDisconnected, this, &WidgetWindow::qmlExit);
    QObject::connect(m_sdk.get(), &ScreenPlaySDK::incommingMessage, this, &WidgetWindow::messageReceived);

    if (!ScreenPlayUtil::getAvailableWidgets().contains(m_type, Qt::CaseSensitivity::CaseInsensitive)) {
        QApplication::exit(-4);
    }

    setType(type);

    Qt::WindowFlags flags = m_window.flags();

    m_window.setFlags(flags | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint | Qt::BypassWindowManagerHint | Qt::SplashScreen);
    m_window.setColor(Qt::transparent);

    qmlRegisterSingletonInstance<WidgetWindow>("ScreenPlayWidget", 1, 0, "Widget", this);

#ifdef Q_OS_WIN
    m_hwnd = reinterpret_cast<HWND>(m_window.winId());
    setWindowBlur();
#endif

    if (projectPath == "test") {
        setSourcePath("qrc:/test.qml");
    } else {
        auto projectOpt = ScreenPlayUtil::openJsonFileToObject(projectPath + "/project.json");
        if (projectOpt.has_value()) {
            qWarning() << "Unable to parse project file!";
            QApplication::exit(-1);
        }

        m_project = projectOpt.value();
        QString fullPath = "file:///" + projectPath + "/" + m_project.value("file").toString();
        setSourcePath(fullPath);
    }

    m_window.setTextRenderType(QQuickWindow::TextRenderType::NativeTextRendering);
    m_window.setResizeMode(QQuickView::ResizeMode::SizeViewToRootObject);
    m_window.setSource(QUrl("qrc:/Widget.qml"));
    m_window.setPosition(m_position);
    m_window.show();

    // Do not trigger position changed save reuqest on startup
    QTimer::singleShot(1000, this, [=, this]() {
        // We limit ourself to only update the position every 500ms!
        auto sendPositionUpdate = [this]() {
            m_positionMessageLimiter.stop();
            if (!m_sdk->isConnected())
                return;

            QJsonObject obj;
            obj.insert("messageType", "positionUpdate");
            obj.insert("positionX", m_window.x());
            obj.insert("positionY", m_window.y());
            m_sdk->sendMessage(obj);
        };
        m_positionMessageLimiter.setInterval(500);

        QObject::connect(&m_positionMessageLimiter, &QTimer::timeout, this, sendPositionUpdate);
        QObject::connect(&m_window, &QWindow::xChanged, this, [this]() { m_positionMessageLimiter.start(); });
        QObject::connect(&m_window, &QWindow::yChanged, this, [this]() { m_positionMessageLimiter.start(); });
    });
}

void WidgetWindow::setSize(QSize size)
{
    m_window.setWidth(size.width());
    m_window.setHeight(size.height());
}

void WidgetWindow::destroyThis()
{
    QCoreApplication::quit();
}

void WidgetWindow::messageReceived(QString key, QString value)
{
    emit qmlSceneValueReceived(key, value);
}

void WidgetWindow::setPos(int xPos, int yPos)
{
    m_window.setPosition({ xPos, yPos });
}

void WidgetWindow::setClickPos(const QPoint& clickPos)
{
    m_clickPos = clickPos;
}

void WidgetWindow::setWidgetSize(const int with, const int height)
{
    m_window.setWidth(with);
    m_window.setHeight(height);
}

void WidgetWindow::clearComponentCache()
{
    m_window.engine()->clearComponentCache();
}

#ifdef Q_OS_WIN
void WidgetWindow::setWindowBlur(unsigned int style)
{

    const HINSTANCE hModule = LoadLibrary(TEXT("user32.dll"));
    if (hModule) {
        struct ACCENTPOLICY {
            int nAccentState;
            int nFlags;
            int nColor;
            int nAnimationId;
        };
        struct WINCOMPATTRDATA {
            int nAttribute;
            PVOID pData;
            ULONG ulDataSize;
        };
        enum class Accent {
            DISABLED = 0,
            GRADIENT = 1,
            TRANSPARENTGRADIENT = 2,
            BLURBEHIND = 3,
            ACRYLIC = 4,
            INVALID = 5
        };
        typedef BOOL(WINAPI * pSetWindowCompositionAttribute)(HWND, WINCOMPATTRDATA*);
        const pSetWindowCompositionAttribute SetWindowCompositionAttribute = (pSetWindowCompositionAttribute)GetProcAddress(hModule, "SetWindowCompositionAttribute");
        if (SetWindowCompositionAttribute) {
            ACCENTPOLICY policy = { static_cast<int>(style), 0, 0, 0 }; // ACCENT_ENABLE_BLURBEHIND=3...
            WINCOMPATTRDATA data = { 19, &policy, sizeof(ACCENTPOLICY) }; // WCA_ACCENT_POLICY=19
            SetWindowCompositionAttribute(m_hwnd, &data);
        }
        FreeLibrary(hModule);
    }
}
#endif
