// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#include "ScreenPlay/util.h"

#include "qarchive_enums.hpp"
#include "qarchivediskcompressor.hpp"
#include "qarchivediskextractor.hpp"

#include <QDesktopServices>
#include <QGuiApplication>

#if defined(Q_OS_WIN)
#include <sentry.h>
#endif

namespace ScreenPlay {

/*!
    \class ScreenPlay::Util
    \inmodule ScreenPlay
    \brief Easy to use global object to use when certain functionality is not available in QML.
*/

/*!
  \brief Constructor.
*/
Util::Util()
    : QObject(nullptr)
{
    m_extractor = std::make_unique<QArchive::DiskExtractor>();
    m_compressor = std::make_unique<QArchive::DiskCompressor>();

    QObject::connect(m_extractor.get(), &QArchive::DiskExtractor::progress, this, &Util::extractionProgressChanged);
    QObject::connect(m_extractor.get(), &QArchive::DiskExtractor::finished, this, &Util::extractionFinished);

    QObject::connect(m_compressor.get(), &QArchive::DiskCompressor::progress, this, &Util::compressionProgressChanged);
    QObject::connect(m_compressor.get(), &QArchive::DiskCompressor::finished, this, &Util::compressionFinished);
}

/*!
  \brief Needed only for QArchive unique_ptr
        https://stackoverflow.com/questions/28386185/cant-use-stdunique-ptrt-with-t-being-a-forward-declaration
*/
Util::~Util() { }

/*!
  \brief Copies the given string to the clipboard.
*/
void Util::copyToClipboard(const QString& text) const
{
    auto* clipboard = QGuiApplication::clipboard();
    clipboard->setText(text);
}

/*!
   \brief Writes a given QJsonObject to a file. The path must be absolute. When truncate is set to
   true the exsisting json file will be overriten.
*/
bool Util::writeJsonObjectToFile(const QString& absoluteFilePath, const QJsonObject& object, bool truncate)
{
    QFile configTmp;
    configTmp.setFileName(absoluteFilePath);
    QIODevice::OpenMode openMode;
    if (truncate) {
        openMode = QIODevice::ReadWrite | QIODevice::Truncate;
    } else {
        openMode = QIODevice::ReadWrite | QIODevice::Append;
    }

    if (!configTmp.open(openMode)) {
        qWarning() << "Could not open out file!" << configTmp.errorString();
        return false;
    }

    QTextStream out(&configTmp);
    out.setEncoding(QStringConverter::Utf8);
    out << QJsonDocument(object).toJson();

    configTmp.close();
    return true;
}

/*!
  \brief Opens a native folder window on the given path. Windows and Mac only for now!
*/
void Util::openFolderInExplorer(const QString& url) const
{
    const QString path = QUrl::fromUserInput(url).toLocalFile();

    // QDesktopServices can hang on Windows
    if (QSysInfo::productType() == "windows") {
        QProcess explorer;
        explorer.setProgram("explorer.exe");
        // When we have space in the path like
        // C:\Program Files (x86)\Steam\...
        // we cannot set the path as an argument. But we can set the working it
        // to the wanted path and open the current path via the dot.
        explorer.setWorkingDirectory(QDir::toNativeSeparators(path));
        explorer.setArguments({ "." });
        explorer.startDetached();
        return;
    }

    QDesktopServices::openUrl(QUrl::fromLocalFile(path));
}

/*!
  \brief Removes file///: or file:// from the url/string
*/
QString Util::toLocal(const QString& url) const
{
    return ScreenPlayUtil::toLocal(url);
}

/*!
  \brief Exports a given project into a .screenplay 7Zip file.
*/
bool Util::exportProject(QString contentPath, QString exportFileName)
{
    m_compressor->clear();
    contentPath = ScreenPlayUtil::toLocal(contentPath);
    exportFileName = ScreenPlayUtil::toLocal(exportFileName);

    QDir dir(contentPath);
    bool success = true;
    if (!dir.exists()) {
        qWarning() << "Directory does not exist!" << dir;
        return false;
    }
    QStringList files;
    for (auto& item : dir.entryInfoList(QDir::Files)) {
        files.append(item.absoluteFilePath());
    }
    QFile exportFile(exportFileName);
    if (exportFile.exists()) {
        if (!exportFile.remove()) {
            qWarning() << "Unable to delte file marked to override!" << dir;
            return false;
        }
    }
    m_compressor->setFileName(exportFileName);
    m_compressor->setArchiveFormat(QArchive::SevenZipFormat);
    m_compressor->addFiles(files);
    m_compressor->start();
    return true;
}

/*!
  \brief Imports a given project from a .screenplay zip file. The argument extractionPath
         must be copied otherwise it will get reset in qml before extracting.
*/
bool Util::importProject(QString archivePath, QString extractionPath)
{
    m_extractor->clear();
    archivePath = ScreenPlayUtil::toLocal(archivePath);
    extractionPath = ScreenPlayUtil::toLocal(extractionPath);

    QFileInfo fileInfo(archivePath);
    if (!fileInfo.fileName().endsWith(".screenplay")) {
        qWarning() << "Unsupported file type: " << fileInfo.fileName() << ". We only support '.screenplay' files.";
        return false;
    }
    const QString name = fileInfo.fileName().remove(".screenplay");

    const auto timestamp = QDateTime::currentDateTime().toString("ddMMyyyyhhmmss-");
    extractionPath = extractionPath + "/" + timestamp + name + "/";
    QDir dir(extractionPath);

    if (dir.exists()) {
        qWarning() << "Directory does already exist!" << dir;
        return false;
    }

    if (!dir.mkdir(extractionPath)) {
        qWarning() << "Unable to create directory:" << dir;
        return false;
    }

    m_extractor->setArchive(archivePath);
    m_extractor->setOutputDirectory(extractionPath);
    m_extractor->setCalculateProgress(true);
    m_extractor->getInfo();
    m_extractor->start();
    return true;
}

/*!
  \brief Loads all content of the legal folder in the qrc into a property string of this class.
  allLicenseLoaded is emited when loading is finished.
*/
void Util::Util::requestAllLicenses()
{
    if (m_requestAllLicensesFuture.isRunning())
        return;

    m_requestAllLicensesFuture = QtConcurrent::run([this]() {
        QString tmp;
        QFile file;
        QTextStream out(&file);

        file.setFileName(":/qml/ScreenPlayApp/legal/Font Awesome Free License.txt");
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        tmp += out.readAll();
        file.close();

        file.setFileName(":/qml/ScreenPlayApp/legal/gpl-3.0.txt");
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        tmp += out.readAll();
        file.close();

        file.setFileName(":/qml/ScreenPlayApp/legal/gpl-3.0.txt");
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        tmp += out.readAll();
        file.close();

        file.setFileName(":/qml/ScreenPlayApp/legal/OFL.txt");
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        tmp += out.readAll();
        file.close();

        file.setFileName(":/qml/ScreenPlayApp/legal/OpenSSL.txt");
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        tmp += out.readAll();
        file.close();

        file.setFileName(":/qml/ScreenPlayApp/legal/Qt LGPLv3.txt");
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        tmp += out.readAll();
        file.close();

        emit this->allLicenseLoaded(tmp);
    });
}

/*!
  \brief Loads all dataprotection of the legal folder in the qrc into a property string of this class.
  allDataProtectionLoaded is emited when loading is finished.
*/
void Util::Util::requestDataProtection()
{
    QString tmp;
    QFile file;
    QTextStream out(&file);

    file.setFileName(":/qml/ScreenPlayApp/legal/DataProtection.txt");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    tmp += out.readAll();
    file.close();

    emit this->allDataProtectionLoaded(tmp);
}

bool Util::fileExists(const QString& filePath) const
{
    const QFileInfo file(toLocal(filePath));
    return file.isFile();
}


/*!
  \brief Takes ownership of \a obj and \a name. Tries to save into a text file
    with of name.
*/
bool Util::writeSettings(const QJsonObject& obj, const QString& absolutePath)
{
    QFile file { absolutePath };
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Could not open" << absolutePath;
        return false;
    }

    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    QJsonDocument doc(obj);

    out << doc.toJson();

    file.close();
    return true;
}

/*!
  \brief Tries to save into a text file with absolute path.
*/
bool Util::writeFile(const QString& text, const QString& absolutePath)
{
    QFile file { absolutePath };
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Could not open" << absolutePath;
        return false;
    }

    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    out << text;

    file.close();
    return true;
}

/*!
  \brief Tries to save into a text file with absolute path.
*/
bool Util::writeFileFromQrc(const QString& qrcPath, const QString& absolutePath)
{

    QFile file { absolutePath };
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Could not open" << absolutePath;
        return false;
    }

    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);

    QFile qrc(qrcPath);
    qrc.open(QIODevice::ReadOnly);

    QTextStream in(&qrc);
    // Read line by line to avoid CLRF/LF issues
    while (!in.atEnd()) {
        out << in.readLine() << "\n";
    }

    qrc.close();
    file.close();
    return true;
}

/*!
  \brief Takes reference to \a obj. If the copy of the thumbnail is successful,
  it adds the corresponding settings entry to the json object reference.
*/
bool Util::copyPreviewThumbnail(QJsonObject& obj, const QString& previewThumbnail, const QString& destination)
{
    const QUrl previewThumbnailUrl { previewThumbnail };
    const QFileInfo previewImageFile(previewThumbnailUrl.toString());
    const QString destinationFilePath = destination + "/" + previewImageFile.fileName();

    if (!previewThumbnail.isEmpty()) {
        if (!QFile::copy(previewThumbnailUrl.toLocalFile(), destinationFilePath)) {
            qDebug() << "Could not copy" << previewThumbnailUrl.toLocalFile() << " to " << destinationFilePath;
            return false;
        }
    }

    obj.insert("previewThumbnail", previewImageFile.fileName());
    obj.insert("preview", previewImageFile.fileName());

    return true;
}

}

#include "moc_util.cpp"
