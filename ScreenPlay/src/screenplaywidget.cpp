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
    const QPoint& position,
    const QString& absolutePath,
    const QString& previewImage,
    const InstalledType::InstalledType type)
    : QObject { nullptr }
    , m_globalVariables { globalVariables }
    , m_previewImage { previewImage }
    , m_appID { appID }
    , m_position { position }
    , m_type { type }
    , m_absolutePath { absolutePath }
{
    const QStringList proArgs {
        m_absolutePath,
        QString { "appID=" + m_appID },
        QVariant::fromValue(m_type).toString(),
        QString::number(m_position.x()),
        QString::number(m_position.y()),
    };

    m_process.setArguments(proArgs);
    m_process.setProgram(m_globalVariables->widgetExecutablePath().path());

    qDebug() << proArgs;

    QObject::connect(&m_process, &QProcess::errorOccurred, this, [](QProcess::ProcessError error) {
        qDebug() << "error: " << error;
    });
    m_process.startDetached();
}

QJsonObject ScreenPlayWidget::getActiveSettingsJson()
{
    QJsonObject obj;
    obj.insert("previewImage", m_previewImage);
    obj.insert("absolutePath", m_absolutePath);
    obj.insert("positionX", m_position.x());
    obj.insert("positionY", m_position.y());
    obj.insert("type", QVariant::fromValue(m_type).toString());
    return obj;
}
}
