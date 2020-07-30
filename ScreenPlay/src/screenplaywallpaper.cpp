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
                auto obj = Util::openJsonFileToObject(absolutePath + "/project.json");
                if (!obj)
                    return;

                if (!obj->contains("properties"))
                    return;
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
    qInfo() << proArgs;

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
    QJsonObject properties;
    if (m_type == InstalledType::InstalledType::VideoWallpaper) {
        obj.insert("fillMode", QVariant::fromValue(m_fillMode).toString());
        obj.insert("isLooping", m_isLooping);
        obj.insert("volume", m_volume);
        obj.insert("playbackRate", m_playbackRate);
    } else {
        auto properties = m_projectSettingsListModel.getActiveSettingsJson();
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
        qDebug() << "WARNING EXIT CODE: " << exitCode;
}

/*!
    \brief Prints the exit code error.
*/
void ScreenPlayWallpaper::processError(QProcess::ProcessError error)
{
    qDebug() << "EX: " << error;
}

void ScreenPlayWallpaper::setWallpaperValue(const QString& key, const QVariant& value, const bool save)
{
    QJsonObject obj;
    obj.insert(key, value.toString());

    if (key == "volume") {
        setVolume(value.toFloat());
    }
    if (key == "playbackRate") {
        setPlaybackRate(value.toFloat());
    }
    if (key == "fillmode") {
        setFillMode(QStringToEnum<FillMode::FillMode>(value.toString(), FillMode::FillMode::Cover));
    }

    m_connection->sendMessage(QJsonDocument(obj).toJson(QJsonDocument::Compact));
    qInfo() << "save" << save;
    if (save)
        emit requestSave();
}

void ScreenPlayWallpaper::setSDKConnection(const std::shared_ptr<SDKConnection>& connection)
{
    m_connection = connection;
    qInfo() << "App Wallpaper connected!";
    qInfo() << playbackRate() << (playbackRate() != 1.0);
    QTimer::singleShot(500, [this]() {
        if (playbackRate() != 1.0) {
            setWallpaperValue("playbackRate", playbackRate(), false);
        }
    });
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
    obj.insert("volume", volume);
    obj.insert("absolutePath", absolutePath);
    obj.insert("file", file);
    obj.insert("checkWallpaperVisible", checkWallpaperVisible);

    m_connection->sendMessage(QJsonDocument(obj).toJson(QJsonDocument::Compact));
    emit requestSave();
}

}
