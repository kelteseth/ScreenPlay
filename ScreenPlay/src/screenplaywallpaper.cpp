// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#include "ScreenPlay/screenplaywallpaper.h"
#include "ScreenPlayCore/util.h"

#include <QDebug>
#include <QDir>
#include <QFileInfoList>

namespace ScreenPlay {

ScreenPlayWallpaper::ScreenPlayWallpaper(
    const std::shared_ptr<GlobalVariables>& globalVariables,
    const QString& appID,
    const WallpaperData wallpaperData,
    const std::shared_ptr<Settings>& settings,
    QObject* parent)
    : ScreenPlayExternalProcess(appID, globalVariables, wallpaperData, parent)
    , m_settings(settings)
    , m_wallpaperData(wallpaperData)
{
    Util util;
    std::optional<QJsonObject> projectOpt = util.openJsonFileToObject(m_wallpaperData.absolutePath() + "/project.json");
    if (projectOpt.has_value()) {
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

    if (!projectSettingsListModelProperties.isEmpty()) {
        m_projectSettingsListModel = std::make_shared<ProjectSettingsListModel>();
        m_projectSettingsListModel->init(m_wallpaperData.type(), projectSettingsListModelProperties);
    }

    // Set up wallpaper-specific arguments
    QString tmpScreenNumber;
    if (m_wallpaperData.monitors().length() > 1) {
        for (const int number : m_wallpaperData.monitors()) {
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

    m_appArgumentsList = QStringList {
        "--screens", screens,
        "--projectpath", m_wallpaperData.absolutePath(),
        "--appID", m_appID,
        "--volume", QString::number(static_cast<double>(m_wallpaperData.volume())),
        "--fillmode", QVariant::fromValue(m_wallpaperData.fillMode()).toString(),
        "--type", QVariant::fromValue(m_wallpaperData.type()).toString(),
        "--check", QString::number(m_settings->checkWallpaperVisible()),
        "--mainapppid", QString::number(m_processManager.getCurrentPID())
    };

    // Fixes issue 84 media key overlay in Qt apps
    if (m_wallpaperData.type() != ContentTypes::InstalledType::GodotWallpaper) {
        m_appArgumentsList.append(" --disable-features=HardwareMediaKeyHandling");
        
        // Add graphics API argument (not for Godot wallpapers)
        m_appArgumentsList.append({ "--graphicsapi", QString::number(static_cast<int>(m_settings->graphicsApi())) });
    }
    if (m_wallpaperData.type() == ContentTypes::InstalledType::GodotWallpaper) {
        if (m_projectJson.contains("version")) {
            const quint64 version = m_projectJson.value("version").toInt();
            const QString packageFileName = QString("project-v%1.zip").arg(version);
            m_appArgumentsList.append({ "--projectPackageFile", packageFileName });
        }
        
        // Add FPS limit argument for Godot wallpapers
        QString fpsValue;
        switch (m_settings->godotFps()) {
        case Settings::GodotFps::Fps1:
            fpsValue = "1";
            break;
        case Settings::GodotFps::Fps24:
            fpsValue = "24";
            break;
        case Settings::GodotFps::Fps30:
            fpsValue = "30";
            break;
        case Settings::GodotFps::Fps60:
            fpsValue = "60";
            break;
        case Settings::GodotFps::Fps120:
            fpsValue = "120";
            break;
        case Settings::GodotFps::Fps144:
            fpsValue = "144";
            break;
        case Settings::GodotFps::Unlimited:
            fpsValue = "0"; // 0 typically means unlimited in Godot
            break;
        case Settings::GodotFps::Vsync:
            fpsValue = "vsync";
            break;
        }
        m_appArgumentsList.append({ "--fps", fpsValue });
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

    m_process.setArguments(m_appArgumentsList);
    const bool success = m_process.startDetached(&m_processID);
    emit processIDChanged(m_processID);

    qInfo() << "Starting ScreenPlayWallpaper detached:" << (success ? "success" : "failed!") << m_process.program();
    qInfo() << m_appArgumentsList;

    if (!success) {
        setState(ScreenPlay::ScreenPlayEnums::AppState::StartingFailed);
    }
    return success;
}

QCoro::Task<Result> ScreenPlayWallpaper::close()
{
    setState(ScreenPlayEnums::AppState::Closing);
    qInfo() << "Close wallpaper with appID:" << m_appID;
    m_pingAliveTimer.stop();

    if (!m_connection) {
        qInfo() << "Cannot request quit, wallpaper never connected!";
        setState(ScreenPlayEnums::AppState::ClosingFailed);
        co_return Result { true, {}, "Quit wallpaper (it was never connected)" };
    }

    if (!m_connection->close()) {
        qCritical() << "Cannot close wallpaper!";
        setState(ScreenPlayEnums::AppState::ClosingFailed);
        co_return Result { false, {}, "Failed to close connection to wallpaper" };
    }

    QTimer timer;
    timer.start(250);
    const int maxRetries = 30;
    for (int i = 1; i <= maxRetries; ++i) {
        co_await timer;
        ProcessManager::ProcessState processState = m_processManager.getProcessState(m_processID);

        if (processState == ProcessManager::ProcessState::NotRunning) {
            qInfo() << "Process" << m_processID << "terminated successfully";
            setState(ScreenPlayEnums::AppState::ClosedGracefully);
            co_return Result { true, {}, "Quit wallpaper gracefully" };
        } else if (processState == ProcessManager::ProcessState::InvalidPID) {
            qInfo() << "Process" << m_processID << "has invalid PID - assuming successful termination";
            setState(ScreenPlayEnums::AppState::ClosedGracefully);
            co_return Result { true, {}, "Quit wallpaper gracefully (invalid PID)" };
        }
        // If Running, continue waiting
    }
    co_return Result { false, {}, QString("Wallpaper with appID '%1' failed to disconnect after %2 attempts").arg(m_appID).arg(maxRetries) };
}

void ScreenPlayWallpaper::setupSDKConnection()
{
    ScreenPlayExternalProcess::setupSDKConnection();

    if (!m_connection) {
        return;
    }

    qInfo() << "[4/4] SDKConnection (Wallpaper) saved!";
    setState(ScreenPlayEnums::AppState::Active);
    syncAllProperties();
}

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

    if (!found && !category.isEmpty()) {
        auto properties = m_wallpaperData.properties();
        if (!properties.contains(category)) {
            properties.insert(category, QJsonObject());
        }
        QJsonObject categoryObj = properties[category].toObject();
        categoryObj.insert(key, QJsonValue::fromVariant(value));
        properties[category] = categoryObj;
        m_wallpaperData.setProperties(properties);
    }

    const bool success = m_connection->sendMessage(QJsonDocument(obj).toJson(QJsonDocument::Compact));
    qDebug() << "sending New values:" << (success ? "✅" : "❌") << absolutePath() << QJsonDocument(obj).toJson(QJsonDocument::Compact);
    return success;
}

bool ScreenPlayWallpaper::replaceLive(const WallpaperData wallpaperData)
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

void ScreenPlayWallpaper::syncAllProperties()
{
    if (!m_connection || state() != ScreenPlayEnums::AppState::Active) {
        return;
    }

    const QJsonObject& properties = m_wallpaperData.properties();
    for (auto categoryIt = properties.constBegin(); categoryIt != properties.constEnd(); ++categoryIt) {
        const QString& category = categoryIt.key();
        const QJsonObject categoryObj = categoryIt.value().toObject();
        for (auto propIt = categoryObj.constBegin(); propIt != categoryObj.constEnd(); ++propIt) {
            const QString& key = propIt.key();
            QVariant value = propIt.value().toVariant();
            setWallpaperValue(key, value, category, false);
            qInfo() << "Sync:" << category << key << value;
        }
    }
}

bool ScreenPlayWallpaper::setWallpaperData(const WallpaperData wallpaperData)
{
    if (isConnected()) {
        qCritical() << "setWallpaperData was called on a live wallpaper. This will not work";
        return false;
    }
    m_wallpaperData = wallpaperData;
    return true;
}

void ScreenPlayWallpaper::setMonitors(QVector<int> monitors)
{
    if (m_wallpaperData.monitors() == monitors)
        return;
    m_wallpaperData.setMonitors(monitors);
    emit monitorsChanged(monitors);
}

void ScreenPlayWallpaper::setVolume(float volume)
{
    if (volume < 0.0f || volume > 1.0f)
        return;
    if (qFuzzyCompare(m_wallpaperData.volume(), volume))
        return;
    m_wallpaperData.setVolume(volume);
    emit volumeChanged(volume);
}

void ScreenPlayWallpaper::setIsLooping(bool isLooping)
{
    if (m_wallpaperData.isLooping() == isLooping)
        return;
    m_wallpaperData.setIsLooping(isLooping);
    emit isLoopingChanged(isLooping);
}

void ScreenPlayWallpaper::setFile(QString file)
{
    if (m_wallpaperData.file() == file)
        return;
    m_wallpaperData.setFile(file);
    emit fileChanged(file);
}

void ScreenPlayWallpaper::setFillMode(Video::FillMode fillMode)
{
    if (m_wallpaperData.fillMode() == fillMode)
        return;
    m_wallpaperData.setFillMode(fillMode);
    emit fillModeChanged(fillMode);
}

}

#include "moc_screenplaywallpaper.cpp"
