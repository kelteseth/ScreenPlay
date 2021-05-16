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
    const QPoint& position,
    const bool debugMode)
    : QObject(nullptr)
    , m_appID { appID }
    , m_position { position }
    , m_sdk { std::make_unique<ScreenPlaySDK>(appID, type) }
    , m_debugMode { debugMode }
{

    qRegisterMetaType<ScreenPlay::InstalledType::InstalledType>();
    qmlRegisterUncreatableMetaObject(ScreenPlay::InstalledType::staticMetaObject,
        "ScreenPlay.Enums.InstalledType",
        1, 0,
        "InstalledType",
        "Error: only enums");

    Qt::WindowFlags flags = m_window.flags();
    m_window.setFlags(flags | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint | Qt::BypassWindowManagerHint | Qt::SplashScreen);
    m_window.setColor(Qt::transparent);

    qmlRegisterSingletonInstance<WidgetWindow>("ScreenPlayWidget", 1, 0, "Widget", this);

#ifdef Q_OS_WIN
    m_hwnd = reinterpret_cast<HWND>(m_window.winId());
    setWindowBlur();
#endif

    if (projectPath == "test") {
        setProjectSourceFileAbsolute({ "qrc:/test.qml" });
        setType(ScreenPlay::InstalledType::InstalledType::QMLWidget);
    } else {
        setProjectPath(projectPath);
        auto projectOpt = ScreenPlayUtil::openJsonFileToObject(m_projectPath + "/project.json");
        if (!projectOpt.has_value()) {
            qWarning() << "Unable to parse project file!";
        }

        m_project = projectOpt.value();
        setProjectSourceFile(m_project.value("file").toString());
        setProjectSourceFileAbsolute(QUrl::fromLocalFile(m_projectPath + "/" + projectSourceFile()));

        if (auto typeOpt = ScreenPlayUtil::getInstalledTypeFromString(m_project.value("type").toString())) {
            setType(typeOpt.value());
        } else {
            qWarning() << "Cannot parse Wallpaper type from value" << m_project.value("type");
        }
    }

    m_window.setTextRenderType(QQuickWindow::TextRenderType::NativeTextRendering);
    m_window.setResizeMode(QQuickView::ResizeMode::SizeViewToRootObject);
    m_window.setSource(QUrl("qrc:/Widget.qml"));
    m_window.setPosition(m_position);
    m_window.show();

    // Debug mode means we directly start the ScreenPlayWallpaper for easy debugging.
    // This means we do not have a running ScreenPlay instance to connect to.
    if (!m_debugMode) {
        QObject::connect(m_sdk.get(), &ScreenPlaySDK::sdkDisconnected, this, &WidgetWindow::qmlExit);
        QObject::connect(m_sdk.get(), &ScreenPlaySDK::incommingMessage, this, &WidgetWindow::messageReceived);
        sdk()->start();
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

    setupLiveReloading();
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

/*!
  \brief Call the qml engine clearComponentCache. This function is used for
         refreshing wallpaper when the content has changed. For example this
         is needed for live editing when the content is chached.
*/
void WidgetWindow::clearComponentCache()
{
    m_window.engine()->clearComponentCache();
}

/*!
 \brief This public slot is for QML usage. We limit the change event updates
        to every 50ms, because the filesystem can be very trigger happy
        with multiple change events per second.
 */
void WidgetWindow::setupLiveReloading()
{
    auto reloadQMLLambda = [this]() {
        m_liveReloadLimiter.stop();
        emit reloadQML(type());
    };
    auto timeoutLambda = [this]() {
        m_liveReloadLimiter.start(50);
    };

    QObject::connect(&m_fileSystemWatcher, &QFileSystemWatcher::directoryChanged, this, timeoutLambda);
    QObject::connect(&m_fileSystemWatcher, &QFileSystemWatcher::fileChanged, this, timeoutLambda);
    QObject::connect(&m_liveReloadLimiter, &QTimer::timeout, this, reloadQMLLambda);
    m_fileSystemWatcher.addPaths({ projectPath() });
}
