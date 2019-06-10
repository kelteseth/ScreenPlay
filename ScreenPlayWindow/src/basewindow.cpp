#include "basewindow.h"

BaseWindow::BaseWindow(QObject* parent)
    : QObject(parent)
{

}

BaseWindow::BaseWindow(QString projectFilePath, QObject* parent)
    : QObject(parent)
{
    QApplication::instance()->installEventFilter(this);
    qRegisterMetaType<BaseWindow::WallpaperType>();
    qmlRegisterType<BaseWindow>("app.screenplay.wallpaper", 1, 0, "Wallpaper");

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
    *    "description": "",
    *    "file": "example.webm",
    *    "preview": "preview.png",
    *    "previewGIF": "preview.gif",
    *    "previewMP4": "preview.mp4",
    *    "title": "example title",
    *    "type": "video"
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

    setFullContentPath("file:///" + projectFilePath + "/" + projectObject.value("file").toString());

    if (projectObject.value("type") == "video") {
        setType(BaseWindow::WallpaperType::Video);
        return;
    }

    if (projectObject.value("type") == "scene") {
        setType(BaseWindow::WallpaperType::ThreeJSScene);
        return;
    }

    if (projectObject.value("type") == "qml") {
        setType(BaseWindow::WallpaperType::Qml);
        return;
    }

    if (projectObject.value("type") == "html") {
        setType(BaseWindow::WallpaperType::Html);
        return;
    }
}

