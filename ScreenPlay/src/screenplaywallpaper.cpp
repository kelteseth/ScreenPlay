#include "screenplaywallpaper.h"

/*!
    \class ScreenPlayWallpaper
    \brief Used for ...
*/
namespace ScreenPlay {

ScreenPlayWallpaper::ScreenPlayWallpaper(const vector<int>& screenNumber,
    const shared_ptr<Settings>& settings,
    const QString& appID,
    const QString& projectPath,
    const QString& previewImage,
    const float volume,
    const QString& fillMode,
    const QString& type,
    QObject* parent)
    : QObject(parent)
    , m_projectSettingsListModel { make_shared<ProjectSettingsListModel>(projectPath + "/project.json") }
    , m_screenNumber { move(screenNumber) }
    , m_projectPath { projectPath }
    , m_previewImage { previewImage }
    , m_appID { appID }
    , m_type { type }
    , m_settings { settings }
{

    QObject::connect(&m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
        [=](int exitCode, QProcess::ExitStatus exitStatus) {
            Q_UNUSED(exitStatus)
            if (exitCode != 0)
                qDebug() << "WARNING EXIT CODE: " << exitCode;
        });

    QObject::connect(&m_process, &QProcess::errorOccurred, [=](QProcess::ProcessError error) {
        qDebug() << "EX: " << error;
    });

    const QStringList proArgs {
        QString::number(m_screenNumber.empty() ? 0 : m_screenNumber[0]),
        m_projectPath,
        QString { "appID=" + m_appID },
        "",
        QString::number(static_cast<double>(volume)),
        fillMode
    };

    //qDebug() << "Creating ScreenPlayWallpaper " << proArgs;

    m_process.setArguments(proArgs);
    m_process.setProgram(m_settings->screenPlayWindowPath().toString());
    m_process.startDetached();
}
}
