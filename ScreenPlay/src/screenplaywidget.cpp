#include "screenplaywidget.h"

namespace ScreenPlay {

/*!
    \class ScreenPlay::ScreenPlayWidget
    \inmodule ScreenPlay
    \brief A Single Object to manage a Widget.

    This class is only for managing the QProcess to an extern ScreenPlayWidget!
*/

/*!
  \brief Constructor.
*/
ScreenPlayWidget::ScreenPlayWidget(
    const QString& appID,
    const std::shared_ptr<GlobalVariables>& globalVariables,
    const QString& projectPath,
    const QString& previewImage,
    const InstalledType::InstalledType type)
    : QObject { nullptr }
    , m_globalVariables { globalVariables }
    , m_projectPath { projectPath }
    , m_previewImage { previewImage }
    , m_appID { appID }
    , m_position { 0, 0 }
    , m_type { type }
{
    const QStringList proArgs {
        m_projectPath,
        QString { "appID=" + m_appID },
        QVariant::fromValue(m_type).toString()
    };

    m_process.setArguments(proArgs);
    m_process.setProgram(m_globalVariables->widgetExecutablePath().path());

    qDebug() << proArgs;

    QObject::connect(&m_process, &QProcess::errorOccurred, this, [](QProcess::ProcessError error) {
        qDebug() << "error: " << error;
    });
    m_process.startDetached();
}
}
