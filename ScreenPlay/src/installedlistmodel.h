#pragma once

#include "profilelistmodel.h"
#include "projectfile.h"
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
#include <QtConcurrent/QtConcurrent>

/*!
    \class Installed List Model
    \brief Lists all installed wallpapers, widgets etc. from a given Path

*/

class InstalledListModel : public QAbstractListModel {
    Q_OBJECT

    Q_PROPERTY(QUrl absoluteStoragePath READ absoluteStoragePath WRITE setabsoluteStoragePath NOTIFY absoluteStoragePathChanged)

public:
    explicit InstalledListModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    bool getProjectByAbsoluteStoragePath(QUrl* path, ProjectFile* spf);

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

public slots:
    void loadInstalledContent();
    void append(const QJsonObject, const QString);
    void reset();

    int getAmountItemLoaded();

    QVariantMap get(QString folderId);

    void setabsoluteStoragePath(QUrl absoluteStoragePath)
    {
        if (m_absoluteStoragePath == absoluteStoragePath)
            return;

        m_absoluteStoragePath = absoluteStoragePath;
        emit absoluteStoragePathChanged(m_absoluteStoragePath);
    }
signals:
    void setScreenVisible(bool visible);
    void setScreenToVideo(QString absolutePath);
    void absoluteStoragePathChanged(QUrl absoluteStoragePath);
    void addInstalledItem(const QJsonObject, const QString);
    void installedLoadingFinished();
    void isLoadingContentChanged(bool isLoadingContent);

private:
    QVector<ProjectFile> m_screenPlayFiles;
    QUrl m_absoluteStoragePath;
};
