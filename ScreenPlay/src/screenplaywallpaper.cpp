// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#include "ScreenPlay/screenplaywallpaper.h"
#include "ScreenPlayCore/util.h"

#include <QDebug>
#include <QDir>
#include <QFileInfoList>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(screenPlayWallpaper, "screenplay.wallpaper")

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

    const bool hasReapplySpacesValue = m_settings->desktopEnvironment() == Settings::DesktopEnvironment::OSX && m_settings->macReapplySpaces();
    if (hasReapplySpacesValue) {
        m_appArgumentsList.append({ "--reapplyspaces", "true" });
    }

    // Fixes issue 84 media key overlay in Qt apps
    if (m_wallpaperData.type() != ContentTypes::InstalledType::GodotWallpaper) {
        m_appArgumentsList.append(" --disable-features=HardwareMediaKeyHandling");

        // Add graphics API argument (not for Godot wallpapers)
        m_appArgumentsList.append({ "--graphicsapi", QString::number(static_cast<int>(m_settings->graphicsApi())) });
    }

    // Add anonymous telemetry setting
    m_appArgumentsList.append({ "--anonymoustelemetry", m_settings->anonymousTelemetry() ? "true" : "false" });

    if (m_wallpaperData.type() == ContentTypes::InstalledType::GodotWallpaper) {
        if (m_projectJson.contains("version")) {
            const quint64 version = m_projectJson.value("version").toInt();
            const QString packageFileName = QString("project-v%1.zip").arg(version);
            m_appArgumentsList.append({ "--projectPackageFile", packageFileName });
        }

        // Add FPS limit argument for Godot wallpapers
        QString fpsValue;
        switch (m_wallpaperData.godotFps()) {
        case Godot::Fps::Fps1:
            fpsValue = "1";
            break;
        case Godot::Fps::Fps6:
            fpsValue = "6";
            break;
        case Godot::Fps::Fps12:
            fpsValue = "12";
            break;
        case Godot::Fps::Fps24:
            fpsValue = "24";
            break;
        case Godot::Fps::Fps30:
            fpsValue = "30";
            break;
        case Godot::Fps::Fps60:
            fpsValue = "60";
            break;
        case Godot::Fps::Fps120:
            fpsValue = "120";
            break;
        case Godot::Fps::Fps144:
            fpsValue = "144";
            break;
        case Godot::Fps::Unlimited:
            fpsValue = "0"; // 0 typically means unlimited in Godot
            break;
        case Godot::Fps::Vsync:
            fpsValue = "vsync";
            break;
        }
        m_appArgumentsList.append({ "--fps", fpsValue });

        // Add 3D scale argument
        m_appArgumentsList.append({ "--3d-scale", QString::number(m_wallpaperData.godot3DScale()) });

        // Add 3D scale mode argument
        QString scaleModeValue;
        switch (m_wallpaperData.godot3DScaleMode()) {
        case Godot::ScaleMode3D::Bilinear:
            scaleModeValue = "0";
            break;
        case Godot::ScaleMode3D::FSR1_0:
            scaleModeValue = "1";
            break;
        case Godot::ScaleMode3D::FSR2_2:
            scaleModeValue = "2";
            break;
        }
        m_appArgumentsList.append({ "--3d-scale-mode", scaleModeValue });

        // Add rendering driver argument
        QString renderingDriver;
        switch (m_wallpaperData.godotRenderingDriver()) {
        case Godot::RenderingDriver::Vulkan:
            renderingDriver = "vulkan";
            break;
        case Godot::RenderingDriver::D3D12:
            renderingDriver = "d3d12";
            break;
        case Godot::RenderingDriver::OpenGL3:
            renderingDriver = "opengl3";
            break;
        case Godot::RenderingDriver::OpenGL3_Angle:
            renderingDriver = "opengl3_angle";
            break;
        }
        m_appArgumentsList.append({ "--rendering-driver", renderingDriver });
    }
}

bool ScreenPlayWallpaper::start()
{
    setState(ScreenPlayEnums::AppState::Starting);

    // Update dynamic arguments with current values before starting
    // This ensures restarted wallpapers receive the latest settings
    updateDynamicArguments();

    if (m_wallpaperData.type() == ContentTypes::InstalledType::GodotWallpaper) {
        m_process.setProgram(m_globalVariables->godotWallpaperExecutablePath().toString());
    } else {
        m_process.setProgram(m_globalVariables->wallpaperExecutablePath().toString());
    }

    m_process.setArguments(m_appArgumentsList);
    const bool success = m_process.startDetached(&m_processID);
    emit processIDChanged(m_processID);

    qCInfo(screenPlayWallpaper) << "Starting ScreenPlayWallpaper detached:" << (success ? "success" : "failed!") << m_process.program();
    qCInfo(screenPlayWallpaper) << m_appArgumentsList;

    if (!success) {
        setState(ScreenPlay::ScreenPlayEnums::AppState::StartingFailed);
    }
    return success;
}

QCoro::Task<Result> ScreenPlayWallpaper::close()
{
    setState(ScreenPlayEnums::AppState::Closing);
    qCInfo(screenPlayWallpaper) << "Close wallpaper with appID:" << m_appID;
    m_pingAliveTimer.stop();

    if (!m_connection) {
        qCInfo(screenPlayWallpaper) << "Cannot request quit, wallpaper never connected!";
        setState(ScreenPlayEnums::AppState::ClosingFailed);
        co_return Result { true, {}, "Quit wallpaper (it was never connected)" };
    }

    if (!m_connection->close()) {
        qCCritical(screenPlayWallpaper) << "Cannot close wallpaper!";
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
            qCInfo(screenPlayWallpaper) << "Process" << m_processID << "terminated successfully";
            setState(ScreenPlayEnums::AppState::ClosedGracefully);
            co_return Result { true, {}, "Quit wallpaper gracefully" };
        } else if (processState == ProcessManager::ProcessState::InvalidPID) {
            qCInfo(screenPlayWallpaper) << "Process" << m_processID << "has invalid PID - assuming successful termination";
            setState(ScreenPlayEnums::AppState::ClosedGracefully);
            co_return Result { true, {}, "Quit wallpaper gracefully (invalid PID)" };
        }
        // If Running, continue waiting
    }
    setState(ScreenPlayEnums::AppState::ClosingFailed);
    co_return Result { false, {}, QString("Wallpaper with appID '%1' failed to disconnect after %2 attempts").arg(m_appID).arg(maxRetries) };
}

void ScreenPlayWallpaper::setupSDKConnection()
{
    ScreenPlayExternalProcess::setupSDKConnection();

    if (!m_connection) {
        return;
    }

    qCInfo(screenPlayWallpaper) << "[4/4] SDKConnection (Wallpaper) saved!";
    setState(ScreenPlayEnums::AppState::Active);
    syncAllProperties();
}

bool ScreenPlayWallpaper::setWallpaperValue(const QString& key, const QVariant& value, const QString& category, const bool save)
{
    if (state() != ScreenPlayEnums::AppState::Active) {
        qCWarning(screenPlayWallpaper) << "Cannot set value for inactive or closing wallpaper!";
        return false;
    }

    if (!m_connection) {
        qCWarning(screenPlayWallpaper) << "Cannot set value for unconnected wallpaper!";
        return false;
    }

    // Convert enum integer values to string names for IPC
    QVariant valueToSend = value;
    if (key == "godotFps") {
        valueToSend = variantEnumToString<Godot::Fps>(value);
    } else if (key == "godot3DScaleMode") {
        valueToSend = variantEnumToString<Godot::ScaleMode3D>(value);
    } else if (key == "fillmode") {
        valueToSend = variantEnumToString<Video::FillMode>(value);
    }

    QJsonObject obj;
    obj.insert(key, QJsonValue::fromVariant(valueToSend));
    bool found = false;
    if (key == "volume") {
        setVolume(value.toFloat());
        m_wallpaperData.setVolume(volume());
        found = true;
    }

    if (key == "fillmode") {
        setFillMode(QVariantToEnum<Video::FillMode>(value, Video::FillMode::Cover));
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
    // qDebug() << "sending New values:" << (success ? "✅" : "❌") << absolutePath() << QJsonDocument(obj).toJson(QJsonDocument::Compact);
    return success;
}

void ScreenPlayWallpaper::updateVolume(const float volume)
{
    m_wallpaperData.setVolume(volume);
    emit volumeChanged(volume);
}

void ScreenPlayWallpaper::updateFillMode(const Video::FillMode fillMode)
{
    m_wallpaperData.setFillMode(fillMode);
    emit fillModeChanged(fillMode);
}

void ScreenPlayWallpaper::updateGodotFps(const Godot::Fps godotFps)
{
    m_wallpaperData.setGodotFps(godotFps);
    emit godotFpsChanged(godotFps);
}

void ScreenPlayWallpaper::updateGodot3DScaleMode(const Godot::ScaleMode3D godot3DScaleMode)
{
    m_wallpaperData.setGodot3DScaleMode(godot3DScaleMode);
    emit godot3DScaleModeChanged(godot3DScaleMode);
}

void ScreenPlayWallpaper::updateGodot3DScale(const float godot3DScale)
{
    m_wallpaperData.setGodot3DScale(godot3DScale);
    emit godot3DScaleChanged(godot3DScale);
}

void ScreenPlayWallpaper::updateProperty(const QString& category, const QString& key, const QVariant& value)
{
    auto properties = m_wallpaperData.properties();
    if (!properties.contains(category)) {
        properties.insert(category, QJsonObject());
    }
    QJsonObject categoryObj = properties[category].toObject();
    categoryObj.insert(key, QJsonValue::fromVariant(value));
    properties[category] = categoryObj;
    m_wallpaperData.setProperties(properties);
}

bool ScreenPlayWallpaper::replaceLive(const WallpaperData wallpaperData)
{
    if (state() != ScreenPlayEnums::AppState::Active) {
        qCWarning(screenPlayWallpaper) << "Cannot replace " << appID() << "at " << monitors() << " with invalid state of: " << state();
        return false;
    }

    if (!m_connection) {
        qCWarning(screenPlayWallpaper) << "Cannot replace for unconnected wallpaper!";
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
        qCWarning(screenPlayWallpaper) << "Cannot sendMessage:" << obj;
        return false;
    }
    return success;
}

void ScreenPlayWallpaper::syncAllProperties()
{
    if (!m_connection || state() != ScreenPlayEnums::AppState::Active) {
        return;
    }

    // Sync volume and fillmode first
    QJsonObject volumeObj;
    volumeObj.insert("volume", m_wallpaperData.volume());
    m_connection->sendMessage(QJsonDocument(volumeObj).toJson(QJsonDocument::Compact));

    QJsonObject fillModeObj;
    fillModeObj.insert("fillmode", QVariant::fromValue(m_wallpaperData.fillMode()).toString());
    m_connection->sendMessage(QJsonDocument(fillModeObj).toJson(QJsonDocument::Compact));

    // Sync custom properties
    const QJsonObject& properties = m_wallpaperData.properties();
    for (auto categoryIt = properties.constBegin(); categoryIt != properties.constEnd(); ++categoryIt) {
        const QString& category = categoryIt.key();
        const QJsonObject categoryObj = categoryIt.value().toObject();
        for (auto propIt = categoryObj.constBegin(); propIt != categoryObj.constEnd(); ++propIt) {
            const QString& key = propIt.key();
            QVariant value = propIt.value().toVariant();
            setWallpaperValue(key, value, category, false);
            qCInfo(screenPlayWallpaper) << "Sync:" << category << key << value;
        }
    }
}

bool ScreenPlayWallpaper::setWallpaperData(const WallpaperData wallpaperData)
{
    if (isConnected()) {
        qCCritical(screenPlayWallpaper) << "setWallpaperData was called on a live wallpaper. This will not work";
        return false;
    }
    m_wallpaperData = wallpaperData;
    return true;
}

/*!
    \brief Updates command-line arguments with current wallpaper settings.

    The command-line arguments (m_appArgumentsList) are built once in the
    constructor with initial values. When the user changes settings like
    volume or fillmode, m_wallpaperData is updated but m_appArgumentsList
    is not. This function refreshes the dynamic arguments before start()
    so that restarted wallpapers (e.g., after a crash) receive the latest
    user-configured values instead of the stale initial ones.
*/
void ScreenPlayWallpaper::updateDynamicArguments()
{
    auto updateArg = [this](const QString& key, const QString& value) {
        int i = m_appArgumentsList.indexOf(key);
        if (i >= 0 && i + 1 < m_appArgumentsList.size())
            m_appArgumentsList[i + 1] = value;
    };

    updateArg("--volume", QString::number(static_cast<double>(m_wallpaperData.volume())));
    updateArg("--fillmode", QVariant::fromValue(m_wallpaperData.fillMode()).toString());
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

void ScreenPlayWallpaper::setGodotFps(Godot::Fps godotFps)
{
    if (m_wallpaperData.godotFps() == godotFps)
        return;
    m_wallpaperData.setGodotFps(godotFps);
    emit godotFpsChanged(godotFps);
}

void ScreenPlayWallpaper::setGodot3DScaleMode(Godot::ScaleMode3D godot3DScaleMode)
{
    if (m_wallpaperData.godot3DScaleMode() == godot3DScaleMode)
        return;
    m_wallpaperData.setGodot3DScaleMode(godot3DScaleMode);
    emit godot3DScaleModeChanged(godot3DScaleMode);
}

void ScreenPlayWallpaper::setGodot3DScale(float godot3DScale)
{
    if (qFuzzyCompare(m_wallpaperData.godot3DScale(), godot3DScale))
        return;
    m_wallpaperData.setGodot3DScale(godot3DScale);
    emit godot3DScaleChanged(godot3DScale);
}

}

#include "moc_screenplaywallpaper.cpp"
