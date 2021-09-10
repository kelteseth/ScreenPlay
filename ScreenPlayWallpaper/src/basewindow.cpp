#include "basewindow.h"

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

BaseWindow::BaseWindow(QObject* parent)
    : QObject(parent)
{
}

BaseWindow::BaseWindow(
    const QVector<int> activeScreensList,
    const QString& projectFilePath,
    const QString& type,
    const bool checkWallpaperVisible,
    const QString& appID,
    const bool debugMode)
    : QObject(nullptr)
    , m_checkWallpaperVisible(checkWallpaperVisible)
    , m_activeScreensList(activeScreensList)
    , m_debugMode(debugMode)
    , m_sdk(std::make_unique<ScreenPlaySDK>(appID, type))
{
    QApplication::instance()->installEventFilter(this);

    qRegisterMetaType<ScreenPlay::InstalledType::InstalledType>();
    qmlRegisterUncreatableMetaObject(ScreenPlay::InstalledType::staticMetaObject,
        "ScreenPlay.Enums.InstalledType",
        1, 0,
        "InstalledType",
        "Error: only enums");

    qmlRegisterType<BaseWindow>("ScreenPlay.Wallpaper", 1, 0, "Wallpaper");

    if (!appID.contains("appID=")) {
        qInfo() << "Invalid appID: " << appID;
        qFatal("AppID does not contain appID=");
    }

    setAppID(appID);
    setProjectPath(projectFilePath);
    setOSVersion(QSysInfo::productVersion());

    if (projectFilePath == "test") {
        setType(ScreenPlay::InstalledType::InstalledType::QMLWallpaper);
        setProjectSourceFileAbsolute({ "qrc:/qml/Test.qml" });
        setupLiveReloading();
        return;
    }

    auto projectOpt = ScreenPlayUtil::openJsonFileToObject(projectFilePath + "/project.json");

    if (!projectOpt.has_value()) {
        QApplication::exit(-5);
    }
    const QJsonObject project = projectOpt.value();

    if (!project.contains("type")) {
        qFatal("No type was specified inside the json object!");
        QApplication::exit(-3);
    }

    if (!project.contains("file") && !project.contains("url")) {
        qFatal("No file was specified inside the json object!");
        QApplication::exit(-4);
    }

    if (auto typeOpt = ScreenPlayUtil::getInstalledTypeFromString(project.value("type").toString())) {
        setType(typeOpt.value());
    } else {
        qCritical() << "Cannot parse Wallpaper type from value" << project.value("type");
    }

    if (m_type == ScreenPlay::InstalledType::InstalledType::WebsiteWallpaper) {
        if (!project.contains("url")) {
            qFatal("No url was specified for a websiteWallpaper!");
            QApplication::exit(-5);
        }
        setProjectSourceFileAbsolute(project.value("url").toString());
    } else {
        setProjectSourceFile(project.value("file").toString());
        setProjectSourceFileAbsolute(QUrl::fromLocalFile(projectFilePath + "/" + projectSourceFile()));
    }

    setupLiveReloading();
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
        setProjectSourceFileAbsolute(QUrl::fromLocalFile(absolutePath + "/" + file));
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
    m_fileSystemWatcher.addPaths({ projectPath() });
}
