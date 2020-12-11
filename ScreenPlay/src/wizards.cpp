#include "wizards.h"

namespace ScreenPlay {
/*!
    \class ScreenPlay::Wizards
    \inmodule ScreenPlay
    \brief  Baseclass for all wizards. Mostly for copying and creating project files.
*/

/*!
  Constructor.
*/
Wizards::Wizards(const std::shared_ptr<GlobalVariables>& globalVariables, QObject* parent)
    : QObject(parent)
    , m_globalVariables(globalVariables)
{
    qRegisterMetaType<Wizards::WizardResult>("Wizards::WizardResult");
    qmlRegisterUncreatableType<Wizards>("ScreenPlay.Wizards", 1, 0, "WizardResult", "Error only for enums");
    qmlRegisterType<Wizards>("ScreenPlay.Wizards", 1, 0, "Wizards");
}

/*!
  \brief Creates a new widget.
*/
void Wizards::createQMLWidget(
    const QString& title,
    const QString& previewThumbnail,
    const QString& createdBy,
    const QString& license,
    const QVector<QString>& tags)
{
    QtConcurrent::run([=]() {
        QUrl localStoragePathUrl { m_globalVariables->localStoragePath() };
        QDir dir;
        dir.cd(localStoragePathUrl.toLocalFile());
        // Create a temp dir so we can later alter it to the workshop id
        auto folderName = QString("_tmp_" + QTime::currentTime().toString()).replace(":", "");
        QString workingPath = dir.path() + "/" + folderName;

        if (!dir.mkdir(folderName)) {
            qDebug() << "Could create folder: " << folderName;
            return;
        }

        QJsonObject obj;
        obj.insert("license", license);
        obj.insert("title", title);
        obj.insert("tags", Util::fillArray(tags));
        obj.insert("createdBy", createdBy);
        obj.insert("type", "qmlWidget");
        obj.insert("file", "main.qml");

        QFile fileMainQML(workingPath + "/main.qml");
        if (!fileMainQML.open(QIODevice::WriteOnly | QIODevice::Text)) {
            qDebug() << "Could not open /main.qml";
            return;
        }

        QTextStream outMainQML(&fileMainQML);
        outMainQML.setCodec("UTF-8");
        outMainQML << "import QtQuick 2.14 \n\n Item {\n id:root \n}";
        fileMainQML.close();

        QFile file(workingPath + "/project.json");
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            qDebug() << "Could not open /project.json";
            return;
        }

        QUrl previewThumbnailUrl { previewThumbnail };
        QFileInfo previewImageFile(previewThumbnailUrl.toLocalFile());

        if (!previewThumbnail.isEmpty()) {
            obj.insert("previewThumbnail", previewImageFile.fileName());
            obj.insert("preview", previewImageFile.fileName());
            if (!QFile::copy(previewThumbnailUrl.toLocalFile(), workingPath + "/" + previewImageFile.fileName())) {
                qDebug() << "Could not copy" << previewThumbnailUrl.toLocalFile() << " to " << workingPath + "/" + previewImageFile.fileName();
                emit widgetCreationFinished(WizardResult::CopyError);
                return;
            }
        }

        QTextStream out(&file);
        out.setCodec("UTF-8");
        QJsonDocument doc(obj);
        out << doc.toJson();
        file.close();

        emit widgetCreationFinished(WizardResult::Ok, workingPath);
    });
}

/*!
  \brief Creates a new widget.
*/
void Wizards::createHTMLWidget(
    const QString& title,
    const QString& previewThumbnail,
    const QString& createdBy,
    const QString& license,
    const QVector<QString>& tags)
{
    QtConcurrent::run([=]() {
        QUrl localStoragePathUrl { m_globalVariables->localStoragePath() };
        QDir dir;
        dir.cd(localStoragePathUrl.toLocalFile());
        // Create a temp dir so we can later alter it to the workshop id
        auto folderName = QString("_tmp_" + QTime::currentTime().toString()).replace(":", "");
        QString workingPath = dir.path() + "/" + folderName;

        if (!dir.mkdir(folderName)) {
            qDebug() << "Could create folder: " << folderName;
            return;
        }

        QJsonObject obj;
        obj.insert("license", license);
        obj.insert("title", title);
        obj.insert("tags", Util::fillArray(tags));
        obj.insert("createdBy", createdBy);
        obj.insert("type", "htmlWidget");
        obj.insert("file", "index.html");

        QFile fileMainHTML(workingPath + "/index.html");
        if (!fileMainHTML.open(QIODevice::WriteOnly | QIODevice::Text)) {
            qDebug() << "Could not open /index.html";
            return;
        }

        QTextStream outMainHTML(&fileMainHTML);
        outMainHTML.setCodec("UTF-8");
        outMainHTML << "<html>\n<head></head>\n<body></body>\n</html>";
        fileMainHTML.close();

        QFile file(workingPath + "/project.json");
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            qDebug() << "Could not open /project.json";
            return;
        }

        QUrl previewThumbnailUrl { previewThumbnail };
        QFileInfo previewImageFile(previewThumbnailUrl.toLocalFile());

        if (!previewThumbnail.isEmpty()) {
            obj.insert("previewThumbnail", previewImageFile.fileName());
            obj.insert("preview", previewImageFile.fileName());
            if (!QFile::copy(previewThumbnailUrl.toLocalFile(), workingPath + "/" + previewImageFile.fileName())) {
                qDebug() << "Could not copy" << previewThumbnailUrl.toLocalFile() << " to " << workingPath + "/" + previewImageFile.fileName();
                emit widgetCreationFinished(WizardResult::CopyError);
                return;
            }
        }

        QTextStream out(&file);
        out.setCodec("UTF-8");
        QJsonDocument doc(obj);
        out << doc.toJson();
        file.close();

        emit widgetCreationFinished(WizardResult::Ok, workingPath);
    });
}

/*!
  \brief Creates a HTML wallpaper.
*/
void Wizards::createHTMLWallpaper(
    const QString& title,
    const QString& previewThumbnail,
    const QString& license,
    const QVector<QString>& tags)
{
    QtConcurrent::run([=]() {
        QUrl localStoragePathUrl { m_globalVariables->localStoragePath() };
        QDir dir;
        dir.cd(localStoragePathUrl.toLocalFile());
        // Create a temp dir so we can later alter it to the workshop id
        auto folderName = QString("_tmp_" + QTime::currentTime().toString()).replace(":", "");
        QString workingPath = dir.path() + "/" + folderName;

        if (!dir.mkdir(folderName)) {
            qDebug() << "Could create folder: " << folderName;
            return;
        }

        QJsonObject obj;
        obj.insert("license", license);
        obj.insert("title", title);
        obj.insert("tags", Util::fillArray(tags));
        obj.insert("type", "htmlWallpaper");
        obj.insert("file", "index.html");

        QFile fileMainHTML(workingPath + "/index.html");
        if (!fileMainHTML.open(QIODevice::WriteOnly | QIODevice::Text)) {
            qDebug() << "Could not open /index.html";
            return;
        }

        QTextStream outMainHTML(&fileMainHTML);
        outMainHTML.setCodec("UTF-8");
        outMainHTML << "<html>\n<head></head>\n"
                       "<h1>This is an empty html Wallpaper!</h1>"
                       "<body></body>\n</html>";
        fileMainHTML.close();

        QUrl previewThumbnailUrl { previewThumbnail };
        QFileInfo previewImageFile(previewThumbnailUrl.toLocalFile());

        if (!previewThumbnail.isEmpty()) {
            obj.insert("previewThumbnail", previewImageFile.fileName());
            obj.insert("preview", previewImageFile.fileName());
            if (!QFile::copy(previewThumbnailUrl.toLocalFile(), workingPath + "/" + previewImageFile.fileName())) {
                qDebug() << "Could not copy" << previewThumbnailUrl.toLocalFile() << " to " << workingPath + "/" + previewImageFile.fileName();
                emit widgetCreationFinished(WizardResult::CopyPreviewThumbnailError);
                return;
            }
        }

        if (!Util::writeSettings(obj, dir.path() + "/project.json")) {
            emit widgetCreationFinished(WizardResult::WriteProjectFileError);
            return;
        }

        emit widgetCreationFinished(WizardResult::Ok, workingPath);
    });
}

/*!
    \brief .
*/
void Wizards::createQMLWallpaper(
    const QString& title,
    const QString& previewThumbnail,
    const QString& license,
    const QVector<QString>& tags)
{
    QtConcurrent::run([=]() {
        std::optional<QString> folderName = createTemporaryFolder();

        if (!folderName.has_value()) {
            emit widgetCreationFinished(WizardResult::CreateProjectFolderError);
            return;
        }

        const QString workingPath = Util::toLocal(m_globalVariables->localStoragePath().toString() + "/" + folderName.value());

        QJsonObject obj;
        obj.insert("license", license);
        obj.insert("title", title);
        obj.insert("tags", Util::fillArray(tags));
        obj.insert("type", "qmlWallpaper");
        obj.insert("file", "main.qml");

        if (!previewThumbnail.isEmpty()) {
            QUrl previewThumbnailUrl { previewThumbnail };
            if (!Util::copyPreviewThumbnail(obj, workingPath + "/" + previewThumbnailUrl.fileName(), workingPath)) {
                emit widgetCreationFinished(WizardResult::CopyPreviewThumbnailError);
                return;
            }
        }

        if (!Util::writeSettings(obj, workingPath + "/project.json")) {
            emit widgetCreationFinished(WizardResult::WriteProjectFileError);
            return;
        }

        if (!Util::writeFile("import QtQuick 2.14 \n\nItem {\n id:root \n}", workingPath + "/main.qml")) {
            emit widgetCreationFinished(WizardResult::WriteProjectFileError);
            return;
        }

        emit widgetCreationFinished(WizardResult::Ok, workingPath);
    });
}

void Wizards::createGifWallpaper(
    const QString& title,
    const QString& licenseName,
    const QString& licenseFile,
    const QString& creator,
    const QString& file,
    const QVector<QString>& tags)
{
    QtConcurrent::run([=]() {
        std::optional<QString> folderName = createTemporaryFolder();

        if (!folderName.has_value()) {
            emit widgetCreationFinished(WizardResult::CreateProjectFolderError);
            return;
        }

        const QString workingPath = Util::toLocal(m_globalVariables->localStoragePath().toString() + "/" + folderName.value());
        const QString gifFileName = QFileInfo(Util::toLocal(file)).fileName();

        QJsonObject obj;
        obj.insert("license", licenseName);
        obj.insert("creator", creator);
        obj.insert("title", title);
        obj.insert("file", gifFileName);
        obj.insert("previewGIF", gifFileName);
        obj.insert("tags", Util::fillArray(tags));
        obj.insert("type", "gifWallpaper");

        if (!Util::writeFileFromQrc(":/assets/wizards/" + licenseFile, workingPath + "/" + licenseFile)) {
            qWarning() << "Could not write " << licenseFile;
            return;
        }

        if (!Util::writeSettings(obj, workingPath + "/project.json")) {
            emit widgetCreationFinished(WizardResult::WriteProjectFileError);
            return;
        }

        if (!QFile::copy(Util::toLocal(file), workingPath + "/" + gifFileName)) {
            qWarning() << "Could not copy gif " << file << " to: " << workingPath + "/" + gifFileName;
            emit widgetCreationFinished(WizardResult::CopyFileError);
            return;
        }

        emit widgetCreationFinished(WizardResult::Ok, workingPath);
    });
}

/*!
    \brief .
*/
void Wizards::createWebsiteWallpaper(
    const QString& title,
    const QString& previewThumbnail,
    const QString& license,
    const QUrl& url,
    const QVector<QString>& tags)
{
    QtConcurrent::run([=]() {
        std::optional<QString> folderName = createTemporaryFolder();

        if (!folderName.has_value()) {
            emit widgetCreationFinished(WizardResult::CreateProjectFolderError);
            return;
        }

        const QString workingPath = Util::toLocal(m_globalVariables->localStoragePath().toString() + "/" + folderName.value());

        QJsonObject obj;
        obj.insert("license", license);
        obj.insert("title", title);
        obj.insert("tags", Util::fillArray(tags));
        obj.insert("type", "qmlWallpaper");
        obj.insert("file", "main.qml");

        if (!previewThumbnail.isEmpty()) {
            QUrl previewThumbnailUrl { previewThumbnail };
            if (!Util::copyPreviewThumbnail(obj, workingPath + "/" + previewThumbnailUrl.fileName(), workingPath)) {
                emit widgetCreationFinished(WizardResult::CopyPreviewThumbnailError);
                return;
            }
        }

        if (!Util::writeSettings(obj, workingPath + "/project.json")) {
            emit widgetCreationFinished(WizardResult::WriteProjectFileError);
            return;
        }

        if (!Util::writeFile("import QtQuick 2.14 \n\nItem {\n id:root \n}", workingPath + "/main.qml")) {
            emit widgetCreationFinished(WizardResult::WriteProjectFileError);
            return;
        }

        emit widgetCreationFinished(WizardResult::Ok, workingPath);
    });
}

/*!
    \brief .
*/
const std::optional<QString> Wizards::createTemporaryFolder() const
{
    QUrl localStoragePathUrl { m_globalVariables->localStoragePath() };
    QDir dir;
    dir.cd(localStoragePathUrl.toLocalFile());

    // Create a temp dir so we can later alter it to the workshop id
    const QString folderName = QString("_tmp_" + QTime::currentTime().toString()).replace(":", "");

    if (!dir.mkdir(folderName)) {
        qWarning() << "Could create folder: " << folderName;
        return {};
    }

    return folderName;
}
}
