#include "linuxwindow.h"

LinuxWindow::LinuxWindow(QVector<int> activeScreensList, QString projectPath, QString id, QString volume, const QString fillmode, QObject* parent)
    : BaseWindow(projectPath)
{
    setAppID(id);
    bool ok = false;
    float volumeParsed = volume.toFloat(&ok);
    if (!ok) {
        qFatal("Could not parse volume");
    }
    setVolume(volumeParsed);

    // WARNING: Setting Window flags must be called *here*!
    Qt::WindowFlags flags = m_window.flags();
    m_window.setFlags(flags | Qt::FramelessWindowHint | Qt::Desktop);

    m_window.setResizeMode(QQuickView::ResizeMode::SizeRootObjectToView);
    m_window.rootContext()->setContextProperty("window", this);
    // Instead of setting "renderType: Text.NativeRendering" every time
    // we can set it here once :)
    m_window.setTextRenderType(QQuickWindow::TextRenderType::NativeTextRendering);
    m_window.setSource(QUrl("qrc:/mainWindow.qml"));
}

void LinuxWindow::setVisible(bool show)
{
    m_window.setVisible(show);
}

void LinuxWindow::destroyThis()
{
    QCoreApplication::quit();
}

