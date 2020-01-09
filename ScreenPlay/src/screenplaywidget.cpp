#include "screenplaywidget.h"

namespace ScreenPlay {

/*!
    \class ScreenPlay::ScreenPlayWidget
    \inmodule ScreenPlay
    \brief A Single Object to manage a Widget.

    This class is only for managing the QProcess to an extern ScreenPlayWidget!
*/

ScreenPlayWidget::ScreenPlayWidget(
    const QString& appID,
    const shared_ptr<GlobalVariables>& globalVariables,
    const QString& projectPath,
    const QString& previewImage,
    const QString& fullPath,
    QObject* parent)
    : QObject { parent }
    , m_globalVariables { globalVariables }
    , m_projectPath { projectPath }
    , m_previewImage { previewImage }
    , m_appID { appID }
    , m_position { 0, 0 }
{
    const QStringList proArgs { m_projectPath, m_appID };
    m_process.setArguments(proArgs);

    if (fullPath.endsWith(".exe")) {
        m_process.setProgram(fullPath);
    } else if (fullPath.endsWith(".qml")) {
        m_process.setProgram(m_globalVariables->widgetExecutablePath().path());
    }

    qDebug() << m_process.program();

    QObject::connect(&m_process, &QProcess::errorOccurred, this, [](QProcess::ProcessError error) {
        qDebug() << "error: " << error;
    });
    m_process.startDetached();
}
}
