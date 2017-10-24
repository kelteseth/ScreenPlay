#ifndef INSTALLEDLISTMODEL_H
#define INSTALLEDLISTMODEL_H

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
#include <QVector>
#include <QStandardPaths>
#include <QString>
#include <QUrl>
#include <QtConcurrent/QtConcurrent>

class InstalledListModel : public QAbstractListModel {
    Q_OBJECT

public:
    explicit InstalledListModel(QObject* parent = 0);

    // Basic functionality:
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    void append(const QJsonObject, const QString);
    QHash<int, QByteArray> roleNames() const override;

    bool getProjectByAbsoluteStoragePath(QUrl *path, ProjectFile* spf);

    Q_INVOKABLE void loadScreens();
    Q_INVOKABLE QVariantMap get(QString folderId);
    Q_PROPERTY(QUrl absoluteStoragePath READ absoluteStoragePath WRITE setabsoluteStoragePath NOTIFY absoluteStoragePathChanged)

    enum InstalledRole {
        TitleRole,
        PreviewRole,
        FolderIdRole,
        FileIdRole,
        AbsoluteStoragePathRole,
    };
    Q_ENUM(InstalledRole)

    QUrl absoluteStoragePath() const
    {
        return m_absoluteStoragePath;
    }

public slots:
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

private:
    QVector<ProjectFile> m_screenPlayFiles;
    QUrl m_absoluteStoragePath;
};

#endif // INSTALLEDLISTMODEL_H
