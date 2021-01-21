#include "basewindow.h"

BaseWindow::BaseWindow(QObject* parent)
    : QObject(parent)
{
}

BaseWindow::BaseWindow(
    const QString& projectFilePath,
    const QVector<int> activeScreensList,
    const bool checkWallpaperVisible)
    : QObject(nullptr)
    , m_checkWallpaperVisible(checkWallpaperVisible)
    , m_activeScreensList(activeScreensList)
{
    QApplication::instance()->installEventFilter(this);

    qRegisterMetaType<BaseWindow::WallpaperType>();

    qmlRegisterType<BaseWindow>("ScreenPlay.Wallpaper", 1, 0, "Wallpaper");

    setOSVersion(QSysInfo::productVersion());

    if (projectFilePath == "test") {
        setType(BaseWindow::WallpaperType::Qml);
        setFullContentPath("qrc:/Test.qml");
        setupLiveReloading();
        return;
    }

    QFile projectFile { projectFilePath + "/project.json" };
    QJsonDocument configJsonDocument;
    QJsonParseError parseError;

    projectFile.open(QIODevice::ReadOnly | QIODevice::Text);
    const QString projectConfig = projectFile.readAll();
    projectFile.close();
    configJsonDocument = QJsonDocument::fromJson(projectConfig.toUtf8(), &parseError);

    /* project.json example:
    *  https://kelteseth.gitlab.io/ScreenPlayDocs/project/project/
    *{
    *    "title": "example title",
    *    "description": "",
    *    "file": "example.webm",
    *    "preview": "preview.png",
    *    "previewGIF": "preview.gif",
    *    "previewWEBM": "preview.webm",
    *    "type": "videoWallpaper"
    *    "url": "https://www.golem.de/" //websiteWallpaper only
    *}
    */

    if (!(parseError.error == QJsonParseError::NoError)) {
        qInfo() << projectFile.fileName()
                << projectConfig
                << parseError.errorString();
        qFatal("Settings Json Parse Error. Exiting now!");
    }

    const QJsonObject project = configJsonDocument.object();

    if (!project.contains("type")) {
        qFatal("No type was specified inside the json object!");
        QApplication::exit(-3);
    }

    if (!project.contains("file")) {
        qFatal("No file was specified inside the json object!");
        QApplication::exit(-4);
    }

    setType(parseWallpaperType(project.value("type").toString().toLower()));
    setBasePath(QUrl::fromUserInput(projectFilePath).toLocalFile());

    if (m_type == WallpaperType::Website) {
        if (!project.contains("url")) {
            qFatal("No url was specified for a websiteWallpaper!");
            QApplication::exit(-5);
        }
        setFullContentPath(project.value("url").toString());
    } else {
        setFullContentPath("file:///" + projectFilePath + "/" + project.value("file").toString());
    }

    setupLiveReloading();
}

void BaseWindow::messageReceived(QString key, QString value)
{
    if (key == "volume") {
        bool ok;
        float tmp = value.toFloat(&ok);
        if (ok) {
            setVolume(tmp);
        }
        return;
    }

    if (key == "playbackRate") {
        bool ok;
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
        bool ok;
        float tmp = value.toFloat(&ok);
        if (ok) {
            setCurrentTime(tmp);
        }
        return;
    }

    emit qmlSceneValueReceived(key, value);
}

void BaseWindow::replaceWallpaper(
    const QString absolutePath,
    const QString file,
    const float volume,
    const QString fillMode,
    const QString type,
    const bool checkWallpaperVisible)
{
    const WallpaperType oldType = this->type();
    setCheckWallpaperVisible(checkWallpaperVisible);
    setVolume(volume);
    setFillMode(fillMode);
    setType(parseWallpaperType(type));
    setFullContentPath("file:///" + absolutePath + "/" + file);
    qInfo() << file;

    if (m_type == WallpaperType::Qml || m_type == WallpaperType::Html)
        emit reloadQML(oldType);

    if (m_type == WallpaperType::Video)
        emit reloadVideo(oldType);
}

// Used for loading shader
// Loading shader relative to the qml file will be available in Qt 6
QString BaseWindow::loadFromFile(const QString& filename)
{
    QFile file;
    file.setFileName(basePath() + "/" + filename);
    qWarning() << "  loadFromFile: " << file.fileName() << file.readAll();
    if (file.open(QIODevice::ReadOnly)) {
        const QString content = file.readAll();
        file.close();
        return content;
    }
    file.close();
    qWarning() << "Could not loadFromFile: " << file.fileName();
    return "";
}

BaseWindow::WallpaperType BaseWindow::parseWallpaperType(const QString& type)
{

    if (type.contains("videoWallpaper", Qt::CaseInsensitive)) {
        return (BaseWindow::WallpaperType::Video);
    }

    if (type.contains("qmlWallpaper", Qt::CaseInsensitive)) {
        return (BaseWindow::WallpaperType::Qml);
    }

    if (type.contains("htmlWallpaper", Qt::CaseInsensitive)) {
        return (BaseWindow::WallpaperType::Html);
    }

    if (type.contains("godotWallpaper", Qt::CaseInsensitive)) {
        return (BaseWindow::WallpaperType::Godot);
    }

    if (type.contains("gifWallpaper", Qt::CaseInsensitive)) {
        return (BaseWindow::WallpaperType::Gif);
    }

    if (type.contains("websiteWallpaper", Qt::CaseInsensitive)) {
        return (BaseWindow::WallpaperType::Website);
    }

    qWarning() << "Could not parse Wallpaper type from value: " << type;

    return BaseWindow::WallpaperType::Video;
}

void BaseWindow::setupLiveReloading()
{
    auto reloadQMLLambda = [this]() { emit reloadQML(type()); };
    QObject::connect(&m_fileSystemWatcher, &QFileSystemWatcher::directoryChanged, this, reloadQMLLambda);
    QObject::connect(&m_fileSystemWatcher, &QFileSystemWatcher::fileChanged, this, reloadQMLLambda);
    const QFileInfo file { m_fullContentPath };
    m_fileSystemWatcher.addPaths({ file.path(), m_fullContentPath });
}
