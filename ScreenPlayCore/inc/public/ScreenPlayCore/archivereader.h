#pragma once

#include <QByteArray>
#include <QJsonObject>
#include <QObject>
#include <QString>
#include <QStringList>
#include <expected>

namespace ScreenPlay {

struct ArchiveEntry {
    QString path;
    QString name;
    qint64 size;
    bool isDirectory;

    ArchiveEntry() = default;
    ArchiveEntry(const QString& path, const QString& name, qint64 size, bool isDirectory)
        : path(path)
        , name(name)
        , size(size)
        , isDirectory(isDirectory)
    {
    }
};

class ArchiveReader : public QObject {
    Q_OBJECT

public:
    explicit ArchiveReader(QObject* parent = nullptr);
    ~ArchiveReader();

    std::expected<bool, QString> openArchive(const QString& archivePath);
    void closeArchive();
    bool isOpen() const;
    std::expected<QByteArray, QString> readFileFromArchive(const QString& filePath) const;
    std::expected<QList<ArchiveEntry>, QString> listEntries() const;
    std::expected<QStringList, QString> findFiles(const QString& pattern, bool caseSensitive = false) const;
    bool containsFile(const QString& filePath) const;
    std::expected<ArchiveEntry, QString> getFileInfo(const QString& filePath) const;
    QString archivePath() const;
    std::expected<QJsonObject, QString> readJsonFromArchive(const QString& jsonFilePath) const;

private:
    class ArchiveReaderPrivate;
    std::unique_ptr<ArchiveReaderPrivate> d;
};

} // namespace ScreenPlay