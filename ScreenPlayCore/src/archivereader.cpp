// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#include "ScreenPlayCore/archivereader.h"

#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QRegularExpression>
#include <archive.h>
#include <archive_entry.h>

namespace ScreenPlay {

class ArchiveReader::ArchiveReaderPrivate {
public:
    ArchiveReaderPrivate() = default;
    ~ArchiveReaderPrivate()
    {
        closeArchive();
    }

    std::expected<bool, QString> openArchive(const QString& archivePath)
    {
        closeArchive();

        QFileInfo fileInfo(archivePath);
        if (!fileInfo.exists() || !fileInfo.isFile()) {
            return std::unexpected(QString("Archive file does not exist: %1").arg(archivePath));
        }

        m_archive = archive_read_new();
        if (!m_archive) {
            return std::unexpected("Failed to create archive reader");
        }

        archive_read_support_filter_all(m_archive);
        archive_read_support_format_all(m_archive);

        QByteArray archivePathBytes = archivePath.toLocal8Bit();
        int result = archive_read_open_filename(m_archive, archivePathBytes.constData(), 10240);
        if (result != ARCHIVE_OK) {
            QString errorMsg = QString("Failed to open archive: %1 - %2")
                                   .arg(archivePath)
                                   .arg(QString::fromUtf8(archive_error_string(m_archive)));
            closeArchive();
            return std::unexpected(errorMsg);
        }

        m_archivePath = archivePath;
        m_isOpen = true;
        return true;
    }

    void closeArchive()
    {
        if (m_archive) {
            if (m_isOpen) {
                archive_read_close(m_archive);
            }
            archive_read_free(m_archive);
            m_archive = nullptr;
        }
        m_isOpen = false;
        m_archivePath.clear();
        m_entriesCache.clear();
        m_entriesCached = false;
    }

    bool isOpen() const
    {
        return m_isOpen && m_archive != nullptr;
    }

    /*! \brief Reads a specific file from the archive without extracting the entire archive.
              Creates a new archive instance for reading since libarchive doesn't support random access well.
     */
    std::expected<QByteArray, QString> readFileFromArchive(const QString& filePath) const
    {
        if (!isOpen()) {
            return std::unexpected("No archive is currently open");
        }

        struct archive* readArchive = archive_read_new();
        if (!readArchive) {
            return std::unexpected("Failed to create archive reader for file reading");
        }

        archive_read_support_filter_all(readArchive);
        archive_read_support_format_all(readArchive);

        QByteArray archivePathBytes = m_archivePath.toLocal8Bit();
        int result = archive_read_open_filename(readArchive, archivePathBytes.constData(), 10240);
        if (result != ARCHIVE_OK) {
            QString errorMsg = QString("Failed to reopen archive for reading: %1")
                                   .arg(QString::fromUtf8(archive_error_string(readArchive)));
            archive_read_free(readArchive);
            return std::unexpected(errorMsg);
        }

        struct archive_entry* entry;
        QByteArray fileContent;
        bool fileFound = false;

        while (archive_read_next_header(readArchive, &entry) == ARCHIVE_OK) {
            const char* entryPath = archive_entry_pathname(entry);
            QString currentPath = QString::fromUtf8(entryPath);

            QString normalizedCurrentPath = QDir::fromNativeSeparators(currentPath);
            QString normalizedFilePath = QDir::fromNativeSeparators(filePath);

            if (normalizedCurrentPath == normalizedFilePath || normalizedCurrentPath.endsWith("/" + normalizedFilePath) || (normalizedFilePath.startsWith("/") && normalizedCurrentPath == normalizedFilePath.mid(1))) {

                if (archive_entry_filetype(entry) == AE_IFREG) {
                    la_int64_t size = archive_entry_size(entry);

                    if (size >= 0 && size < 100 * 1024 * 1024) { // 100MB limit for safety
                        fileContent.resize(static_cast<int>(size));

                        la_ssize_t bytesRead = archive_read_data(readArchive, fileContent.data(), size);
                        if (bytesRead == size) {
                            fileFound = true;
                        } else {
                            archive_read_close(readArchive);
                            archive_read_free(readArchive);
                            return std::unexpected(QString("Failed to read complete file data for: %1").arg(filePath));
                        }
                    } else {
                        archive_read_close(readArchive);
                        archive_read_free(readArchive);
                        return std::unexpected(QString("File size is invalid or too large: %1 (size: %2)").arg(filePath).arg(size));
                    }
                } else {
                    archive_read_close(readArchive);
                    archive_read_free(readArchive);
                    return std::unexpected(QString("Path exists but is not a file: %1").arg(filePath));
                }
                break;
            } else {
                archive_read_data_skip(readArchive);
            }
        }

        archive_read_close(readArchive);
        archive_read_free(readArchive);

        if (!fileFound) {
            return std::unexpected(QString("File not found in archive: %1").arg(filePath));
        }

        return fileContent;
    }

    /*!
     * Lists all entries in the archive. Uses caching to avoid re-reading the archive multiple times.
     */
    std::expected<QList<ArchiveEntry>, QString> listEntries() const
    {
        if (!isOpen()) {
            return std::unexpected("No archive is currently open");
        }

        if (m_entriesCached) {
            return m_entriesCache;
        }

        struct archive* listArchive = archive_read_new();
        if (!listArchive) {
            return std::unexpected("Failed to create archive reader for listing");
        }

        archive_read_support_filter_all(listArchive);
        archive_read_support_format_all(listArchive);

        QByteArray archivePathBytes = m_archivePath.toLocal8Bit();
        int result = archive_read_open_filename(listArchive, archivePathBytes.constData(), 10240);
        if (result != ARCHIVE_OK) {
            QString errorMsg = QString("Failed to reopen archive for listing: %1")
                                   .arg(QString::fromUtf8(archive_error_string(listArchive)));
            archive_read_free(listArchive);
            return std::unexpected(errorMsg);
        }

        QList<ArchiveEntry> entries;
        struct archive_entry* entry;

        while (archive_read_next_header(listArchive, &entry) == ARCHIVE_OK) {
            const char* entryPath = archive_entry_pathname(entry);
            QString path = QString::fromUtf8(entryPath);

            QFileInfo pathInfo(path);
            QString name = pathInfo.fileName();
            la_int64_t size = archive_entry_size(entry);
            bool isDirectory = (archive_entry_filetype(entry) == AE_IFDIR);

            entries.append(ArchiveEntry(path, name, size, isDirectory));
            archive_read_data_skip(listArchive);
        }

        archive_read_close(listArchive);
        archive_read_free(listArchive);

        m_entriesCache = entries;
        m_entriesCached = true;

        return entries;
    }

    std::expected<QStringList, QString> findFiles(const QString& pattern, bool caseSensitive) const
    {
        auto entriesResult = listEntries();
        if (!entriesResult.has_value()) {
            return std::unexpected(entriesResult.error());
        }

        QStringList matchingFiles;
        QRegularExpression regex;

        QString regexPattern = QRegularExpression::wildcardToRegularExpression(pattern);
        regex.setPattern(regexPattern);

        if (!caseSensitive) {
            regex.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
        }

        for (const auto& entry : entriesResult.value()) {
            if (!entry.isDirectory && regex.match(entry.path).hasMatch()) {
                matchingFiles.append(entry.path);
            }
        }

        return matchingFiles;
    }

    bool containsFile(const QString& filePath) const
    {
        auto entriesResult = listEntries();
        if (!entriesResult.has_value()) {
            return false;
        }

        QString normalizedFilePath = QDir::fromNativeSeparators(filePath);

        for (const auto& entry : entriesResult.value()) {
            if (entry.isDirectory)
                continue;

            QString normalizedEntryPath = QDir::fromNativeSeparators(entry.path);
            if (normalizedEntryPath == normalizedFilePath || normalizedEntryPath.endsWith("/" + normalizedFilePath) || (normalizedFilePath.startsWith("/") && normalizedEntryPath == normalizedFilePath.mid(1))) {
                return true;
            }
        }

        return false;
    }

    std::expected<ArchiveEntry, QString> getFileInfo(const QString& filePath) const
    {
        auto entriesResult = listEntries();
        if (!entriesResult.has_value()) {
            return std::unexpected(entriesResult.error());
        }

        QString normalizedFilePath = QDir::fromNativeSeparators(filePath);

        for (const auto& entry : entriesResult.value()) {
            QString normalizedEntryPath = QDir::fromNativeSeparators(entry.path);
            if (normalizedEntryPath == normalizedFilePath || normalizedEntryPath.endsWith("/" + normalizedFilePath) || (normalizedFilePath.startsWith("/") && normalizedEntryPath == normalizedFilePath.mid(1))) {
                return entry;
            }
        }

        return std::unexpected(QString("File not found in archive: %1").arg(filePath));
    }

    QString archivePath() const
    {
        return m_archivePath;
    }

private:
    struct archive* m_archive = nullptr;
    bool m_isOpen = false;
    QString m_archivePath;

    mutable QList<ArchiveEntry> m_entriesCache;
    mutable bool m_entriesCached = false;
};

ArchiveReader::ArchiveReader(QObject* parent)
    : QObject(parent)
    , d(std::make_unique<ArchiveReaderPrivate>())
{
}

ArchiveReader::~ArchiveReader() = default;

std::expected<bool, QString> ArchiveReader::openArchive(const QString& archivePath)
{
    return d->openArchive(archivePath);
}

void ArchiveReader::closeArchive()
{
    d->closeArchive();
}

bool ArchiveReader::isOpen() const
{
    return d->isOpen();
}

std::expected<QByteArray, QString> ArchiveReader::readFileFromArchive(const QString& filePath) const
{
    return d->readFileFromArchive(filePath);
}

std::expected<QList<ArchiveEntry>, QString> ArchiveReader::listEntries() const
{
    return d->listEntries();
}

std::expected<QStringList, QString> ArchiveReader::findFiles(const QString& pattern, bool caseSensitive) const
{
    return d->findFiles(pattern, caseSensitive);
}

bool ArchiveReader::containsFile(const QString& filePath) const
{
    return d->containsFile(filePath);
}

std::expected<ArchiveEntry, QString> ArchiveReader::getFileInfo(const QString& filePath) const
{
    return d->getFileInfo(filePath);
}

QString ArchiveReader::archivePath() const
{
    return d->archivePath();
}

/*!
 * Convenience function that reads and parses a JSON file from the archive in one step.
 */
std::expected<QJsonObject, QString> ArchiveReader::readJsonFromArchive(const QString& jsonFilePath) const
{
    auto fileResult = readFileFromArchive(jsonFilePath);
    if (!fileResult.has_value()) {
        return std::unexpected(fileResult.error());
    }

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(fileResult.value(), &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        return std::unexpected(QString("Failed to parse JSON: %1").arg(parseError.errorString()));
    }

    if (!doc.isObject()) {
        return std::unexpected("JSON content is not an object");
    }

    return doc.object();
}

} // namespace ScreenPlay

#include "moc_archivereader.cpp"
