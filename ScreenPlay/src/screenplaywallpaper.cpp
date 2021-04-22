#include "screenplaywallpaper.h"

namespace ScreenPlay {

/*!
    \class ScreenPlay::ScreenPlayWallpaper
    \inmodule ScreenPlay
    \brief A Single Object to manage a Wallpaper.

    This class is only for managing the QProcess to an extern ScreenPlayWallpaper!
*/

/*!
    \brief Destructor for ScreenPlayWallpaper.
*/
ScreenPlayWallpaper::~ScreenPlayWallpaper()
{
    qInfo() << "Remove wallpaper " << m_appID;
    m_connection->close();
}

/*!
    \brief  Constructor for ScreenPlayWallpaper.
*/
ScreenPlayWallpaper::ScreenPlayWallpaper(const QVector<int>& screenNumber,
    const std::shared_ptr<GlobalVariables>& globalVariables,
    const QString& appID,
    const QString& absolutePath,
    const QString& previewImage,
    const QString& file,
    const float volume,
    const float playbackRate,
    const FillMode::FillMode fillMode,
    const InstalledType::InstalledType type,
    const QJsonObject& properties,
    const bool checkWallpaperVisible,
    QObject* parent)
    : QObject(parent)
    , m_globalVariables { globalVariables }
    , m_screenNumber { screenNumber }
    , m_previewImage { previewImage }
    , m_type { type }
    , m_fillMode { fillMode }
    , m_appID { appID }
    , m_absolutePath { absolutePath }
    , m_file { file }
    , m_volume { volume }
    , m_playbackRate { playbackRate }
{
    {
        // If
        QJsonObject projectSettingsListModelProperties;

        if (type == InstalledType::InstalledType::VideoWallpaper) {
            projectSettingsListModelProperties.insert("volume", m_volume);
            projectSettingsListModelProperties.insert("playbackRate", m_playbackRate);
        } else {
            if (properties.isEmpty()) {
                auto obj = ScreenPlayUtil::openJsonFileToObject(absolutePath + "/project.json");
                if (!obj)
                    return;

                if (obj->contains("properties"))
                    projectSettingsListModelProperties = obj->value("properties").toObject();
            } else {
                projectSettingsListModelProperties = properties;
            }
        }
        m_projectSettingsListModel.init(type, projectSettingsListModelProperties);
    }

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

    m_appArgumentsList = QStringList {
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
}

bool ScreenPlayWallpaper::start()
{
    m_process.setArguments(m_appArgumentsList);
    m_process.setProgram(m_globalVariables->wallpaperExecutablePath().toString());
    // We must start detatched otherwise we would instantly close the process
    // and would loose the animted fade-out and the background refresh needed
    // to display the original wallpaper.
    const bool success = m_process.startDetached();
    qInfo() << "Starting ScreenPlayWallpaper detached: " << (success ? "success" : "failed!");
    if (!success) {
        emit error(QString("Could not start Wallpaper: " + m_process.errorString()));
    }
    return success;
}

/*!
    \brief Loads the project.json that contains all settings to display
            the wallpaper.
*/
QJsonObject ScreenPlayWallpaper::getActiveSettingsJson()
{
    QJsonArray screenNumber;
    for (const int i : qAsConst(m_screenNumber)) {
        screenNumber.append(i);
    }

    QJsonObject obj;
    QJsonObject properties;
    if (m_type == InstalledType::InstalledType::VideoWallpaper) {
        obj.insert("fillMode", QVariant::fromValue(m_fillMode).toString());
        obj.insert("isLooping", m_isLooping);
        obj.insert("volume", m_volume);
        obj.insert("playbackRate", m_playbackRate);
    } else {
        QJsonObject properties = m_projectSettingsListModel.getActiveSettingsJson();
        if (!properties.isEmpty())
            obj.insert("properties", properties);
    }
    obj.insert("file", m_file);
    obj.insert("absolutePath", m_absolutePath);
    obj.insert("monitors", screenNumber);
    obj.insert("previewImage", m_previewImage);
    obj.insert("type", QVariant::fromValue(m_type).toString());
    return obj;
}

/*!
    \brief Prints the exit code if != 0.
*/
void ScreenPlayWallpaper::processExit(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitStatus)
    if (exitCode != 0)
        qWarning() << "WARNING EXIT CODE: " << exitCode;
}

/*!
    \brief Prints the exit code error.
*/
void ScreenPlayWallpaper::processError(QProcess::ProcessError error)
{
    qWarning() << "EX: " << error;
}

/*!
    \brief Sets a wallpaper value. We directly set the property if it is either volume,
        playbackRate or fillMode. Otherwise it is a simple key, value json pair.

*/
bool ScreenPlayWallpaper::setWallpaperValue(const QString& key, const QString& value, const bool save)
{
    QJsonObject obj;
    obj.insert(key, value);

    if (key == "volume") {
        setVolume(value.toFloat());
    }
    if (key == "playbackRate") {
        setPlaybackRate(value.toFloat());
    }
    if (key == "fillmode") {
        setFillMode(QStringToEnum<FillMode::FillMode>(value, FillMode::FillMode::Cover));
    }

    const bool success = m_connection->sendMessage(QJsonDocument(obj).toJson(QJsonDocument::Compact));

    if (save && success)
        emit requestSave();

    return success;
}

/*!
    \brief  Connects to ScreenPlay. Start a alive ping check for every
            GlobalVariables::contentPingAliveIntervalMS seconds.
*/
void ScreenPlayWallpaper::setSDKConnection(std::unique_ptr<SDKConnection> connection)
{
    m_connection = std::move(connection);

    QTimer::singleShot(1000, this, [this]() {
        if (playbackRate() != 1.0) {
            setWallpaperValue("playbackRate", QString::number(playbackRate()), false);
        }

        QObject::connect(&m_pingAliveTimer, &QTimer::timeout, this, [this]() {
            qInfo() << "For " << m_pingAliveTimer.interval() << "ms no alive signal received. This means the Wallpaper is dead and likely crashed!";
            emit requestClose(m_appID);
        });
        m_pingAliveTimer.start(GlobalVariables::contentPingAliveIntervalMS);
    });
    // Check every X seconds if the wallpaper is still alive
    QObject::connect(m_connection.get(), &SDKConnection::pingAliveReceived, this, [this]() {
        m_pingAliveTimer.stop();
        m_pingAliveTimer.start(GlobalVariables::contentPingAliveIntervalMS);
    });
}

/*!
    \brief Replaces the current wallpaper with the given one.
*/
void ScreenPlayWallpaper::replace(
    const QString& absolutePath,
    const QString& previewImage,
    const QString& file,
    const float volume,
    const FillMode::FillMode fillMode,
    const InstalledType::InstalledType type,
    const bool checkWallpaperVisible)
{
    if (!m_connection)
        return;

    m_previewImage = previewImage;
    m_type = type;
    m_fillMode = fillMode;
    m_absolutePath = absolutePath;
    m_file = file;

    QJsonObject obj;
    obj.insert("command", "replace");
    obj.insert("type", QVariant::fromValue(type).toString());
    obj.insert("fillMode", QVariant::fromValue(fillMode).toString());
    obj.insert("volume", std::floor(volume * 100.0F) / 100.0f);
    obj.insert("absolutePath", absolutePath);
    obj.insert("file", file);
    obj.insert("checkWallpaperVisible", checkWallpaperVisible);

    m_connection->sendMessage(QJsonDocument(obj).toJson(QJsonDocument::Compact));
    emit requestSave();
}

}
