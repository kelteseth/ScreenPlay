// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#include "ScreenPlay/wizards.h"
#include "ScreenPlayUtil/util.h"

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
void Wizards::createQMLWidget(const QString& title,
    const QString& licenseName,
    const QString& licenseFile,
    const QString& createdBy,
    const QString& previewThumbnail,
    const QVector<QString>& tags)
{
    if (m_wizardFuture.isRunning()) {
        qWarning() << "Another wizard is already running! Abort.";
        return;
    }

    m_wizardFuture = QtConcurrent::run([=]() {
        std::optional<QString> folderName = createTemporaryFolder();

        if (!folderName.has_value()) {
            emit widgetCreationFinished(WizardResult::CreateProjectFolderError);
            return;
        }

        const QString workingPath = ScreenPlayUtil::toLocal(m_globalVariables->localStoragePath().toString() + "/" + folderName.value());

        QJsonObject obj;
        obj.insert("license", licenseName);
        obj.insert("title", title);
        obj.insert("tags", ScreenPlayUtil::fillArray(tags));
        obj.insert("createdBy", createdBy);
        obj.insert("type", "qmlWidget");
        obj.insert("file", "main.qml");

        if (!Util::writeFileFromQrc(":/qml/ScreenPlayApp/assets/wizards/" + licenseFile, workingPath + "/" + licenseFile)) {
            qWarning() << "Could not write " << licenseFile;
            emit widgetCreationFinished(WizardResult::WriteLicenseFileError);
            return;
        }

        if (!Util::writeFileFromQrc(":/qml/ScreenPlayApp/qml/Create/WizardsFiles/QMLWidgetMain.qml", workingPath + "/main.qml")) {
            qWarning() << "Could not write main.qml";
            emit widgetCreationFinished(WizardResult::WriteProjectFileError);
            return;
        }

        if (!previewThumbnail.isEmpty()) {
            QUrl previewThumbnailUrl { previewThumbnail };
            QFileInfo previewImageFile(previewThumbnailUrl.toLocalFile());
            obj.insert("previewThumbnail", previewImageFile.fileName());
            obj.insert("preview", previewImageFile.fileName());
            if (!QFile::copy(previewThumbnailUrl.toLocalFile(), workingPath + "/" + previewImageFile.fileName())) {
                qWarning() << "Could not copy" << previewThumbnailUrl.toLocalFile() << " to " << workingPath + "/" + previewImageFile.fileName();
                emit widgetCreationFinished(WizardResult::CopyError);
                return;
            }
        }

        if (!Util::writeSettings(obj, workingPath + "/project.json")) {
            emit widgetCreationFinished(WizardResult::WriteProjectFileError);
            return;
        }

        emit widgetCreationFinished(WizardResult::Ok, workingPath);
    });
}

/*!
  \brief Creates a new widget.
*/
void Wizards::createHTMLWidget(const QString& title,
    const QString& licenseName,
    const QString& licenseFile,
    const QString& createdBy,
    const QString& previewThumbnail,
    const QVector<QString>& tags)
{
    if (m_wizardFuture.isRunning()) {
        qWarning() << "Another wizard is already running! Abort.";
        return;
    }

    m_wizardFuture = QtConcurrent::run([=]() {
        std::optional<QString> folderName = createTemporaryFolder();

        if (!folderName.has_value()) {
            emit widgetCreationFinished(WizardResult::CreateProjectFolderError);
            return;
        }

        const QString workingPath = ScreenPlayUtil::toLocal(m_globalVariables->localStoragePath().toString() + "/" + folderName.value());

        QJsonObject obj;
        obj.insert("license", licenseName);
        obj.insert("createdBy", createdBy);
        obj.insert("title", title);
        obj.insert("tags", ScreenPlayUtil::fillArray(tags));
        obj.insert("type", "htmlWidget");
        obj.insert("file", "index.html");

        if (!Util::writeFileFromQrc(":/qml/ScreenPlayApp/assets/wizards/" + licenseFile, workingPath + "/" + licenseFile)) {
            qWarning() << "Could not write " << licenseFile;
            emit widgetCreationFinished(WizardResult::WriteLicenseFileError);
            return;
        }

        if (!Util::writeFileFromQrc(":/qml/ScreenPlayApp/qml/Create/WizardsFiles/HTMLWidgetMain.html", workingPath + "/index.html")) {
            qWarning() << "Could not write HTMLWidgetMain.html";
            emit widgetCreationFinished(WizardResult::WriteProjectFileError);
            return;
        }

        QUrl previewThumbnailUrl { previewThumbnail };
        QFileInfo previewImageFile(previewThumbnailUrl.toLocalFile());

        if (!previewThumbnail.isEmpty()) {
            obj.insert("previewThumbnail", previewImageFile.fileName());
            obj.insert("preview", previewImageFile.fileName());
            if (!QFile::copy(previewThumbnailUrl.toLocalFile(), workingPath + "/" + previewImageFile.fileName())) {
                qWarning() << "Could not copy" << previewThumbnailUrl.toLocalFile() << " to " << workingPath + "/" + previewImageFile.fileName();
                emit widgetCreationFinished(WizardResult::CopyError);
                return;
            }
        }

        if (!Util::writeSettings(obj, workingPath + "/project.json")) {
            emit widgetCreationFinished(WizardResult::WriteProjectFileError);
            return;
        }

        emit widgetCreationFinished(WizardResult::Ok, workingPath);
    });
}

/*!
  \brief Creates a HTML wallpaper.
*/
void Wizards::createHTMLWallpaper(
    const QString& title,
    const QString& licenseName,
    const QString& licenseFile,
    const QString& createdBy,
    const QString& previewThumbnail,
    const QVector<QString>& tags)
{
    if (m_wizardFuture.isRunning()) {
        qWarning() << "Another wizard is already running! Abort.";
        return;
    }

    m_wizardFuture = QtConcurrent::run([=]() {
        std::optional<QString> folderName = createTemporaryFolder();

        if (!folderName.has_value()) {
            emit widgetCreationFinished(WizardResult::CreateProjectFolderError);
            return;
        }

        const QString workingPath = ScreenPlayUtil::toLocal(m_globalVariables->localStoragePath().toString() + "/" + folderName.value());

        QJsonObject obj;
        obj.insert("license", licenseName);
        obj.insert("createdBy", createdBy);
        obj.insert("title", title);
        obj.insert("tags", ScreenPlayUtil::fillArray(tags));
        obj.insert("type", "htmlWallpaper");
        obj.insert("file", "index.html");

        if (!Util::writeFileFromQrc(":/qml/ScreenPlayApp/assets/wizards/" + licenseFile, workingPath + "/" + licenseFile)) {
            qWarning() << "Could not write " << licenseFile;
            emit widgetCreationFinished(WizardResult::WriteLicenseFileError);
            return;
        }

        if (!Util::writeFileFromQrc(":/qml/ScreenPlayApp/qml/Create/WizardsFiles/HTMLWallpaperMain.html", workingPath + "/index.html")) {
            qWarning() << "Could not write HTMLWallpaperMain.html";
            emit widgetCreationFinished(WizardResult::WriteProjectFileError);
            return;
        }

        if (!previewThumbnail.isEmpty()) {
            QUrl previewThumbnailUrl { previewThumbnail };
            QFileInfo previewImageFile(previewThumbnailUrl.toLocalFile());
            obj.insert("previewThumbnail", previewImageFile.fileName());
            obj.insert("preview", previewImageFile.fileName());
            if (!QFile::copy(previewThumbnailUrl.toLocalFile(), workingPath + "/" + previewImageFile.fileName())) {
                qWarning() << "Could not copy" << previewThumbnailUrl.toLocalFile() << " to " << workingPath + "/" + previewImageFile.fileName();
                emit widgetCreationFinished(WizardResult::CopyPreviewThumbnailError);
                return;
            }
        }

        if (!Util::writeSettings(obj, workingPath + "/project.json")) {
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
    const QString& licenseName,
    const QString& licenseFile,
    const QString& createdBy,
    const QString& previewThumbnail,
    const QVector<QString>& tags)
{
    if (m_wizardFuture.isRunning()) {
        qWarning() << "Another wizard is already running! Abort.";
        return;
    }

    m_wizardFuture = QtConcurrent::run([=]() {
        std::optional<QString> folderName = createTemporaryFolder();

        if (!folderName.has_value()) {
            emit widgetCreationFinished(WizardResult::CreateProjectFolderError);
            return;
        }

        const QString workingPath = ScreenPlayUtil::toLocal(m_globalVariables->localStoragePath().toString() + "/" + folderName.value());

        QJsonObject obj;
        obj.insert("license", licenseName);
        obj.insert("title", title);
        obj.insert("createdBy", createdBy);
        obj.insert("tags", ScreenPlayUtil::fillArray(tags));
        obj.insert("type", "qmlWallpaper");
        obj.insert("file", "main.qml");

        if (!previewThumbnail.isEmpty()) {
            if (!Util::copyPreviewThumbnail(obj, previewThumbnail, workingPath)) {
                emit widgetCreationFinished(WizardResult::CopyPreviewThumbnailError);
                return;
            }
        }

        if (!Util::writeFileFromQrc(":/qml/ScreenPlayApp/assets/wizards/" + licenseFile, workingPath + "/" + licenseFile)) {
            qWarning() << "Could not write " << licenseFile;
            emit widgetCreationFinished(WizardResult::WriteLicenseFileError);
            return;
        }

        const QString qmlproject = workingPath + "/" + title + ".qmlproject";
        if (!Util::writeFileFromQrc(":/qml/ScreenPlayApp/assets/wizards/" + QString("QmlProject.qmlproject"), qmlproject)) {
            qWarning() << "Could not write " << qmlproject;
            emit widgetCreationFinished(WizardResult::WriteLicenseFileError);
            return;
        }

        if (!Util::writeSettings(obj, workingPath + "/project.json")) {
            emit widgetCreationFinished(WizardResult::WriteProjectFileError);
            return;
        }

        if (!Util::writeFileFromQrc(":/qml/ScreenPlayApp/qml/Create/WizardsFiles/QMLWallpaperMain.qml", workingPath + "/main.qml")) {
            qWarning() << "Could not write main.qml";
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
    if (m_wizardFuture.isRunning()) {
        qWarning() << "Another wizard is already running! Abort.";
        return;
    }

    m_wizardFuture = QtConcurrent::run([=]() {
        std::optional<QString> folderName = createTemporaryFolder();

        if (!folderName.has_value()) {
            emit widgetCreationFinished(WizardResult::CreateProjectFolderError);
            return;
        }

        const QString workingPath = ScreenPlayUtil::toLocal(m_globalVariables->localStoragePath().toString() + "/" + folderName.value());
        const QString gifFileName = QFileInfo(ScreenPlayUtil::toLocal(file)).fileName();

        QJsonObject obj;
        obj.insert("license", licenseName);
        obj.insert("creator", creator);
        obj.insert("title", title);
        obj.insert("file", gifFileName);
        obj.insert("previewGIF", gifFileName);
        obj.insert("tags", ScreenPlayUtil::fillArray(tags));
        obj.insert("type", "gifWallpaper");

        if (!Util::writeFileFromQrc(":/qml/ScreenPlayApp/assets/wizards/" + licenseFile, workingPath + "/" + licenseFile)) {
            qWarning() << "Could not write " << licenseFile;
            emit widgetCreationFinished(WizardResult::WriteLicenseFileError);
            return;
        }

        if (!Util::writeSettings(obj, workingPath + "/project.json")) {
            emit widgetCreationFinished(WizardResult::WriteProjectFileError);
            return;
        }

        if (!QFile::copy(ScreenPlayUtil::toLocal(file), workingPath + "/" + gifFileName)) {
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
    const QUrl& url,
    const QVector<QString>& tags)
{
    if (m_wizardFuture.isRunning()) {
        qWarning() << "Another wizard is already running! Abort.";
        return;
    }

    m_wizardFuture = QtConcurrent::run([=]() {
        std::optional<QString> folderName = createTemporaryFolder();

        if (!folderName.has_value()) {
            emit widgetCreationFinished(WizardResult::CreateProjectFolderError);
            return;
        }

        const QString workingPath = ScreenPlayUtil::toLocal(m_globalVariables->localStoragePath().toString() + "/" + folderName.value());

        QJsonObject obj;
        obj.insert("title", title);
        obj.insert("tags", ScreenPlayUtil::fillArray(tags));
        obj.insert("type", "websiteWallpaper");
        obj.insert("url", url.toString());

        if (!previewThumbnail.isEmpty()) {
            if (!Util::copyPreviewThumbnail(obj, previewThumbnail, workingPath)) {
                emit widgetCreationFinished(WizardResult::CopyPreviewThumbnailError);
                return;
            }
        }

        if (!Util::writeSettings(obj, workingPath + "/project.json")) {
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
    const QUrl localStoragePathUrl { m_globalVariables->localStoragePath() };
    const QDir dir { localStoragePathUrl.toLocalFile() };

    // Create a temp dir so we can later alter it to the workshop id
    const QString currentTime = QDateTime::currentDateTime().toString("yyyy_MM_dd_hhmmss");
    const QString folderName = currentTime;

    if (!dir.mkdir(folderName)) {
        qWarning() << "Could create folder: " << folderName;
        return {};
    }

    return folderName;
}
}
