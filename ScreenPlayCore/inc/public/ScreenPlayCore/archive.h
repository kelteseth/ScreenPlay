#pragma once
#include <QObject>
#include <QQmlEngine>

#include "qarchive_enums.hpp"
#include "qarchivediskcompressor.hpp"
#include "qarchivediskextractor.hpp"

namespace ScreenPlay {

class Archive : public QObject {
    Q_OBJECT
    QML_ELEMENT
public:
    explicit Archive(QObject* parent = nullptr);

    Q_INVOKABLE bool importProject(QString archivePath, QString extractionPath);
    Q_INVOKABLE bool exportProject(QString contentPath, QString exportFileName);

signals:
    void extractionProgressChanged(QString file, int proc, int total, qint64 br, qint64 bt);
    void extractionFinished();
    void compressionProgressChanged(QString file, int proc, int total, qint64 br, qint64 bt);
    void compressionFinished();

private:
    std::unique_ptr<QArchive::DiskCompressor> m_compressor;
    std::unique_ptr<QArchive::DiskExtractor> m_extractor;
};
}
