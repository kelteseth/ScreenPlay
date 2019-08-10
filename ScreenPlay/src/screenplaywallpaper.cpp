#include "screenplaywallpaper.h"

/*!
    \class ScreenPlayWallpaper
    \brief Used for ...
*/
namespace ScreenPlay {

ScreenPlayWallpaper::ScreenPlayWallpaper(const QVector<int>& screenNumber,
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
    , m_screenNumber { screenNumber }
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

    QString tmpScreenNumber;
    if (m_screenNumber.length() > 1) {
        for (const int number : m_screenNumber) {
            // IMPORTANT: NO TRAILING COMMA!
            if (number == m_screenNumber.back()) {
                tmpScreenNumber += QString::number(number);
            } else {
                tmpScreenNumber += QString::number(number) + ",";
            }
        }
    } else {
        tmpScreenNumber = QString::number(m_screenNumber.first());
    }

    const QStringList proArgs {
        tmpScreenNumber,
        m_projectPath,
        QString { "appID=" + m_appID },
        "",
        QString::number(static_cast<double>(volume)),
        fillMode
    };

    qDebug() << "Creating ScreenPlayWallpaper " << proArgs;

    m_process.setArguments(proArgs);
    m_process.setProgram(m_settings->screenPlayWallpaperPath().toString());
    m_process.startDetached();
}
}
