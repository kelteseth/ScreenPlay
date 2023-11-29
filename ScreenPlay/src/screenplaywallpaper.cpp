// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#include "ScreenPlay/screenplaywallpaper.h"

#include "util.h"

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
    const QVector<int>& screenNumber,
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
    const std::shared_ptr<Settings>& settings,
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
    , m_settings { settings }
{
    std::optional<QJsonObject> projectOpt = ScreenPlayUtil::openJsonFileToObject(absolutePath + "/project.json");
    if (projectOpt.has_value()) {
        m_projectJson = projectOpt.value();
    }
    QJsonObject projectSettingsListModelProperties;
    if (type == InstalledType::InstalledType::VideoWallpaper) {
        projectSettingsListModelProperties.insert("volume", m_volume);
        projectSettingsListModelProperties.insert("playbackRate", m_playbackRate);
    } else {
        if (properties.isEmpty()) {
            if (auto obj = ScreenPlayUtil::openJsonFileToObject(absolutePath + "/project.json")) {
                if (obj->contains("properties"))
                    projectSettingsListModelProperties = obj->value("properties").toObject();
            }
        } else {
            projectSettingsListModelProperties = properties;
        }
    }

    if (!projectSettingsListModelProperties.isEmpty())
        m_projectSettingsListModel.init(type, projectSettingsListModelProperties);

    QObject::connect(&m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &ScreenPlayWallpaper::processExit);
    QObject::connect(&m_process, &QProcess::errorOccurred, this, &ScreenPlayWallpaper::processError);

    QString tmpScreenNumber;
    if (m_screenNumber.length() > 1) {
        for (const int number : std::as_const(m_screenNumber)) {
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
        QString::number(m_settings->checkWallpaperVisible()),
    };

    // Fixes issue 84 media key overlay in Qt apps
    if (m_type != InstalledType::InstalledType::GodotWallpaper) {
        m_appArgumentsList.append(" --disable-features=HardwareMediaKeyHandling");
    }
    if (m_type == InstalledType::InstalledType::GodotWallpaper) {
        if (m_projectJson.contains("version")) {
            const quint64 version = m_projectJson.value("version").toInt();
            const QString packageFileName = QString("project-v%1.zip").arg(version);
            m_appArgumentsList.append(packageFileName);
        }
    }
}

bool ScreenPlayWallpaper::start()
{
    if (m_type == InstalledType::InstalledType::GodotWallpaper) {
        if (!exportGodotProject())
            return false;
    }

    m_process.setArguments(m_appArgumentsList);
    if (m_type == InstalledType::InstalledType::GodotWallpaper) {
        m_process.setProgram(m_globalVariables->godotWallpaperExecutablePath().toString());
    } else {
        m_process.setProgram(m_globalVariables->wallpaperExecutablePath().toString());
    }

    // We must start detatched otherwise we would instantly close the process
    // and would loose the animted fade-out and the background refresh needed
    // to display the original wallpaper.
    const bool success = m_process.startDetached();
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
    for (const int i : std::as_const(m_screenNumber)) {
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
    \brief Sends command quit to the wallpaper.
*/
void ScreenPlayWallpaper::close()
{
    // When the wallpaper never connected, this is invalid
    if (!m_connection) {
        qCritical() << "Cannot request quit, wallpaper never connected!";
        return;
    }

    if (!m_connection->close()) {
        qCritical() << "Cannot close wallpaper!";
        return;
    }
    m_isExiting = true;
}
/*!
    \brief Prints the exit code if != 0.
*/
void ScreenPlayWallpaper::processExit(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (exitCode != 0)
        qWarning() << "WARNING EXIT CODE: " << exitCode << exitStatus;
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
    if (m_isExiting)
        return false;

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
    qInfo() << "[4/4] SDKConnection (Wallpaper) saved!";
    setIsConnected(true);

    QObject::connect(m_connection.get(), &SDKConnection::disconnected, this, [this]() {
        setIsConnected(false);
        qInfo() << "Wallpaper:" << m_connection->appID() << "disconnected";
    });
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
bool ScreenPlayWallpaper::replace(
    const QString& absolutePath,
    const QString& previewImage,
    const QString& file,
    const float volume,
    const FillMode::FillMode fillMode,
    const InstalledType::InstalledType type,
    const bool checkWallpaperVisible)
{

    if (m_isExiting)
        return false;

    if (!m_connection) {
        qWarning() << "Cannot replace for unconnected wallpaper!";
        return false;
    }

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

    const bool success = m_connection->sendMessage(QJsonDocument(obj).toJson(QJsonDocument::Compact));
    emit requestSave();
    return success;
}

/*!
    \brief .
*/
bool ScreenPlayWallpaper::exportGodotProject()
{
    if (!m_projectJson.contains("version"))
        return false;

    const quint64 version = m_projectJson.value("version").toInt();
    const QString packageFileName = QString("project-v%1.zip").arg(version);
    QFileInfo godotPackageFile(m_absolutePath + "/" + packageFileName);
    // Skip reexport
    if (godotPackageFile.exists())
        return true;

    qInfo() << "No suitable version found for Godot package" << packageFileName << " at" << godotPackageFile.absoluteFilePath() << " exporting a new pck as zip.";

    // Prepare the Godot export command
    const QList<QString>
        godotCmd
        = { "--export-pack", "--headless", "Windows Desktop", packageFileName };

    // Create QProcess instance to run the command
    QProcess process;

    // Set the working directory to the given absolute path
    process.setWorkingDirectory(m_absolutePath);
    process.setProgram(m_globalVariables->godotEditorExecutablePath().toString());
    // Start the Godot export process
    process.setArguments(godotCmd);
    process.start();

    // Wait for the process to finish or timeout
    const int timeoutMilliseconds = 30000;
    if (!process.waitForFinished(timeoutMilliseconds)) {
        qCritical() << "Godot export process timed out or failed to start.";
        return false;
    }

    // Capture the standard output and error
    QString stdoutString = process.readAllStandardOutput();
    QString stderrString = process.readAllStandardError();

    // If you want to print the output to the console:
    if (!stdoutString.isEmpty())
        qDebug() << "Output:" << stdoutString;
    if (!stderrString.isEmpty())
        qDebug() << "Error:" << stderrString;

    // Check for errors
    if (process.exitStatus() != QProcess::NormalExit || process.exitCode() != 0) {
        qCritical() << "Failed to export Godot project. Error:" << process.errorString();
        return false;
    }

    // Check if the project.zip file was created
    QString zipPath = QDir(m_absolutePath).filePath(packageFileName);
    if (!QFile::exists(zipPath)) {
        qCritical() << "Expected export file (" << packageFileName << ") was not created.";
        return false;
    }

    // Optional: Verify if the .zip file is valid
    // (A complete verification would involve extracting the file and checking its contents,
    // but for simplicity, we're just checking its size here)
    QFileInfo zipInfo(zipPath);
    if (zipInfo.size() <= 0) {
        qCritical() << "The exported " << packageFileName << " file seems to be invalid.";
        return false;
    }

    return true;
}
}

#include "moc_screenplaywallpaper.cpp"
