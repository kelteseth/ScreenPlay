#include "ScreenPlayCore/sparchive.h"
#include "ScreenPlayCore/util.h"
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(coreArchive, "screenplay.core.archive")

namespace ScreenPlay {

SPArchive::SPArchive(QObject* parent)
    : QObject { parent }
{

    m_extractor = std::make_unique<QArchive::DiskExtractor>();
    m_compressor = std::make_unique<QArchive::DiskCompressor>();
    QObject::connect(m_extractor.get(), &QArchive::DiskExtractor::progress, this, &SPArchive::extractionProgressChanged);
    QObject::connect(m_extractor.get(), &QArchive::DiskExtractor::finished, this, &SPArchive::extractionFinished);

    QObject::connect(m_compressor.get(), &QArchive::DiskCompressor::progress, this, &SPArchive::compressionProgressChanged);
    QObject::connect(m_compressor.get(), &QArchive::DiskCompressor::finished, this, &SPArchive::compressionFinished);
}

/*!
  \brief Imports a given project from a .screenplay zip file. The argument extractionPath
         must be copied otherwise it will get reset in qml before extracting.
*/
bool SPArchive::importProject(QString archivePath, QString extractionPath)
{
    m_extractor->clear();
    Util util;
    archivePath = util.toLocal(archivePath);
    extractionPath = util.toLocal(extractionPath);

    QFileInfo fileInfo(archivePath);
    if (!fileInfo.fileName().endsWith(".screenplay")) {
        qCWarning(coreArchive) << "Unsupported file type: " << fileInfo.fileName() << ". We only support '.screenplay' files.";
        return false;
    }
    const QString name = fileInfo.fileName().remove(".screenplay");

    const auto timestamp = QDateTime::currentDateTime().toString("ddMMyyyyhhmmss-");
    extractionPath = extractionPath + "/" + timestamp + name + "/";
    QDir dir(extractionPath);

    if (dir.exists()) {
        qCWarning(coreArchive) << "Directory does already exist!" << dir;
        return false;
    }

    if (!dir.mkdir(extractionPath)) {
        qCWarning(coreArchive) << "Unable to create directory:" << dir;
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
bool SPArchive::exportProject(QString contentPath, QString exportFileName)
{
    m_compressor->clear();
    Util util;
    contentPath = util.toLocal(contentPath);
    exportFileName = util.toLocal(exportFileName);

    QDir dir(contentPath);
    bool success = true;
    if (!dir.exists()) {
        qCWarning(coreArchive) << "Directory does not exist!" << dir;
        return false;
    }
    QStringList files;
    for (auto& item : dir.entryInfoList(QDir::Files)) {
        files.append(item.absoluteFilePath());
    }
    QFile exportFile(exportFileName);
    if (exportFile.exists()) {
        if (!exportFile.remove()) {
            qCWarning(coreArchive) << "Unable to delte file marked to override!" << dir;
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

#include "moc_sparchive.cpp"