#pragma once

#include <QAbstractListModel>
#include <QByteArray>
#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFuture>
#include <QFutureWatcher>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>
#include <QString>
#include <QUrl>
#include <QVector>
#include <QFileSystemWatcher>
#include <QtConcurrent/QtConcurrent>


#include "profilelistmodel.h"
#include "projectfile.h"

namespace ScreenPlay {

class InstalledListModel : public QAbstractListModel {
    Q_OBJECT


    Q_PROPERTY(int count READ count WRITE setCount NOTIFY countChanged)
    Q_PROPERTY(QUrl absoluteStoragePath READ absoluteStoragePath WRITE setabsoluteStoragePath NOTIFY absoluteStoragePathChanged)

public:
    explicit InstalledListModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;


    enum InstalledRole {
        TitleRole = Qt::UserRole,
        TypeRole,
        PreviewRole,
        PreviewGIFRole,
        FolderIdRole,
        FileIdRole,
        AbsoluteStoragePathRole,
        WorkshopIDRole,
    };
    Q_ENUM(InstalledRole)

    QUrl absoluteStoragePath() const
    {
        return m_absoluteStoragePath;
    }

    int count() const
    {
        return m_count;
    }

public slots:
    void loadInstalledContent();
    void append(const QJsonObject &, const QString &);
    void reset();
    void init();
    QVariantMap get(QString folderId);

    void setabsoluteStoragePath(const QUrl& absoluteStoragePath)
    {
        if (m_absoluteStoragePath == absoluteStoragePath)
            return;

        m_absoluteStoragePath = absoluteStoragePath;
        emit absoluteStoragePathChanged(m_absoluteStoragePath);
    }
    void setCount(int count)
    {
        if (m_count == count)
            return;

        m_count = count;
        emit countChanged(m_count);
    }

signals:
    void setScreenVisible(bool visible);
    void setScreenToVideo(QString absolutePath);
    void absoluteStoragePathChanged(QUrl absoluteStoragePath);
    void addInstalledItem(const QJsonObject, const QString);
    void installedLoadingFinished();
    void isLoadingContentChanged(bool isLoadingContent);

    void countChanged(int count);

private:
    QFileSystemWatcher m_fileSystemWatcher;
    QVector<ProjectFile> m_screenPlayFiles;
    QUrl m_absoluteStoragePath;
    int m_count{0};
};
}
