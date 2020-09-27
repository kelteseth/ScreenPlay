#include "basewindow.h"

BaseWindow::BaseWindow(QObject* parent)
    : QObject(parent)
{
}

BaseWindow::BaseWindow(QString projectFilePath, const QVector<int> activeScreensList, const bool checkWallpaperVisible)
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
        setFullContentPath("qrc:/test.qml");
        return;
    }

    QFile projectFile;
    QJsonDocument configJsonDocument;
    QJsonParseError parseError;



    projectFile.setFileName(projectFilePath + "/project.json");
    projectFile.open(QIODevice::ReadOnly | QIODevice::Text);
    QString projectConfig = projectFile.readAll();
    configJsonDocument = QJsonDocument::fromJson(projectConfig.toUtf8(), &parseError);

    /* project.json example:
    *{
    *    "title": "example title",
    *    "description": "",
    *    "file": "example.webm",
    *    "preview": "preview.png",
    *    "previewGIF": "preview.gif",
    *    "previewWEBM": "preview.webm",
    *    "type": "videoWallpaper"
    *}
    */

    if (!(parseError.error == QJsonParseError::NoError)) {
        qDebug() << projectConfig << "\n"
                 << parseError.errorString();
        qFatal("Settings Json Parse Error. Exiting now!");
    }

    QJsonObject projectObject = configJsonDocument.object();

    if (!projectObject.contains("file")) {
        qFatal("No file was specified inside the json object!");
    }

    if (!projectObject.contains("type")) {
        qFatal("No type was specified inside the json object!");
    }

    setBasePath(QUrl::fromUserInput(projectFilePath).toLocalFile());
    setFullContentPath("file:///" + projectFilePath + "/" + projectObject.value("file").toString());

    auto reloadQMLLambda = [this]() { emit reloadQML(type()); };
    QObject::connect(&m_fileSystemWatcher, &QFileSystemWatcher::directoryChanged, this, reloadQMLLambda);
    QObject::connect(&m_fileSystemWatcher, &QFileSystemWatcher::fileChanged, this, reloadQMLLambda);
    m_fileSystemWatcher.addPaths({ projectFilePath, projectFilePath + "/" + projectObject.value("file").toString() });

    QString type = projectObject.value("type").toString().toLower();

    setType(parseWallpaperType(type));
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
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return file.readAll();
    }
    qWarning() << "Could not loadFromFile: " << file.fileName();
    return "";
}

BaseWindow::WallpaperType BaseWindow::parseWallpaperType(const QString& type)
{

    if (type.contains("VideoWallpaper", Qt::CaseInsensitive)) {
        return (BaseWindow::WallpaperType::Video);
    }

    if (type.contains("QmlWallpaper", Qt::CaseInsensitive)) {
        return (BaseWindow::WallpaperType::Qml);
    }

    if (type.contains("HtmlWallpaper", Qt::CaseInsensitive)) {
        return (BaseWindow::WallpaperType::Html);
    }

    if (type.contains("GodotWallpaper", Qt::CaseInsensitive)) {
        return (BaseWindow::WallpaperType::Godot);
    }

    qWarning() << "Could not parse Wallpaper type from value: " << type;

    return BaseWindow::WallpaperType::Video;
}
