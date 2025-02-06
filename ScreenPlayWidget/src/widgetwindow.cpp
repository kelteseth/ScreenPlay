// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#include "widgetwindow.h"

#include <QDirIterator>
#include <QGuiApplication>
#include <QSysInfo>

#include "ScreenPlayCore/contenttypes.h"
#include "ScreenPlayCore/processmanager.h"
#include "ScreenPlayCore/util.h"

namespace ScreenPlay {
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
    const qint64 mainAppPID,
    std::shared_ptr<QQuickView> quickView,
    const bool debugMode)
    : QObject(nullptr)
    , m_quickView { quickView }
    , m_appID { appID }
    , m_position { position }
    , m_debugMode { debugMode }
{
    setMainAppPID(mainAppPID);

            Qt::WindowFlags flags = m_quickView->flags();
    if (QSysInfo::productType() == "macos") {
        // Setting it as a SlashScreen causes the window to hide on focus lost
        m_quickView->setFlags(flags | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
    } else if (QSysInfo::productType() == "windows") {
        // Must be splash screen to not show up in the taskbar
        m_quickView->setFlags(flags | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint | Qt::BypassWindowManagerHint | Qt::SplashScreen);
    }

    

#ifdef Q_OS_WIN
    m_hwnd = reinterpret_cast<HWND>(m_quickView->winId());
    setWindowBlur();
#endif

    ScreenPlay::Util util;
    if (projectPath == "test") {
        setProjectSourceFileAbsolute({ "qrc:/qml/ScreenPlayWidget/qml/Test.qml" });
        setType(ScreenPlay::ContentTypes::InstalledType::QMLWidget);
    } else {
        setProjectPath(projectPath);
        auto projectOpt = util.openJsonFileToObject(m_projectPath + "/project.json");
        if (!projectOpt.has_value()) {
            qWarning() << "Unable to parse project file!";
            return;
        }

        m_project = projectOpt.value();
        setProjectSourceFile(m_project.value("file").toString());
        setProjectSourceFileAbsolute(QUrl::fromLocalFile(m_projectPath + "/" + projectSourceFile()));

        if (auto typeOpt = util.getInstalledTypeFromString(m_project.value("type").toString())) {
            setType(typeOpt.value());
        } else {
            qWarning() << "Cannot parse Wallpaper type from value" << m_project.value("type");
        }
    }

    // Debug mode means we directly start the ScreenPlayWallpaper for easy debugging.
    // This means we do not have a running ScreenPlay instance to connect to.
    if (!m_debugMode) {
        m_sdk = std::make_unique<ScreenPlaySDK>(appID, type);
        m_sdk->setMainAppPID(mainAppPID);
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
                obj.insert("positionX", m_quickView->x());
                obj.insert("positionY", m_quickView->y());
                m_sdk->sendMessage(obj);
            };
            m_positionMessageLimiter.setInterval(500);

            QObject::connect(&m_positionMessageLimiter, &QTimer::timeout, this, sendPositionUpdate);
            QObject::connect(m_quickView.get(), &QWindow::xChanged, this, [this]() { m_positionMessageLimiter.start(); });
            QObject::connect(m_quickView.get(), &QWindow::yChanged, this, [this]() { m_positionMessageLimiter.start(); });
        });
    }
    
}

void WidgetWindow::start(){

    auto* guiAppInst = dynamic_cast<QGuiApplication*>(QGuiApplication::instance());
    m_quickView->engine()->addImportPath(guiAppInst->applicationDirPath() + "/qml");
    m_quickView->setResizeMode(QQuickView::ResizeMode::SizeViewToRootObject);
    m_quickView->loadFromModule("ScreenPlayWidget", "ScreenPlayWidgetMain");
    m_quickView->setColor(Qt::transparent);
    m_quickView->setPosition(m_position);


    setupLiveReloading();
}

void WidgetWindow::setSize(QSize size)
{
    m_quickView->setWidth(size.width());
    m_quickView->setHeight(size.height());
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
    m_quickView->setPosition({ xPos, yPos });
}

void WidgetWindow::setClickPos(const QPoint& clickPos)
{
    m_clickPos = clickPos;
}

void WidgetWindow::setWidgetSize(const int with, const int height)
{
    m_quickView->setWidth(with);
    m_quickView->setHeight(height);
}

#ifdef Q_OS_WIN
void WidgetWindow::setWindowBlur(quint64 style)
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
    m_quickView->engine()->clearComponentCache();
}

void WidgetWindow::show()
{
    m_quickView->show();
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

    QDirIterator projectFilesIter(projectPath(), { "*.qml", "*.html", "*.css", "*.js" }, QDir::Files | QDir::NoSymLinks, QDirIterator::Subdirectories);
    m_fileSystemWatcher.addPath({ projectPath() });
    while (projectFilesIter.hasNext()) {
        m_fileSystemWatcher.addPath(projectFilesIter.next());
    }
}
qint64 WidgetWindow::mainAppPID() const
{
    return m_mainAppPID;
}

void WidgetWindow::setMainAppPID(qint64 mainAppPID)
{
    if (m_mainAppPID == mainAppPID)
        return;
    m_mainAppPID = mainAppPID;
    emit mainAppPIDChanged(m_mainAppPID);
}
}
#include "moc_widgetwindow.cpp"
