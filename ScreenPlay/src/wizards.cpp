// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#include "ScreenPlay/wizards.h"
#include "CMakeVariables.h"
#include "ScreenPlayCore/util.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFont>
#include <QFutureWatcher>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QLinearGradient>
#include <QPainter>
#include <QTextOption>
#include <QTime>
#include <QTimer>
#include <QtMath>

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
}

/*!
  \brief Creates a new widget.
*/
QCoro::QmlTask Wizards::createQMLWidget(
    const QString& title,
    const QString& licenseName,
    const QString& licenseFile,
    const QString& createdBy,
    const QString& previewThumbnail,
    const QVector<QString>& tags)
{
    return QCoro::QmlTask([this, title, licenseName, licenseFile, createdBy, previewThumbnail, tags]() -> QCoro::Task<Result> {
        std::optional<QString> folderName = createTemporaryFolder();

        if (!folderName.has_value()) {
            QString errorMessage = tr("Unable to create temporary folder");
            qCritical() << errorMessage;
            co_return Result { false, QVariant::fromValue(WizardResult::CreateProjectFolderError), errorMessage };
        }

        const QString workingPath = m_util.toLocal(m_globalVariables->localStoragePath().toString() + "/" + folderName.value());

        QJsonObject obj;
        obj.insert("license", licenseName);
        obj.insert("title", title);
        obj.insert("tags", m_util.fillArray(tags));
        obj.insert("createdBy", createdBy);
        obj.insert("type", QVariant::fromValue(ContentTypes::InstalledType::QMLWidget).toString());
        obj.insert("file", "main.qml");

        if (!m_util.writeFileFromQrc(":/qt/qml/ScreenPlay/assets/wizards/" + licenseFile, workingPath + "/" + licenseFile)) {
            QString errorMessage = tr("Could not write %1").arg(licenseFile);
            qCritical() << errorMessage;
            co_return Result { false, QVariant::fromValue(WizardResult::WriteLicenseFileError), errorMessage };
        }

        if (!m_util.writeFileFromQrc(":/qt/qml/ScreenPlay/qml/Create/WizardsFiles/QMLWidgetMain.qml", workingPath + "/main.qml")) {
            QString errorMessage = tr("Could not write main.qml");
            qCritical() << errorMessage;
            co_return Result { false, QVariant::fromValue(WizardResult::WriteFileError), errorMessage };
        }

        if (!previewThumbnail.isEmpty()) {
            if (!m_util.copyPreviewThumbnail(obj, previewThumbnail, workingPath)) {
                QString errorMessage = tr("Could not copy thumbnail");
                qCritical() << errorMessage;
                co_return Result { false, QVariant::fromValue(WizardResult::CopyPreviewThumbnailError), errorMessage };
            }
        } else {
            obj.insert("preview", "preview.png");
            createPreviewImage(title, workingPath);
        }

        if (!m_util.writeSettings(obj, workingPath + "/project.json")) {
            QString errorMessage = tr("Could not write project file");
            qCritical() << errorMessage;
            co_return Result { false, QVariant::fromValue(WizardResult::WriteProjectFileError), errorMessage };
        }

        co_return Result { true };
    }());
}

/*!
  \brief Creates a new widget.
*/
QCoro::QmlTask Wizards::createHTMLWidget(
    const QString& title,
    const QString& licenseName,
    const QString& licenseFile,
    const QString& createdBy,
    const QString& previewThumbnail,
    const QVector<QString>& tags)
{
    return QCoro::QmlTask([this, title, licenseName, licenseFile, createdBy, previewThumbnail, tags]() -> QCoro::Task<Result> {
        std::optional<QString> folderName = createTemporaryFolder();

        if (!folderName.has_value()) {
            QString errorMessage = tr("Unable to create temporary folder");
            qCritical() << errorMessage;
            co_return Result { false, QVariant::fromValue(WizardResult::CreateProjectFolderError), errorMessage };
        }

        const QString workingPath = m_util.toLocal(m_globalVariables->localStoragePath().toString() + "/" + folderName.value());

        QJsonObject obj;
        obj.insert("license", licenseName);
        obj.insert("createdBy", createdBy);
        obj.insert("title", title);
        obj.insert("tags", m_util.fillArray(tags));
        obj.insert("type", QVariant::fromValue(ContentTypes::InstalledType::HTMLWidget).toString());
        obj.insert("file", "index.html");

        if (!m_util.writeFileFromQrc(":/qt/qml/ScreenPlay/assets/wizards/" + licenseFile, workingPath + "/" + licenseFile)) {
            QString errorMessage = tr("Could not write %1").arg(licenseFile);
            qCritical() << errorMessage;
            co_return Result { false, QVariant::fromValue(WizardResult::WriteLicenseFileError), errorMessage };
        }

        if (!m_util.writeFileFromQrc(":/qt/qml/ScreenPlay/qml/Create/WizardsFiles/HTMLWidgetMain.html", workingPath + "/index.html")) {
            QString errorMessage = tr("Could not write index.html");
            qCritical() << errorMessage;
            co_return Result { false, QVariant::fromValue(WizardResult::WriteFileError), errorMessage };
        }

        QUrl previewThumbnailUrl { previewThumbnail };
        QFileInfo previewImageFile(previewThumbnailUrl.toLocalFile());

        if (!previewThumbnail.isEmpty()) {
            if (!m_util.copyPreviewThumbnail(obj, previewThumbnail, workingPath)) {
                QString errorMessage = tr("Could copy preview thumbnail");
                qCritical() << errorMessage;
                co_return Result { false, QVariant::fromValue(WizardResult::CopyPreviewThumbnailError), errorMessage };
            }
        } else {
            obj.insert("preview", "preview.png");
            createPreviewImage(title, workingPath);
        }

        if (!m_util.writeSettings(obj, workingPath + "/project.json")) {
            QString errorMessage = tr("Could not write project file");
            qCritical() << errorMessage;
            co_return Result { false, QVariant::fromValue(WizardResult::WriteProjectFileError), errorMessage };
        }

        co_return Result { true };
    }());
}

/*!
  \brief Creates a HTML wallpaper.
*/
QCoro::QmlTask Wizards::createHTMLWallpaper(
    const QString& title,
    const QString& licenseName,
    const QString& licenseFile,
    const QString& createdBy,
    const QString& previewThumbnail,
    const QVector<QString>& tags)
{
    return QCoro::QmlTask([this, title, licenseName, licenseFile, createdBy, previewThumbnail, tags]() -> QCoro::Task<Result> {
        std::optional<QString> folderName = createTemporaryFolder();

        if (!folderName.has_value()) {
            QString errorMessage = tr("Unable to create temporary folder");
            qCritical() << errorMessage;
            co_return Result { false, QVariant::fromValue(WizardResult::CreateProjectFolderError), errorMessage };
        }

        const QString workingPath = m_util.toLocal(m_globalVariables->localStoragePath().toString() + "/" + folderName.value());

        QJsonObject obj;
        obj.insert("license", licenseName);
        obj.insert("createdBy", createdBy);
        obj.insert("title", title);
        obj.insert("tags", m_util.fillArray(tags));
        obj.insert("type", QVariant::fromValue(ContentTypes::InstalledType::HTMLWallpaper).toString());
        obj.insert("file", "index.html");

        if (!m_util.writeFileFromQrc(":/qt/qml/ScreenPlay/assets/wizards/" + licenseFile, workingPath + "/" + licenseFile)) {
            QString errorMessage = tr("Could not write %1").arg(licenseFile);
            qCritical() << errorMessage;
            co_return Result { false, QVariant::fromValue(WizardResult::WriteLicenseFileError), errorMessage };
        }

        if (!m_util.writeFileFromQrc(":/qt/qml/ScreenPlay/qml/Create/WizardsFiles/HTMLWallpaperMain.html", workingPath + "/index.html")) {
            QString errorMessage = tr("Could not write index.html");
            qCritical() << errorMessage;
            co_return Result { false, QVariant::fromValue(WizardResult::WriteFileError), errorMessage };
        }

        if (!previewThumbnail.isEmpty()) {
            if (!m_util.copyPreviewThumbnail(obj, previewThumbnail, workingPath)) {
                QString errorMessage = tr("Could copy preview thumbnail");
                qCritical() << errorMessage;
                co_return Result { false, QVariant::fromValue(WizardResult::CopyPreviewThumbnailError), errorMessage };
            }
        } else {
            obj.insert("preview", "preview.png");
            createPreviewImage(title, workingPath);
        }

        if (!m_util.writeSettings(obj, workingPath + "/project.json")) {
            QString errorMessage = tr("Could not write project file");
            qCritical() << errorMessage;
            co_return Result { false, QVariant::fromValue(WizardResult::WriteProjectFileError), errorMessage };
        }

        co_return Result { true };
    }());
}

/*!
    \brief .
*/
QCoro::QmlTask Wizards::createQMLWallpaper(
    const QString& title,
    const QString& licenseName,
    const QString& licenseFile,
    const QString& createdBy,
    const QString& previewThumbnail,
    const QVector<QString>& tags)
{
    return QCoro::QmlTask([this, title, licenseName, licenseFile, createdBy, previewThumbnail, tags]() -> QCoro::Task<Result> {
        std::optional<QString> folderName = createTemporaryFolder();

        if (!folderName.has_value()) {
            QString errorMessage = tr("Unable to create temporary folder");
            qCritical() << errorMessage;
            co_return Result { false, QVariant::fromValue(WizardResult::CreateProjectFolderError), errorMessage };
        }

        const QString workingPath = m_util.toLocal(m_globalVariables->localStoragePath().toString() + "/" + folderName.value());

        QJsonObject obj;
        obj.insert("license", licenseName);
        obj.insert("title", title);
        obj.insert("createdBy", createdBy);
        obj.insert("tags", m_util.fillArray(tags));
        obj.insert("type", QVariant::fromValue(ContentTypes::InstalledType::QMLWallpaper).toString());
        obj.insert("file", "main.qml");

        if (!previewThumbnail.isEmpty()) {
            if (!m_util.copyPreviewThumbnail(obj, previewThumbnail, workingPath)) {
                QString errorMessage = tr("Could copy preview thumbnail");
                qCritical() << errorMessage;
                co_return Result { false, QVariant::fromValue(WizardResult::CopyPreviewThumbnailError), errorMessage };
            }
        } else {
            obj.insert("preview", "preview.png");
            createPreviewImage(title, workingPath);
        }

        if (!m_util.writeFileFromQrc(":/qt/qml/ScreenPlay/assets/wizards/" + licenseFile, workingPath + "/" + licenseFile)) {
            QString errorMessage = tr("Could not write %1").arg(licenseFile);
            qCritical() << errorMessage;
            co_return Result { false, QVariant::fromValue(WizardResult::WriteLicenseFileError), errorMessage };
        }

        const QString qmlproject = workingPath + "/" + title + ".qmlproject";
        if (!m_util.writeFileFromQrc(":/qt/qml/ScreenPlay/qml/Create/WizardsFiles/QmlProject.qmlproject", qmlproject)) {
            QString errorMessage = tr("Could not write QmlProject.qmlproject");
            qCritical() << errorMessage;
            co_return Result { false, QVariant::fromValue(WizardResult::WriteFileError), errorMessage };
        }

        if (!m_util.writeSettings(obj, workingPath + "/project.json")) {
            QString errorMessage = tr("Could not write project file");
            qCritical() << errorMessage;
            co_return Result { false, QVariant::fromValue(WizardResult::WriteProjectFileError), errorMessage };
        }

        if (!m_util.writeFileFromQrc(":/qt/qml/ScreenPlay/qml/Create/WizardsFiles/QMLWallpaperMain.qml", workingPath + "/main.qml")) {
            QString errorMessage = tr("Could not write main.qml");
            qCritical() << errorMessage;
            co_return Result { false, QVariant::fromValue(WizardResult::WriteFileError), errorMessage };
        }
        co_return Result { true };
    }());
}

/*!
    \brief .
*/
QCoro::QmlTask Wizards::createGodotWallpaper(
    const QString& title,
    const QString& licenseName,
    const QString& licenseFile,
    const QString& createdBy,
    const QString& previewThumbnail,
    const QVector<QString>& tags)
{
    return QCoro::QmlTask([this, title, licenseName, licenseFile, createdBy, previewThumbnail, tags]() -> QCoro::Task<Result> {
        std::optional<QString> folderName = createTemporaryFolder();

        if (!folderName.has_value()) {
            QString errorMessage = tr("Unable to create temporary folder");
            qCritical() << errorMessage;
            co_return Result { false, QVariant::fromValue(WizardResult::CreateProjectFolderError), errorMessage };
        }

        const QString workingPath = m_util.toLocal(m_globalVariables->localStoragePath().toString() + "/" + folderName.value());

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
        obj.insert("tags", m_util.fillArray(tags));
        obj.insert("type", QVariant::fromValue(ContentTypes::InstalledType::GodotWallpaper).toString());
        obj.insert("file", "wallpaper.tscn");

        if (!previewThumbnail.isEmpty()) {
            if (!m_util.copyPreviewThumbnail(obj, previewThumbnail, workingPath)) {
                QString errorMessage = tr("Could copy preview thumbnail");
                qCritical() << errorMessage;
                co_return Result { false, QVariant::fromValue(WizardResult::CopyPreviewThumbnailError), errorMessage };
            }
        } else {
            obj.insert("preview", "preview.png");
            createPreviewImage(title, workingPath);
        }

        if (!m_util.writeFileFromQrc(":/qt/qml/ScreenPlay/assets/wizards/" + licenseFile, workingPath + "/" + licenseFile)) {
            QString errorMessage = tr("Could not write %1").arg(licenseFile);
            qCritical() << errorMessage;
            co_return Result { false, QVariant::fromValue(WizardResult::WriteLicenseFileError), errorMessage };
        }

        if (!m_util.writeSettings(obj, workingPath + "/project.json")) {
            QString errorMessage = tr("Could not write project file");
            qCritical() << errorMessage;
            co_return Result { false, QVariant::fromValue(WizardResult::WriteProjectFileError), errorMessage };
        }

        if (!m_util.writeFileFromQrc(":/qt/qml/ScreenPlay/qml/Create/WizardsFiles/Godot_v5/project.godot", workingPath + "/project.godot")) {
            QString errorMessage = tr("Could not write project.godot");
            qCritical() << errorMessage;
            co_return Result { false, QVariant::fromValue(WizardResult::WriteFileError), errorMessage };
        }

        if (!m_util.writeFileFromQrc(":/qt/qml/ScreenPlay/qml/Create/WizardsFiles/Godot_v5/spinner.gd", workingPath + "/spinner.gd")) {
            QString errorMessage = tr("Could not write spinner.gd");
            qCritical() << errorMessage;
            co_return Result { false, QVariant::fromValue(WizardResult::WriteFileError), errorMessage };
        }

        if (!m_util.writeFileFromQrc(":/qt/qml/ScreenPlay/qml/Create/WizardsFiles/Godot_v5/wallpaper.tscn", workingPath + "/wallpaper.tscn")) {
            QString errorMessage = tr("Could not write wallpaper.tscn");
            qCritical() << errorMessage;
            co_return Result { false, QVariant::fromValue(WizardResult::WriteFileError), errorMessage };
        }

        // This presets file is needed for the export. Because we do only export
        // package files, it does not matter that we hardcode "Windows Desktop" as
        // export preset.
        if (!m_util.writeFileFromQrc(":/qt/qml/ScreenPlay/qml/Create/WizardsFiles/Godot_v5/export_presets.cfg", workingPath + "/export_presets.cfg")) {
            QString errorMessage = tr("Could not write export_presets.cfg");
            qCritical() << errorMessage;
            co_return Result { false, QVariant::fromValue(WizardResult::WriteFileError), errorMessage };
        }

        co_return Result { true };
    }());
}

QCoro::QmlTask Wizards::createGifWallpaper(
    const QString& title,
    const QString& licenseName,
    const QString& licenseFile,
    const QString& creator,
    const QString& file,
    const QVector<QString>& tags)
{
    return QCoro::QmlTask([this, title, licenseName, licenseFile, creator, file, tags]() -> QCoro::Task<Result> {
        std::optional<QString> folderName = createTemporaryFolder();

        if (!folderName.has_value()) {
            QString errorMessage = tr("Unable to create temporary folder");
            qCritical() << errorMessage;
            co_return Result { false, QVariant::fromValue(WizardResult::CreateProjectFolderError), errorMessage };
        }

        const QString workingPath = m_util.toLocal(m_globalVariables->localStoragePath().toString() + "/" + folderName.value());
        const QString gifFileName = QFileInfo(m_util.toLocal(file)).fileName();

        QJsonObject obj;
        obj.insert("license", licenseName);
        obj.insert("creator", creator);
        obj.insert("title", title);
        obj.insert("file", gifFileName);
        obj.insert("previewGIF", gifFileName);
        obj.insert("tags", m_util.fillArray(tags));
        obj.insert("type", QVariant::fromValue(ContentTypes::InstalledType::GifWallpaper).toString());

        if (!m_util.writeFileFromQrc(":/qt/qml/ScreenPlay/assets/wizards/" + licenseFile, workingPath + "/" + licenseFile)) {
            QString errorMessage = tr("Could not write %1").arg(licenseFile);
            qCritical() << errorMessage;
            co_return Result { false, QVariant::fromValue(WizardResult::WriteLicenseFileError), errorMessage };
        }

        if (!m_util.writeSettings(obj, workingPath + "/project.json")) {
            QString errorMessage = tr("Could not write project file");
            qCritical() << errorMessage;
            co_return Result { false, QVariant::fromValue(WizardResult::WriteProjectFileError), errorMessage };
        }

        if (!QFile::copy(m_util.toLocal(file), workingPath + "/" + gifFileName)) {
            QString errorMessage = tr("Could not copy gif");
            qCritical() << errorMessage;
            co_return Result { false, QVariant::fromValue(WizardResult::CopyFileError), errorMessage };
        }

        co_return Result { true };
    }());
}

/*!
    \brief .
*/
QCoro::QmlTask Wizards::createWebsiteWallpaper(
    const QString& title,
    const QString& previewThumbnail,
    const QUrl& url,
    const QVector<QString>& tags)
{
    return QCoro::QmlTask([this, title, previewThumbnail, url, tags]() -> QCoro::Task<Result> {
        std::optional<QString> folderName = createTemporaryFolder();

        if (!folderName.has_value()) {
            QString errorMessage = tr("Unable to create temporary folder");
            qCritical() << errorMessage;
            co_return Result { false, QVariant::fromValue(WizardResult::CreateProjectFolderError), errorMessage };
        }

        const QString workingPath = m_util.toLocal(m_globalVariables->localStoragePath().toString() + "/" + folderName.value());

        QJsonObject obj;
        obj.insert("title", title);
        obj.insert("tags", m_util.fillArray(tags));
        obj.insert("type", QVariant::fromValue(ContentTypes::InstalledType::WebsiteWallpaper).toString());
        obj.insert("url", url.toString());

        if (!previewThumbnail.isEmpty()) {
            if (!m_util.copyPreviewThumbnail(obj, previewThumbnail, workingPath)) {
                QString errorMessage = tr("Could copy preview thumbnail");
                qCritical() << errorMessage;
                co_return Result { false, QVariant::fromValue(WizardResult::CopyPreviewThumbnailError), errorMessage };
            }
        } else {
            obj.insert("preview", "preview.png");
            createPreviewImage(title, workingPath);
        }

        if (!m_util.writeSettings(obj, workingPath + "/project.json")) {
            QString errorMessage = tr("Could not write project file");
            qCritical() << errorMessage;
            co_return Result { false, QVariant::fromValue(WizardResult::WriteProjectFileError), errorMessage };
        }

        co_return Result { true };
    }());
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
