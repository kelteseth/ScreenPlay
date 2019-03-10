#pragma once

#include "profilelistmodel.h"
#include "projectfile.h"
#include <QAbstractListModel>
#include <QByteArray>
#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>
#include <QString>
#include <QUrl>
#include <QVector>
#include <QtConcurrent/QtConcurrent>
#include <QFutureWatcher>
#include <QFuture>


/*!
    \class Installed List Model
    \brief Lists all installed items from a given Path

*/

class InstalledListModel : public QAbstractListModel {
    Q_OBJECT

public:
    explicit InstalledListModel(QObject* parent = 0);

    // Basic functionality:
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    bool getProjectByAbsoluteStoragePath(QUrl* path, ProjectFile* spf);

    Q_PROPERTY(QUrl absoluteStoragePath READ absoluteStoragePath WRITE setabsoluteStoragePath NOTIFY absoluteStoragePathChanged)

    enum InstalledRole {
        TitleRole,
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

public slots:
    void loadInstalledContent();
    QVariantMap get(QString folderId);
    void append(const QJsonObject, const QString);
    void setabsoluteStoragePath(QUrl absoluteStoragePath)
    {
        if (m_absoluteStoragePath == absoluteStoragePath)
            return;

        m_absoluteStoragePath = absoluteStoragePath;
        emit absoluteStoragePathChanged(m_absoluteStoragePath);
    }
    int getAmountItemLoaded();
    void reset();

signals:
    void setScreenVisible(bool visible);
    void setScreenToVideo(QString absolutePath);
    void absoluteStoragePathChanged(QUrl absoluteStoragePath);
    void addInstalledItem(const QJsonObject, const QString);
    void installedLoadingFinished();

private:
    QVector<ProjectFile> m_screenPlayFiles;
    QUrl m_absoluteStoragePath;
    QFuture<void> m_loadScreenFuture;
    QFutureWatcher<void> m_loadScreenWatcher;
};
