#include "ScreenPlayUtil/archive.h"
#include "ScreenPlayUtil/util.h"
namespace ScreenPlay {

Archive::Archive(QObject* parent)
    : QObject { parent }
{

    m_extractor = std::make_unique<QArchive::DiskExtractor>();
    m_compressor = std::make_unique<QArchive::DiskCompressor>();
    QObject::connect(m_extractor.get(), &QArchive::DiskExtractor::progress, this, &Archive::extractionProgressChanged);
    QObject::connect(m_extractor.get(), &QArchive::DiskExtractor::finished, this, &Archive::extractionFinished);

    QObject::connect(m_compressor.get(), &QArchive::DiskCompressor::progress, this, &Archive::compressionProgressChanged);
    QObject::connect(m_compressor.get(), &QArchive::DiskCompressor::finished, this, &Archive::compressionFinished);
}

/*!
  \brief Imports a given project from a .screenplay zip file. The argument extractionPath
         must be copied otherwise it will get reset in qml before extracting.
*/
bool Archive::importProject(QString archivePath, QString extractionPath)
{
    m_extractor->clear();
    Util util;
    archivePath = util.toLocal(archivePath);
    extractionPath = util.toLocal(extractionPath);

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
  \brief Exports a given project into a .screenplay 7Zip file.
*/
bool Archive::exportProject(QString contentPath, QString exportFileName)
{
    m_compressor->clear();
    Util util;
    contentPath = util.toLocal(contentPath);
    exportFileName = util.toLocal(exportFileName);

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
}
