#include "screenplaywallpaper.h"

namespace ScreenPlay {

/*!
    \class ScreenPlay::ScreenPlayWallpaper
    \inmodule ScreenPlay
    \brief A Single Object to manage a Wallpaper.

    This class is only for managing the QProcess to an extern ScreenPlayWallpaper!
*/

/*!
    \brief  Constructor for video Wallpaper.
*/
ScreenPlayWallpaper::ScreenPlayWallpaper(
    const QVector<int>& screenNumber,
    const std::shared_ptr<GlobalVariables>& globalVariables,
    const std::shared_ptr<SDKConnector>& sdkConnector,
    const QString& appID,
    const QString& absolutePath,
    const QString& previewImage,
    const QString& file,
    const float volume,
    const FillMode::FillMode fillMode,
    const InstalledType::InstalledType type,
    const bool checkWallpaperVisible,
    QObject* parent)
    : QObject(parent)
    , m_projectSettingsListModel { absolutePath + "/project.json" }
    , m_globalVariables { globalVariables }
    , m_sdkConnector { sdkConnector }
    , m_screenNumber { screenNumber }
    , m_previewImage { previewImage }
    , m_type { type }
    , m_fillMode { fillMode }
    , m_appID { appID }
    , m_absolutePath { absolutePath }
    , m_file { file }

{

    QObject::connect(&m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &ScreenPlayWallpaper::processExit);
    QObject::connect(&m_process, &QProcess::errorOccurred, this, &ScreenPlayWallpaper::processError);

    QString tmpScreenNumber;
    if (m_screenNumber.length() > 1) {
        for (const int number : qAsConst(m_screenNumber)) {
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
        QVariant::fromValue(fillMode).toString(),
        QVariant::fromValue(type).toString(),
        QString::number(checkWallpaperVisible),
        // Fixes issue 84 media key overlay
        " --disable-features=HardwareMediaKeyHandling"
    };

    m_process.setArguments(proArgs);
    m_process.setProgram(m_globalVariables->wallpaperExecutablePath().toString());
    m_process.startDetached();
}

QJsonObject ScreenPlayWallpaper::getActiveSettingsJson()
{
    QJsonArray screenNumber;
    for (const int i : m_screenNumber) {
        screenNumber.append(i);
    }

    QJsonObject obj;
    obj.insert("file", m_file);
    obj.insert("absolutePath", m_absolutePath);
    obj.insert("fillMode", QVariant::fromValue(m_fillMode).toString());
    obj.insert("isLooping", m_isLooping);
    obj.insert("monitors", screenNumber);
    obj.insert("previewImage", m_previewImage);
    obj.insert("type", QVariant::fromValue(m_type).toString());
    obj.insert("volume", m_volume);
    return obj;
}

/*!
    \brief Prints the exit code if != 0.
*/
void ScreenPlayWallpaper::processExit(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitStatus)
    if (exitCode != 0)
        qDebug() << "WARNING EXIT CODE: " << exitCode;
}

/*!
    \brief Prints the exit code error.
*/
void ScreenPlayWallpaper::processError(QProcess::ProcessError error)
{
    qDebug() << "EX: " << error;
}

ProjectSettingsListModel* ScreenPlayWallpaper::getProjectSettingsListModel()
{
    return &m_projectSettingsListModel;
}

void ScreenPlayWallpaper::setSDKConnection(const std::shared_ptr<SDKConnection> &connection)
{
    m_connection = connection;
    qInfo() << "App Wallpaper connected!";
    //QObject::connect(m_connection.get(),&SDKConnection::readyRead,this,[](){});
}

void ScreenPlayWallpaper::replace(
        const QString& absolutePath,
        const QString& previewImage,
        const QString& file,
        const float volume,
        const FillMode::FillMode fillMode,
        const InstalledType::InstalledType type,
        const bool checkWallpaperVisible)
{
    m_previewImage = previewImage;
    m_type = type;
    m_fillMode = fillMode;
    m_absolutePath = absolutePath;
    m_file = file;

    QJsonObject obj;
    obj.insert("command", "replace");
    obj.insert("type", QVariant::fromValue(type).toString());
    obj.insert("fillMode", QVariant::fromValue(fillMode).toString());
    obj.insert("volume", volume);
    obj.insert("absolutePath", absolutePath);
    obj.insert("file", file);
    obj.insert("checkWallpaperVisible", checkWallpaperVisible);

    m_sdkConnector->replace(m_appID, obj);
}

}
