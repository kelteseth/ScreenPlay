// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#include "basewindow.h"

#include "ScreenPlayUtil/projectfile.h"
#include "ScreenPlayUtil/util.h"

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

BaseWindow::BaseWindow()
{
    QApplication::instance()->installEventFilter(this);

    qRegisterMetaType<ScreenPlay::InstalledType::InstalledType>();
    qmlRegisterUncreatableMetaObject(ScreenPlay::InstalledType::staticMetaObject,
        "ScreenPlay.Enums.InstalledType",
        1, 0,
        "InstalledType",
        "Error: only enums");

    qmlRegisterUncreatableMetaObject(ScreenPlay::VideoCodec::staticMetaObject,
        "ScreenPlay.Enums.VideoCodec",
        1, 0,
        "VideoCodec",
        "Error: only enums");

    qmlRegisterType<BaseWindow>("ScreenPlay.Wallpaper", 1, 0, "Wallpaper");

    setOSVersion(QSysInfo::productVersion());
}

BaseWindow::ExitCode BaseWindow::setup()
{

    if (projectPath() == "test") {
        setType(ScreenPlay::InstalledType::InstalledType::QMLWallpaper);
        setProjectSourceFileAbsolute({ "qrc:/qml/ScreenPlayWallpaper/qml/Test.qml" });
        setupLiveReloading();
        return BaseWindow::Success;
    }
    ScreenPlay::ProjectFile projectFile;
    projectFile.projectJsonFilePath = QFileInfo(projectPath() + "/project.json");
    if (!projectFile.init()) {
        qWarning() << "Invalid project at " << projectPath();
        return BaseWindow::ParsingError;
    }

    setProjectSourceFile(projectFile.file);

    if (m_type == ScreenPlay::InstalledType::InstalledType::WebsiteWallpaper) {
        setProjectSourceFileAbsolute(projectFile.url);
    } else {
        setProjectSourceFileAbsolute(QUrl::fromLocalFile(projectPath() + "/" + projectSourceFile()));
    }

    setupLiveReloading();

    // Debug mode means we start the ScreenPlayWallpaper
    // directly without an running ScreenPlay
    if (!debugMode()) {
        m_sdk = std::make_unique<ScreenPlaySDK>(appID(), QVariant::fromValue(type()).toString());
        connect(m_sdk.get(), &ScreenPlaySDK::sdkDisconnected, this, &BaseWindow::destroyThis);
        connect(m_sdk.get(), &ScreenPlaySDK::incommingMessage, this, &BaseWindow::messageReceived);
        connect(m_sdk.get(), &ScreenPlaySDK::replaceWallpaper, this, &BaseWindow::replaceWallpaper);
        sdk()->start();
    }

    return BaseWindow::Success;
}

/*!
 \brief messageReceived.
 */
void BaseWindow::messageReceived(QString key, QString value)
{
    if (key == "volume") {
        bool ok = false;
        float tmp = value.toFloat(&ok);
        if (ok) {
            setVolume(tmp);
        }
        return;
    }

    if (key == "playbackRate") {
        bool ok = false;
        float tmp = value.toFloat(&ok);
        if (ok) {
            setPlaybackRate(tmp);
        }
        return;
    }

    if (key == "loops") {
        bool tmp = QVariant(value).toBool();
        setLoops(tmp);
        return;
    }

    if (key == "isPlaying") {
        bool tmp = QVariant(value).toBool();
        setIsPlaying(tmp);
        return;
    }

    if (key == "muted") {
        bool tmp = QVariant(value).toBool();
        setMuted(tmp);
        return;
    }

    if (key == "fillmode") {
        // HTML5 Video uses - that c++ enums cannot
        if (QVariant(value).toString() == "Scale_Down") {
            setFillMode("Scale-Down");
        } else {
            setFillMode(QVariant(value).toString());
        }
        return;
    }

    if (key == "currentTime") {
        bool ok = false;
        float tmp = value.toFloat(&ok);
        if (ok) {
            setCurrentTime(tmp);
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
    const bool checkWallpaperVisible)
{
    const ScreenPlay::InstalledType::InstalledType oldType = this->type();
    setCheckWallpaperVisible(checkWallpaperVisible);
    setVolume(volume);
    setFillMode(fillMode);

    if (auto typeOpt = ScreenPlayUtil::getInstalledTypeFromString(type)) {
        setType(typeOpt.value());
    }

    if (type.contains("websiteWallpaper", Qt::CaseInsensitive)) {
        setProjectSourceFileAbsolute(file);
    } else {
        setProjectSourceFileAbsolute(QUrl::fromUserInput(absolutePath + "/" + file));
    }

    if (m_type == ScreenPlay::InstalledType::InstalledType::QMLWallpaper || m_type == ScreenPlay::InstalledType::InstalledType::HTMLWallpaper)
        emit reloadQML(oldType);

    if (m_type == ScreenPlay::InstalledType::InstalledType::VideoWallpaper)
        emit reloadVideo(oldType);

    if (m_type == ScreenPlay::InstalledType::InstalledType::GifWallpaper)
        emit reloadGIF(oldType);
}

/*!
 \brief Used for loading shader. Loading shader relative to the qml file will be available in Qt 6
 */
QString BaseWindow::loadFromFile(const QString& filename)
{
    QFile file(projectPath() + "/" + filename);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Could not loadFromFile: " << file.fileName();
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
    return QApplication::applicationDirPath();
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
    m_fileSystemWatcher.addPaths({ QUrl::fromUserInput(projectPath()).toLocalFile() });
}

ScreenPlay::VideoCodec::VideoCodec BaseWindow::videoCodec() const
{
    return m_videoCodec;
}

void BaseWindow::setVideoCodec(ScreenPlay::VideoCodec::VideoCodec newVideoCodec)
{
    if (m_videoCodec == newVideoCodec)
        return;
    m_videoCodec = newVideoCodec;
    emit videoCodecChanged();
}
