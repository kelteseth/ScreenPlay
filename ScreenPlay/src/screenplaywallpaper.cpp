// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#include "ScreenPlay/screenplaywallpaper.h"
#include "ScreenPlayCore/util.h"

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
    , m_settings { settings }
    , m_appID { appID }
    , m_wallpaperData { wallpaperData }
{
    Util util;
    std::optional<QJsonObject> projectOpt = util.openJsonFileToObject(m_wallpaperData.absolutePath()
        + "/project.json");
    if (projectOpt.has_value()) { // TODO handle no value
        m_projectJson = projectOpt.value();
    }
    QJsonObject projectSettingsListModelProperties;
    if (m_wallpaperData.type() == ContentTypes::InstalledType::VideoWallpaper) {
        projectSettingsListModelProperties.insert("volume", m_wallpaperData.volume());
    } else {
        // Load base settings from project.json
        if (auto obj = util.openJsonFileToObject(m_wallpaperData.absolutePath() + "/project.json")) {
            if (obj->contains("properties")) {
                projectSettingsListModelProperties = obj->value("properties").toObject();

                // Apply any user-modified properties over the base settings
                const QJsonObject& userProperties = m_wallpaperData.properties();
                for (auto it = userProperties.begin(); it != userProperties.end(); ++it) {
                    const QString& category = it.key();
                    const QJsonObject& categoryProperties = it.value().toObject();

                    // If this category exists in project settings, merge the properties
                    if (projectSettingsListModelProperties.contains(category)) {
                        QJsonObject baseCategory = projectSettingsListModelProperties[category].toObject();

                        // Update each property in the category
                        for (auto propIt = categoryProperties.begin(); propIt != categoryProperties.end(); ++propIt) {
                            const QString& property = propIt.key();
                            if (baseCategory.contains(property)) {
                                QJsonObject propObj = baseCategory[property].toObject();
                                propObj["value"] = propIt.value();
                                baseCategory[property] = propObj;
                            }
                        }

                        projectSettingsListModelProperties[category] = baseCategory;
                    }
                }
            }
        }
    }
    qDebug().noquote() << Util().toString(projectSettingsListModelProperties);

    if (!projectSettingsListModelProperties.isEmpty()) {
        m_projectSettingsListModel = std::make_shared<ProjectSettingsListModel>();
        m_projectSettingsListModel->init(m_wallpaperData.type(), projectSettingsListModelProperties);
    }

    QObject::connect(&m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &ScreenPlayWallpaper::processExit);
    QObject::connect(&m_process, &QProcess::errorOccurred, this, [this]() {
        setState(ScreenPlay::ScreenPlayEnums::AppState::ErrorOccouredWhileActive);
    });

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

    m_appArgumentsList = QStringList { "--screens",
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
        QString::number(m_processManager.getCurrentPID()) };

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
    setState(ScreenPlayEnums::AppState::Starting);
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
        setState(ScreenPlay::ScreenPlayEnums::AppState::StartingFailed);
    }
    return success;
}

/*!
    \brief Sends command quit to the wallpaper.
*/
QCoro::Task<Result> ScreenPlayWallpaper::close()
{
    setState(ScreenPlayEnums::AppState::Closing);
    qInfo() << "Close wallpaper with appID:" << m_appID;
    m_pingAliveTimer.stop(); // Stop the timer when closing
    if (!m_connection) {
        qInfo() << "Cannot request quit, wallpaper never connected!";
        setState(ScreenPlayEnums::AppState::Inactive);
        co_return Result { true, {}, "Quit wallpaper (it was never connected)" };
    }
    if (!m_connection->close()) {
        qCritical() << "Cannot close wallpaper!";
        setState(ScreenPlayEnums::AppState::Inactive);
        co_return Result { false, {}, "Failed to close connection to wallpaper" };
    }
    QTimer timer;
    timer.start(250);
    const int maxRetries = 30;
    for (int i = 1; i <= maxRetries; ++i) {
        // Wait for the timer to tick
        co_await timer;
        if (!isConnected()) {
            setState(ScreenPlayEnums::AppState::Inactive);
            co_return Result { true };
        }
    }
    co_return Result { false, {}, QString("Wallpaper with appID '%1' failed to disconnect after %2 attempts").arg(m_appID).arg(maxRetries) };
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
    \brief Sets a wallpaper value. We directly set the property if it is either volume
        or fillMode. Otherwise it is a simple key, value json pair.

*/

bool ScreenPlayWallpaper::setWallpaperValue(const QString& key, const QVariant& value, const QString& category, const bool save)
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
    obj.insert(key, QJsonValue::fromVariant(value));
    bool found = false;
    if (key == "volume") {
        setVolume(value.toFloat());
        m_wallpaperData.setVolume(volume());
        found = true;
    }

    if (key == "fillmode") {
        setFillMode(QStringToEnum<Video::FillMode>(value.toString(), Video::FillMode::Cover));
        m_wallpaperData.setFillMode(fillMode());
        found = true;
    }
    // User defined properties
    if (!found && !category.isEmpty()) {
        auto properties = m_wallpaperData.properties();

        // Check if the category already exists
        if (!properties.contains(category)) {
            // Create a new category object if it doesn't exist
            properties.insert(category, QJsonObject());
        }

        // Get the category object
        QJsonObject categoryObj = properties[category].toObject();

        // Add or update the property in the category
        categoryObj.insert(key, QJsonValue::fromVariant(value));

        // Update the category in the properties
        properties[category] = categoryObj;

        // Set the updated properties
        m_wallpaperData.setProperties(properties);
    }
    const bool success = m_connection->sendMessage(QJsonDocument(obj).toJson(QJsonDocument::Compact));
    qDebug() << "sending New values:" << (success ? "✅" : "❌") << absolutePath() << QJsonDocument(obj).toJson(QJsonDocument::Compact);
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

void ScreenPlayWallpaper::syncAllProperties()
{
    // Only sync if we're connected and active
    if (!m_connection || state() != ScreenPlayEnums::AppState::Active) {
        return;
    }

    // Then sync all category-based properties
    const QJsonObject& properties = m_wallpaperData.properties();

    // Iterate through each category
    for (auto categoryIt = properties.constBegin(); categoryIt != properties.constEnd(); ++categoryIt) {
        const QString& category = categoryIt.key();
        const QJsonObject categoryObj = categoryIt.value().toObject();

        // Iterate through properties in this category
        for (auto propIt = categoryObj.constBegin(); propIt != categoryObj.constEnd(); ++propIt) {
            const QString& key = propIt.key();
            QVariant value = propIt.value().toVariant();

            // Send each property with its category
            setWallpaperValue(key, value, category, false);
            qInfo() << "Sync:" << category << key << value;
        }
    }
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
    syncAllProperties();

    QObject::connect(m_connection.get(), &SDKConnection::disconnected, this, [this]() {
        setIsConnected(false);
        setState(ScreenPlayEnums::AppState::Inactive);
        m_pingAliveTimer.stop(); // Stop the timer when disconnected
        qInfo() << "Wallpaper:" << m_connection->appID() << "disconnected";
    });

    QTimer::singleShot(4000, this, [this]() {
        QObject::connect(&m_pingAliveTimer, &QTimer::timeout, this, [this]() {
            // qInfo() << "For " << m_pingAliveTimer.interval() << "ms no alive signal received. This means the Wallpaper is dead and likely crashed!";
            // setState(ScreenPlayEnums::AppState::Closing);
            std::optional<bool> running = m_processManager.isRunning(m_processID);

            if (running.has_value()) {
                // qInfo() << "running:" << running.value();
            } else {
                qInfo() << "INVALID PID:" << m_processID;
                emit requestClose(m_appID);
            }
        });
        m_pingAliveTimer.start(GlobalVariables::contentPingAliveIntervalMS);
    });

    // // Check every X seconds if the wallpaper is still alive
    // QObject::connect(m_connection.get(), &SDKConnection::pingAliveReceived, this, [this]() {
    //     if (state() == ScreenPlayEnums::AppState::Active) {
    //         m_pingAliveTimer.stop();
    //         m_pingAliveTimer.start(GlobalVariables::contentPingAliveIntervalMS);

    //         std::optional<bool> running = m_processManager.isRunning(m_processID);

    //         if (running.has_value()) {
    //             // qInfo() << "running:" << running.value();
    //         } else {
    //             qInfo() << "INVALID PID:" << m_processID;
    //         }
    //     }
    // });
}

/*!
    \brief Replaces the current wallpaper with the given one.
*/
bool ScreenPlayWallpaper::replaceLive(
    const WallpaperData wallpaperData)
{
    if (state() != ScreenPlayEnums::AppState::Active) {
        qWarning() << "Cannot replace " << appID() << "at " << monitors() << " with invalid state of: " << state();
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
    obj.insert("checkWallpaperVisible", false);
    obj.insert("properties", Util().flattenProperties(wallpaperData.properties()));

    const bool success = m_connection->sendMessage(QJsonDocument(obj).toJson(QJsonDocument::Compact));
    if (!success) {
        qWarning() << "Cannot sendMessage:" << obj;
        return false;
    }
    return success;
}
}

#include "moc_screenplaywallpaper.cpp"
