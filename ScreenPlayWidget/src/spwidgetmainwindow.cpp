#include "spwidgetmainwindow.h"

#include <QCoreApplication>

SPWidgetmainwindow::SPWidgetmainwindow(QString projectPath, QString appid, QObject* parent)
    : QObject(parent)
{

    m_appID = appid;

    m_hwnd = reinterpret_cast<HWND>(m_window.winId());
    Qt::WindowFlags flags = m_window.flags();
    m_window.setWidth(500);
    m_window.setHeight(300);
    m_window.setFlags(flags | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint | Qt::BypassWindowManagerHint | Qt::SplashScreen);

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
    QString fullPath = projectPath + "/" + m_project.value("file").toString();

    m_window.rootContext()->setContextProperty("backend", this);
    m_window.setColor(Qt::transparent);
    m_window.setResizeMode(QQuickView::ResizeMode::SizeRootObjectToView);

    m_window.setSource(QUrl("qrc:/main.qml"));
#ifdef Q_OS_WIN
    SetWindowBlur(m_hwnd);
#endif
    m_window.show();

    emit setWidgetSource(fullPath);
}

void SPWidgetmainwindow::setSize(QSize size)
{
    m_window.setWidth(size.width());
    m_window.setHeight(size.height());
}

void SPWidgetmainwindow::destroyThis()
{
    QCoreApplication::quit();
}

void SPWidgetmainwindow::messageReceived(QString key, QString value)
{
    emit qmlSceneValueReceived(key, value);
}

void SPWidgetmainwindow::setPos(int xPos, int yPos)
{

    QPoint delta((xPos - m_clickPos.x()), (yPos - m_clickPos.y()));
    int new_x = m_window.x() + delta.x();
    int new_y = m_window.y() + delta.y();

    m_window.setPosition(QPoint(new_x, new_y));
}

void SPWidgetmainwindow::setClickPos(const QPoint& clickPos)
{
    m_clickPos = clickPos;
}

void SPWidgetmainwindow::SetWindowBlur(HWND hWnd)
{
#ifdef Q_OS_WIN

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
            ACCENTPOLICY policy = { (int)Accent::BLURBEHIND, 0, 0, 0 }; // ACCENT_ENABLE_BLURBEHIND=3...
            WINCOMPATTRDATA data = { 19, &policy, sizeof(ACCENTPOLICY) }; // WCA_ACCENT_POLICY=19
            SetWindowCompositionAttribute(hWnd, &data);
        }
        FreeLibrary(hModule);
    }

#endif
}
