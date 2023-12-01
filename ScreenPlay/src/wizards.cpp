// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#include "ScreenPlay/wizards.h"
#include "ScreenPlay/CMakeVariables.h"
#include "ScreenPlayUtil/util.h"
#include <QFont>
#include <QLinearGradient>
#include <QPainter>
#include <QTextOption>

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
        obj.insert("type", QVariant::fromValue(ContentTypes::InstalledType::QMLWidget).toString());
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
            if (!Util::copyPreviewThumbnail(obj, previewThumbnail, workingPath)) {
                emit widgetCreationFinished(WizardResult::CopyPreviewThumbnailError);
                return;
            }
        } else {
            obj.insert("preview", "preview.png");
            createPreviewImage(title, workingPath);
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
        obj.insert("type", QVariant::fromValue(ContentTypes::InstalledType::HTMLWidget).toString());
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
            if (!Util::copyPreviewThumbnail(obj, previewThumbnail, workingPath)) {
                emit widgetCreationFinished(WizardResult::CopyPreviewThumbnailError);
                return;
            }
        } else {
            obj.insert("preview", "preview.png");
            createPreviewImage(title, workingPath);
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
        obj.insert("type", QVariant::fromValue(ContentTypes::InstalledType::HTMLWallpaper).toString());
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
            if (!Util::copyPreviewThumbnail(obj, previewThumbnail, workingPath)) {
                emit widgetCreationFinished(WizardResult::CopyPreviewThumbnailError);
                return;
            }
        } else {
            obj.insert("preview", "preview.png");
            createPreviewImage(title, workingPath);
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
        obj.insert("type", QVariant::fromValue(ContentTypes::InstalledType::QMLWallpaper).toString());
        obj.insert("file", "main.qml");

        if (!previewThumbnail.isEmpty()) {
            if (!Util::copyPreviewThumbnail(obj, previewThumbnail, workingPath)) {
                emit widgetCreationFinished(WizardResult::CopyPreviewThumbnailError);
                return;
            }
        } else {
            obj.insert("preview", "preview.png");
            createPreviewImage(title, workingPath);
        }

        if (!Util::writeFileFromQrc(":/qml/ScreenPlayApp/assets/wizards/" + licenseFile, workingPath + "/" + licenseFile)) {
            qWarning() << "Could not write " << licenseFile;
            emit widgetCreationFinished(WizardResult::WriteLicenseFileError);
            return;
        }

        const QString qmlproject = workingPath + "/" + title + ".qmlproject";
        if (!Util::writeFileFromQrc(":/qml/ScreenPlayApp/qml/Create/WizardsFiles/" + QString("QmlProject.qmlproject"), qmlproject)) {
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

/*!
    \brief .
*/
void Wizards::createGodotWallpaper(
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
        // Every version change will trigger an reexport
        obj.insert("version", 1);
        // Something like v4.2-beta3 or v5.0.1-stable
        QString godotVersionMajor = QString::number(SCREENPLAY_GODOT_VERSION_MAJOR);
        QString godotVersionMinor = QString::number(SCREENPLAY_GODOT_VERSION_MINOR);
        obj.insert("godotVersionMajor", godotVersionMajor);
        obj.insert("godotVersionMinor", godotVersionMinor);
        obj.insert("tags", ScreenPlayUtil::fillArray(tags));
        obj.insert("type", QVariant::fromValue(ContentTypes::InstalledType::GodotWallpaper).toString());
        obj.insert("file", "wallpaper.tscn");

        if (!previewThumbnail.isEmpty()) {
            if (!Util::copyPreviewThumbnail(obj, previewThumbnail, workingPath)) {
                emit widgetCreationFinished(WizardResult::CopyPreviewThumbnailError);
                return;
            }
        } else {
            obj.insert("preview", "preview.png");
            createPreviewImage(title, workingPath);
        }

        if (!Util::writeFileFromQrc(":/qml/ScreenPlayApp/assets/wizards/" + licenseFile, workingPath + "/" + licenseFile)) {
            qWarning() << "Could not write " << licenseFile;
            emit widgetCreationFinished(WizardResult::WriteLicenseFileError);
            return;
        }

        if (!Util::writeSettings(obj, workingPath + "/project.json")) {
            emit widgetCreationFinished(WizardResult::WriteProjectFileError);
            return;
        }

        if (!Util::writeFileFromQrc(":/qml/ScreenPlayApp/qml/Create/WizardsFiles/Godot_v5/project.godot", workingPath + "/project.godot")) {
            qWarning() << "Could not write project.godot";
            return;
        }

        if (!Util::writeFileFromQrc(":/qml/ScreenPlayApp/qml/Create/WizardsFiles/Godot_v5/spinner.gd", workingPath + "/spinner.gd")) {
            qWarning() << "Could not write spinner.gd";
            return;
        }

        if (!Util::writeFileFromQrc(":/qml/ScreenPlayApp/qml/Create/WizardsFiles/Godot_v5/wallpaper.tscn", workingPath + "/wallpaper.tscn")) {
            qWarning() << "Could not write wallpaper.tscn";
            return;
        }

        // This presets file is needed for the export. Because we do only export
        // package files, it does not matter that we hardcode "Windows Desktop" as
        // export preset.
        if (!Util::writeFileFromQrc(":/qml/ScreenPlayApp/qml/Create/WizardsFiles/Godot_v5/export_presets.cfg", workingPath + "/export_presets.cfg")) {
            qWarning() << "Could not write export_presets.cfg";
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
        obj.insert("type", QVariant::fromValue(ContentTypes::InstalledType::GifWallpaper).toString());

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
        obj.insert("type", QVariant::fromValue(ContentTypes::InstalledType::WebsiteWallpaper).toString());
        obj.insert("url", url.toString());

        if (!previewThumbnail.isEmpty()) {
            if (!Util::copyPreviewThumbnail(obj, previewThumbnail, workingPath)) {
                emit widgetCreationFinished(WizardResult::CopyPreviewThumbnailError);
                return;
            }
        } else {
            obj.insert("preview", "preview.png");
            createPreviewImage(title, workingPath);
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

/*!
    \brief Creates a default preview.png.
*/
void Wizards::createPreviewImage(const QString& name, const QString& targetPath)
{
    // Step 1: Create QImage and QPainter
    QImage image(749, 442, QImage::Format_ARGB32);
    QPainter painter(&image);

    // Step 2: Select Random Colors (example colors, replace with Material colors)
    // These are just placeholder colors, replace with actual Material colors
    int randomIndex1 = QRandomGenerator::global()->bounded(5);
    int randomIndex2 = QRandomGenerator::global()->bounded(5);

    // Step 3: Create and Set Gradient
    QLinearGradient gradient(QPointF(0, image.height()), QPointF(image.width(), 0));
    gradient.setColorAt(0, m_gradientColors[randomIndex1].darker()); // Dark color
    gradient.setColorAt(1, m_gradientColors[randomIndex2].lighter()); // Bright color

    painter.fillRect(image.rect(), gradient);

    // Step 4: Draw Text
    // Set the font size depending on the size of the image
    painter.setPen(Qt::white);
    int fontSize = qMin(image.width(), image.height()) / 10; // Adjust proportion as needed
    QFont font = painter.font();
    font.setFamily("Noto Sans Light");
    font.setPointSize(fontSize);
    painter.setFont(font);

    // Define a margin and adjust the rect accordingly
    int margin = 30;
    QRect rect(margin, margin, image.width() - 2 * margin, image.height() - 2 * margin);

    // Draw text within the adjusted rect
    QTextOption option(Qt::AlignCenter);
    painter.drawText(rect, name, option);

    // Step 5: Save Image
    image.save(targetPath + "/preview.png");
}
}

#include "moc_wizards.cpp"
