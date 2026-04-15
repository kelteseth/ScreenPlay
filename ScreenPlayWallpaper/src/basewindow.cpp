// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#include "basewindow.h"
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(wallpaperBase, "screenplay.wallpaper.base")

/*!
    \module ScreenPlayWallpaper
    \title ScreenPlayWallpaper
    \brief Module for ScreenPlayWallpaper.
*/

/*!
    \class BaseWindow
    \inmodule ScreenPlayWallpaper
    \brief  .
*/

namespace ScreenPlay {
BaseWindow::BaseWindow()
    : m_currentState(std::make_unique<WallpaperState>(this))
    , m_targetState(std::make_unique<WallpaperState>(this))
{
    QGuiApplication::instance()->installEventFilter(this);
    setOSVersion(QSysInfo::productVersion());
}

WallpaperExit::Code BaseWindow::setup()
{

    if (projectPath() == "test") {
        setType(ScreenPlay::ContentTypes::InstalledType::QMLWallpaper);
        setProjectSourceFileAbsolute({ "qrc:/qt/qml/ScreenPlayWallpaper/qml/Test.qml" });
        setupLiveReloading();
        return WallpaperExit::Code::Ok;
    }
    ScreenPlay::ProjectFile projectFile;
    projectFile.projectJsonFilePath = QFileInfo(projectPath() + "/project.json");
    if (!projectFile.init()) {
        qCWarning(wallpaperBase) << "Invalid project at " << projectPath();
        return WallpaperExit::Code::Invalid_Setup_ProjectParsingError;
    }
    setType(projectFile.type);
    setProjectSourceFile(projectFile.file);

    // We do not yet have implemented continue playing the audio.mp3 yet
    // so disable the checkWallpaperVisible for now
    if (m_currentState->checkWallpaperVisible()) {
        if (projectFile.containsAudio) {
            qCInfo(wallpaperBase) << "Disable wallpaper visible check, because it contains audio.";
            m_currentState->setCheckWallpaperVisible(false);
        }
    }

    if (m_type == ScreenPlay::ContentTypes::InstalledType::WebsiteWallpaper) {
        setProjectSourceFileAbsolute(projectFile.url);
    } else {
        setProjectSourceFileAbsolute(QUrl::fromLocalFile(projectPath() + "/" + projectSourceFile()));
    }

    setupLiveReloading();
    return WallpaperExit::Code::Ok;
}

/*!
 \brief .
 */
void BaseWindow::connectToMainApp()
{
    // Debug mode means we start the ScreenPlayWallpaper
    // directly without an running ScreenPlay
    if (!debugMode()) {
        m_sdk = std::make_unique<ScreenPlaySDK>(appID(), QVariant::fromValue(type()).toString());
        m_sdk->setMainAppPID(m_mainAppPID);
        connect(m_sdk.get(), &ScreenPlaySDK::incomingMessage, this, &BaseWindow::messageReceived);
        connect(m_sdk.get(), &ScreenPlaySDK::replaceWallpaper, this, &BaseWindow::replaceWallpaper);
        connect(m_sdk.get(), &ScreenPlaySDK::sdkDisconnected, this, &BaseWindow::destroyThis);
        sdk()->start();
    }
}

/*!
 \brief messageReceived.
 */
void BaseWindow::messageReceived(const QString& key, const QString& value)
{
    if (key == "volume") {
        bool ok = false;
        float tmp = value.toFloat(&ok);
        if (ok) {
            m_currentState->setVolume(tmp);
        }
        return;
    }

    if (key == "loops") {
        bool tmp = QVariant(value).toBool();
        m_currentState->setLoops(tmp);
        return;
    }

    if (key == "isPlaying") {
        bool tmp = QVariant(value).toBool();
        m_currentState->setIsPlaying(tmp);
        return;
    }

    if (key == "muted") {
        bool tmp = QVariant(value).toBool();
        m_currentState->setMuted(tmp);
        return;
    }

    if (key == "fillmode") {
        // HTML5 Video uses - that c++ enums cannot
        if (QVariant(value).toString() == "Scale_Down") {
            m_currentState->setFillMode("Scale-Down");
        } else {
            m_currentState->setFillMode(QVariant(value).toString());
        }
        return;
    }

    emit qmlSceneValueReceived(key, value);
}

/*!
 \brief replaceWallpaper.
 */
void BaseWindow::replaceWallpaper(
    const QString absolutePath,
    const QString file,
    const float volume,
    const QString fillMode,
    const QString type,
    const bool checkWallpaperVisible,
    const QJsonObject wallpaperProperties)
{
    const ScreenPlay::ContentTypes::InstalledType oldType = this->type();

    // Set target state for crossfade transition - don't apply immediately
    // QML will use this for the incoming video and call applyTargetSettings() when done
    m_targetState->setVolume(volume);
    m_targetState->setFillMode(fillMode);
    m_targetState->setCheckWallpaperVisible(checkWallpaperVisible);

    if (auto typeOpt = ScreenPlay::Util().getInstalledTypeFromString(type)) {
        setType(typeOpt.value());
    }

    if (type.contains("websiteWallpaper", Qt::CaseInsensitive)) {
        setProjectSourceFileAbsolute(file);
    } else {
        setProjectSourceFileAbsolute(QUrl::fromUserInput(absolutePath + "/" + file));
    }

    if (m_type == ScreenPlay::ContentTypes::InstalledType::QMLWallpaper || m_type == ScreenPlay::ContentTypes::InstalledType::HTMLWallpaper)
        emit reloadQML(oldType);

    if (m_type == ScreenPlay::ContentTypes::InstalledType::VideoWallpaper)
        emit reloadVideo(oldType);

    if (m_type == ScreenPlay::ContentTypes::InstalledType::GifWallpaper)
        emit reloadGIF(oldType);

    if (!wallpaperProperties.isEmpty()) {
        for (auto it = wallpaperProperties.constBegin(); it != wallpaperProperties.constEnd(); ++it) {
            qCDebug(wallpaperBase) << "New values: " << it.key() << it.value();
            // Convert to QVariant first for proper type handling
            QVariant value = it.value().toVariant();
            emit qmlSceneValueReceived(it.key(), value.toString());
        }
    }
}

/*!
 \brief Apply target settings after crossfade transition completes.
 Called from QML when the transition animation finishes.
 */
void BaseWindow::applyTargetSettings()
{
    // Copy all values from target state to current state
    m_currentState->copyFrom(m_targetState.get());
}

/*!
 \brief QML Convenience function for global fade in
 */
void BaseWindow::requestFadeIn()
{
    emit fadeIn();
}

/*!
 \brief Used for loading shader. Loading shader relative to the qml file will be available in Qt 6
 */
QString BaseWindow::loadFromFile(const QString& filename)
{
    QFile file(projectPath() + "/" + filename);
    if (!file.open(QIODevice::ReadOnly)) {
        qCWarning(wallpaperBase) << "Could not loadFromFile: " << file.fileName();
        file.close();
        return "";
    }
    const QString content = file.readAll();
    file.close();
    return content;
}

/*!
 \brief This public slot is for QML usage.
 */
QString BaseWindow::getApplicationPath()
{
    return QGuiApplication::applicationDirPath();
}

/*!
 \brief This public slot is for QML usage. We limit the change event updates
        to every 50ms, because the filesystem can be very trigger happy
        with multiple change events per second.
 */
void BaseWindow::setupLiveReloading()
{
    auto reloadQMLLambda = [this]() {
        m_liveReloadLimiter.stop();
        emit reloadQML(type());
    };
    auto timeoutLambda = [this]() {
        m_liveReloadLimiter.start(50);
    };

    QObject::connect(&m_fileSystemWatcher, &QFileSystemWatcher::directoryChanged, this, timeoutLambda);
    QObject::connect(&m_fileSystemWatcher, &QFileSystemWatcher::fileChanged, this, timeoutLambda);
    QObject::connect(&m_liveReloadLimiter, &QTimer::timeout, this, reloadQMLLambda);
    m_fileSystemWatcher.addPath(projectPath());
    QDirIterator projectFilesIter(projectPath(), { "*.qml", "*.html", "*.css", "*.js" }, QDir::Files | QDir::NoSymLinks, QDirIterator::Subdirectories);
    m_fileSystemWatcher.addPath({ projectPath() });
    while (projectFilesIter.hasNext()) {
        m_fileSystemWatcher.addPath(projectFilesIter.next());
    }
}

ScreenPlay::Video::VideoCodec BaseWindow::videoCodec() const
{
    return m_videoCodec;
}

void BaseWindow::setVideoCodec(ScreenPlay::Video::VideoCodec newVideoCodec)
{
    if (m_videoCodec == newVideoCodec)
        return;
    m_videoCodec = newVideoCodec;
    emit videoCodecChanged(newVideoCodec);
}

qint64 BaseWindow::mainAppPID() const
{
    return m_mainAppPID;
}

void BaseWindow::setMainAppPID(qint64 mainAppPID)
{
    if (m_mainAppPID == mainAppPID)
        return;
    m_mainAppPID = mainAppPID;
    emit mainAppPIDChanged(m_mainAppPID);
}
}

#include "moc_basewindow.cpp"
