#include "screenplaywidget.h"

/*!
    \class ScreenPlayWidget
    \brief Used for ...
*/

ScreenPlayWidget::ScreenPlayWidget(const QString& appID, const shared_ptr<Settings>& settings,
    const QString& projectPath,
    const QString& previewImage,
    const QString& fullPath,
    QObject* parent)
    : QObject { parent }
    , m_projectPath { projectPath }
    , m_previewImage { previewImage }
    , m_appID { appID }
    , m_position { 0, 0 }
    , m_settings(settings)
{
    const QStringList proArgs { m_projectPath, m_appID };
    m_process.setArguments(proArgs);

    if (fullPath.endsWith(".exe")) {
        m_process.setProgram(fullPath);
    } else if (fullPath.endsWith(".qml")) {
        m_process.setProgram(m_settings->getScreenPlayWidgetPath().path());
    }

    qDebug() << m_process.program();

    QObject::connect(&m_process, &QProcess::errorOccurred, this, [](QProcess::ProcessError error) {
        qDebug() << "error: " << error;
    });
    m_process.startDetached();
}
