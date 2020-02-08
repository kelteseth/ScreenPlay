#include "widgetwindow.h"

#include <QCoreApplication>

WidgetWindow::WidgetWindow(const QString projectPath, const QString appid, const QString type)
    : QObject(nullptr)
    , m_appID { appid }
    , m_type { type }
{
    QStringList availableTypes {
        "qmlWidget",
        "htmlWidget"
    };

    if (!availableTypes.contains(m_type)) {
        QGuiApplication::exit(-4);
    }

    Qt::WindowFlags flags = m_window.flags();
    m_window.setWidth(300);
    m_window.setHeight(300);
    m_window.setFlags(flags | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint | Qt::BypassWindowManagerHint | Qt::SplashScreen);
    m_window.setColor(Qt::transparent);

    qmlRegisterSingletonInstance<WidgetWindow>("ScreenPlayWidget", 1, 0, "Widget", this);

#ifdef Q_OS_WIN
    m_hwnd = reinterpret_cast<HWND>(m_window.winId());
    setWindowBlur();
#endif

    if (projectPath != "test") {
        QFile configTmp;
        QJsonDocument configJsonDocument;
        QJsonParseError parseError;

        configTmp.setFileName(projectPath + "/project.json");
        configTmp.open(QIODevice::ReadOnly | QIODevice::Text);
        m_projectConfig = configTmp.readAll();
        configJsonDocument = QJsonDocument::fromJson(m_projectConfig.toUtf8(), &parseError);

        if (!(parseError.error == QJsonParseError::NoError)) {
            qWarning("Settings Json Parse Error ");
        }

        m_project = configJsonDocument.object();
        QString fullPath = "file:///" + projectPath + "/" + m_project.value("file").toString();
        setSourcePath(fullPath);
    } else {
        setSourcePath("qrc:/test.qml");
    }
    // Instead of setting "renderType: Text.NativeRendering" every time
    // we can set it here once :)

    m_window.setTextRenderType(QQuickWindow::TextRenderType::NativeTextRendering);
    //    m_window.setResizeMode(QQuickView::ResizeMode::SizeViewToRootObject);
    m_window.setSource(QUrl("qrc:/mainWidget.qml"));
    m_window.show();
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
