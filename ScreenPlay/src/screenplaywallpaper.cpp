// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#include "ScreenPlay/screenplaywallpaper.h"
#include "ScreenPlayUtil/util.h"

#include <QDebug>
#include <QDir>
#include <QFileInfoList>

namespace ScreenPlay {

/*!
    \class ScreenPlay::ScreenPlayWallpaper
    \inmodule ScreenPlay
    \brief A Single Object to manage a Wallpaper.

    This class is only for managing the QProcess to an extern ScreenPlayWallpaper!
*/

/*!
    \brief  Constructor for ScreenPlayWallpaper.
*/
ScreenPlayWallpaper::ScreenPlayWallpaper(
    const std::shared_ptr<GlobalVariables>& globalVariables,
    const QString& appID,
    const WallpaperData wallpaperData,
    const std::shared_ptr<Settings>& settings,
    QObject* parent)
    : QObject(parent)
    , m_globalVariables { globalVariables }
    , m_wallpaperData { wallpaperData }
    , m_settings { settings }
    , m_appID { appID }
{
    Util util;
    std::optional<QJsonObject> projectOpt = util.openJsonFileToObject(m_wallpaperData.absolutePath()
                                                                      + "/project.json");
    if (projectOpt.has_value()) {
        m_projectJson = projectOpt.value();
    }
    QJsonObject projectSettingsListModelProperties;
    if (m_wallpaperData.type() == ContentTypes::InstalledType::VideoWallpaper) {
        projectSettingsListModelProperties.insert("volume", m_wallpaperData.volume());
        projectSettingsListModelProperties.insert("playbackRate", m_wallpaperData.playbackRate());
    } else {
        if (m_wallpaperData.properties().isEmpty()) {
            if (auto obj = util.openJsonFileToObject(m_wallpaperData.absolutePath()
                                                     + "/project.json")) {
                if (obj->contains("properties"))
                    projectSettingsListModelProperties = obj->value("properties").toObject();
            }
        } else {
            projectSettingsListModelProperties = m_wallpaperData.properties();
        }
    }

    if (!projectSettingsListModelProperties.isEmpty())
        m_projectSettingsListModel.init(m_wallpaperData.type(), projectSettingsListModelProperties);

    QObject::connect(&m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &ScreenPlayWallpaper::processExit);
    QObject::connect(&m_process, &QProcess::errorOccurred, this, &ScreenPlayWallpaper::processError);

    QString tmpScreenNumber;
    if (m_wallpaperData.monitors().length() > 1) {
        for (const int number : m_wallpaperData.monitors()) {
            // IMPORTANT: NO TRAILING COMMA!
            if (number == m_wallpaperData.monitors().back()) {
                tmpScreenNumber += QString::number(number);
            } else {
                tmpScreenNumber += QString::number(number) + ",";
            }
        }
    } else {
        tmpScreenNumber = QString::number(m_wallpaperData.monitors().first());
    }

    const QString screens = "{" + tmpScreenNumber + "}";

    m_appArgumentsList = QStringList{"--screens",
                                     screens,
                                     "--projectpath",
                                     m_wallpaperData.absolutePath(),
                                     "--appID",
                                     m_appID,
                                     "--volume",
                                     QString::number(static_cast<double>(m_wallpaperData.volume())),
                                     "--fillmode",
                                     QVariant::fromValue(m_wallpaperData.fillMode()).toString(),
                                     "--type",
                                     QVariant::fromValue(m_wallpaperData.type()).toString(),
                                     "--check",
                                     QString::number(m_settings->checkWallpaperVisible()),
                                     "--mainapppid",
                                     QString::number(m_processManager.getCurrentPID())};

    // Fixes issue 84 media key overlay in Qt apps
    if (m_wallpaperData.type() != ContentTypes::InstalledType::GodotWallpaper) {
        m_appArgumentsList.append(" --disable-features=HardwareMediaKeyHandling");
    }
    if (m_wallpaperData.type() == ContentTypes::InstalledType::GodotWallpaper) {
        if (m_projectJson.contains("version")) {
            const quint64 version = m_projectJson.value("version").toInt();
            const QString packageFileName = QString("project-v%1.zip").arg(version);
            m_appArgumentsList.append({ "--projectPackageFile", packageFileName });
        }
    }
}

bool ScreenPlayWallpaper::start()
{
    if (m_wallpaperData.type() == ContentTypes::InstalledType::GodotWallpaper) {
        m_process.setProgram(m_globalVariables->godotWallpaperExecutablePath().toString());
    } else {
        m_process.setProgram(m_globalVariables->wallpaperExecutablePath().toString());
    }

    // We must start detatched otherwise we would instantly close the process
    // and would loose the animted fade-out and the background refresh needed
    // to display the original wallpaper.
    m_process.setArguments(m_appArgumentsList);
    const bool success = m_process.startDetached(&m_processID);
    emit processIDChanged(m_processID);
    qInfo() << "Starting ScreenPlayWallpaper detached: " << (success ? "success" : "failed!") << m_process.program();
    qInfo() << m_appArgumentsList;
    if (!success) {
        qInfo() << m_process.program() << m_appArgumentsList;
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
    for (const int i : m_wallpaperData.monitors()) {
        screenNumber.append(i);
    }

    QJsonObject obj;

    if (m_wallpaperData.type() == ContentTypes::InstalledType::VideoWallpaper) {
        obj.insert("fillMode", QVariant::fromValue(m_wallpaperData.fillMode()).toString());
        obj.insert("isLooping", m_wallpaperData.isLooping());
        obj.insert("volume", m_wallpaperData.volume());
        obj.insert("playbackRate", m_wallpaperData.playbackRate());
    } else {
        QJsonObject properties = m_projectSettingsListModel.getActiveSettingsJson();
        if (!properties.isEmpty()) {
            obj.insert("properties", properties);
        }
    }

    obj.insert("file", m_wallpaperData.file());
    obj.insert("absolutePath", m_wallpaperData.absolutePath());
    obj.insert("monitors", screenNumber);
    obj.insert("previewImage", m_wallpaperData.previewImage());
    obj.insert("type", QVariant::fromValue(m_wallpaperData.type()).toString());

    return obj;
}

/*!
    \brief Sends command quit to the wallpaper.
*/
void ScreenPlayWallpaper::close()
{
    setState(ScreenPlayEnums::AppState::Closing);
    qInfo() << "Close wallpaper with appID:" << m_appID;

    m_pingAliveTimer.stop(); // Stop the timer when closing

    // When the wallpaper never connected, this is invalid
    if (!m_connection) {
        qCritical() << "Cannot request quit, wallpaper never connected!";
        setState(ScreenPlayEnums::AppState::Inactive);
        return;
    }

    if (!m_connection->close()) {
        qCritical() << "Cannot close wallpaper!";
        setState(ScreenPlayEnums::AppState::Inactive);
        return;
    }
    // The state will be set to Inactive in the disconnected callback
}
/*!
    \brief Prints the exit code if != 0.
*/
void ScreenPlayWallpaper::processExit(int exitCode, QProcess::ExitStatus exitStatus)
{
    setState(ScreenPlay::ScreenPlayEnums::AppState::Inactive);
    if (exitCode != 0) {
        qCritical() << "ERROR: Wallpaper closed with appID: " << m_appID << " EXIT CODE: " << exitCode << exitStatus;
        return;
    }
    qDebug() << "Wallpaper closed with appID: " << m_appID;
}

/*!
    \brief Prints the exit code error.
*/
void ScreenPlayWallpaper::processError(QProcess::ProcessError error)
{
    setState(ScreenPlay::ScreenPlayEnums::AppState::Inactive);
    qWarning() << "EX: " << error;
}

/*!
    \brief Sets a wallpaper value. We directly set the property if it is either volume,
        playbackRate or fillMode. Otherwise it is a simple key, value json pair.

*/

bool ScreenPlayWallpaper::setWallpaperValue(const QString& key, const QString& value, const bool save)
{
    if (state() != ScreenPlayEnums::AppState::Active) {
        qWarning() << "Cannot set value for inactive or closing wallpaper!";
        return false;
    }

    if (!m_connection) {
        qWarning() << "Cannot set value for unconnected wallpaper!";
        return false;
    }

    QJsonObject obj;
    obj.insert(key, value);

    if (key == "volume") {
        setVolume(value.toFloat());
    }
    if (key == "playbackRate") {
        setPlaybackRate(value.toFloat());
    }
    if (key == "fillmode") {
        setFillMode(QStringToEnum<Video::FillMode>(value, Video::FillMode::Cover));
    }

    const bool success = m_connection->sendMessage(QJsonDocument(obj).toJson(QJsonDocument::Compact));

    if (save && success)
        emit requestSave(); // TODO REMOVE

    return success;
}

ScreenPlay::ScreenPlayEnums::AppState ScreenPlayWallpaper::state() const
{
    return m_state;
}

void ScreenPlayWallpaper::setState(ScreenPlay::ScreenPlayEnums::AppState state)
{
    m_state = state;
}

const WallpaperData& ScreenPlayWallpaper::wallpaperData()
{
    return m_wallpaperData;
}

/*!
    \brief  Connects to ScreenPlay. Start a alive ping check for every
            GlobalVariables::contentPingAliveIntervalMS seconds.
*/
void ScreenPlayWallpaper::setSDKConnection(std::unique_ptr<SDKConnection> connection)
{
    m_connection = std::move(connection);
    qInfo() << "[4/4] SDKConnection (Wallpaper) saved!";
    setIsConnected(true);
    setState(ScreenPlayEnums::AppState::Active);

    QObject::connect(m_connection.get(), &SDKConnection::disconnected, this, [this]() {
        setIsConnected(false);
        setState(ScreenPlayEnums::AppState::Inactive);
        m_pingAliveTimer.stop(); // Stop the timer when disconnected
        qInfo() << "Wallpaper:" << m_connection->appID() << "disconnected";
    });

    QTimer::singleShot(1000, this, [this]() {
        if (playbackRate() != 1.0) {
            setWallpaperValue("playbackRate", QString::number(playbackRate()), false);
        }

        QObject::connect(&m_pingAliveTimer, &QTimer::timeout, this, [this]() {
            qInfo() << "For " << m_pingAliveTimer.interval() << "ms no alive signal received. This means the Wallpaper is dead and likely crashed!";
            setState(ScreenPlayEnums::AppState::Closing);
            emit requestClose(m_appID);
        });
        m_pingAliveTimer.start(GlobalVariables::contentPingAliveIntervalMS);
    });

    // Check every X seconds if the wallpaper is still alive
    QObject::connect(m_connection.get(), &SDKConnection::pingAliveReceived, this, [this]() {
        if (state() == ScreenPlayEnums::AppState::Active) {
            m_pingAliveTimer.stop();
            m_pingAliveTimer.start(GlobalVariables::contentPingAliveIntervalMS);

            std::optional<bool> running = m_processManager.isRunning(m_processID);

            if (running.has_value()) {
                // qInfo() << "running:" << running.value();
            } else {
                qInfo() << "INVALID PID:" << m_processID;
            }
        }
    });
}

/*!
    \brief Replaces the current wallpaper with the given one.
*/
bool ScreenPlayWallpaper::replace(
    const WallpaperData wallpaperData,
    const bool checkWallpaperVisible)
{
    if (state() != ScreenPlayEnums::AppState::Active) {
        qWarning() << "Cannot replace inactive or closing wallpaper!";
        return false;
    }

    if (!m_connection) {
        qWarning() << "Cannot replace for unconnected wallpaper!";
        return false;
    }

    m_wallpaperData = wallpaperData;

    QJsonObject obj;
    obj.insert("command", "replace");
    obj.insert("type", QVariant::fromValue(m_wallpaperData.type()).toString());
    obj.insert("fillMode", QVariant::fromValue(m_wallpaperData.fillMode()).toString());
    obj.insert("volume", std::floor(m_wallpaperData.volume() * 100.0F) / 100.0f);
    obj.insert("absolutePath", m_wallpaperData.absolutePath());
    obj.insert("file", m_wallpaperData.file());
    obj.insert("checkWallpaperVisible", checkWallpaperVisible);

    const bool success = m_connection->sendMessage(QJsonDocument(obj).toJson(QJsonDocument::Compact));
    emit requestSave();
    return success;
}
}

#include "moc_screenplaywallpaper.cpp"
