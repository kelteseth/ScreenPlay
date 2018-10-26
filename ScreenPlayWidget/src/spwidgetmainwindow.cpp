#include "spwidgetmainwindow.h"

SPWidgetmainwindow::SPWidgetmainwindow(QString projectPath, QString appid, QScreen* parent)
    : QWindow(parent)
{

    m_appID = appid;

    m_hwnd = (HWND)this->winId();
    Qt::WindowFlags flags = this->flags();
    this->setWidth(500);
    this->setHeight(300);
    this->setFlags(flags | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint | Qt::BypassWindowManagerHint | Qt::SplashScreen);

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

    m_quickRenderer = QSharedPointer<QQuickView>(new QQuickView(this));
    m_quickRenderer.data()->rootContext()->setContextProperty("backend", this);
    m_quickRenderer.data()->setColor(Qt::transparent);
    m_quickRenderer.data()->setWidth(this->width());
    m_quickRenderer.data()->setHeight(this->height());
    m_quickRenderer.data()->setResizeMode(QQuickView::ResizeMode::SizeRootObjectToView);

    m_quickRenderer.data()->setSource(QUrl("qrc:/main.qml"));
#ifdef Q_OS_WIN
    SetWindowBlur(m_hwnd);
#endif
    show();
    m_quickRenderer.data()->show();

    emit setWidgetSource(fullPath);
}

void SPWidgetmainwindow::setSize(QSize size)
{
    this->setWidth(size.width());
    this->setHeight(size.height());
    m_quickRenderer.data()->setWidth(size.width());
    m_quickRenderer.data()->setHeight(size.height());
}

void SPWidgetmainwindow::setPos(int xPos, int yPos)
{

    QPoint delta((xPos - m_clickPos.x()), (yPos - m_clickPos.y()));
    int new_x = x() + delta.x();
    int new_y = y() + delta.y();

    setPosition(QPoint(new_x, new_y));
}

void SPWidgetmainwindow::setClickPos(const QPoint &clickPos)
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
