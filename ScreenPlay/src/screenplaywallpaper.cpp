#include "screenplaywallpaper.h"


namespace ScreenPlay {

/*!
    \class ScreenPlay::ScreenPlayWallpaper
    \inmodule ScreenPlay
    \brief ScreenPlayWallpaper.
*/

ScreenPlayWallpaper::ScreenPlayWallpaper(
    const QVector<int>& screenNumber,
    const shared_ptr<GlobalVariables>& globalVariables,
    const QString& appID,
    const QString& absolutePath,
    const QString& previewImage,
    const float volume,
    const QString& fillMode,
    const QString& type,
    QObject* parent)
    : QObject(parent)
    , m_projectSettingsListModel { make_shared<ProjectSettingsListModel>(absolutePath + "/project.json") }
    , m_globalVariables { globalVariables }
    , m_screenNumber { screenNumber }
    , m_previewImage { QString { absolutePath + "/" + previewImage } }
    , m_type { type }
    , m_appID { appID }
    , m_absolutePath { absolutePath }
{

    QObject::connect(&m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &ScreenPlayWallpaper::processExit);
    QObject::connect(&m_process, &QProcess::errorOccurred, this, &ScreenPlayWallpaper::processError);

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
        m_absolutePath,
        QString { "appID=" + m_appID },
        QString::number(static_cast<double>(volume)),
        fillMode,
        type
    };

    qDebug() << "Creating ScreenPlayWallpaper " << proArgs;

    m_process.setArguments(proArgs);
    m_process.setProgram(m_globalVariables->wallpaperExecutablePath().toString());
    m_process.startDetached();
}

ScreenPlayWallpaper::ScreenPlayWallpaper(
    const QVector<int>& screenNumber,
    const shared_ptr<GlobalVariables>& globalVariables,
    const QString& appID,
    const QString& absolutePath,
    const QString& previewImage,
    const QString& type,
    const QJsonObject& profileJsonObject,
    QObject* parent)
    : QObject(parent)
    , m_projectSettingsListModel { make_shared<ProjectSettingsListModel>(absolutePath + "/project.json") }
    , m_globalVariables { globalVariables }
    , m_screenNumber { screenNumber }
    , m_previewImage { QString { absolutePath + "/" + previewImage } }
    , m_type { type }
    , m_appID { appID }
    , m_absolutePath { absolutePath }
    , m_profileJsonObject { profileJsonObject }
{
    QObject::connect(&m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &ScreenPlayWallpaper::processExit);
    QObject::connect(&m_process, &QProcess::errorOccurred, this, &ScreenPlayWallpaper::processError);

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
        m_absolutePath,
        QString { "appID=" + m_appID },
        QString::number(static_cast<double>(1)),
        "fill",
        type
    };

    qDebug() << "Creating ScreenPlayWallpaper " << proArgs;

    m_process.setArguments(proArgs);
    m_process.setProgram(m_globalVariables->wallpaperExecutablePath().toString());
    m_process.startDetached();
}

void ScreenPlayWallpaper::processExit(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitStatus)
    if (exitCode != 0)
        qDebug() << "WARNING EXIT CODE: " << exitCode;
}

void ScreenPlayWallpaper::processError(QProcess::ProcessError error)
{
    qDebug() << "EX: " << error;
}

}
