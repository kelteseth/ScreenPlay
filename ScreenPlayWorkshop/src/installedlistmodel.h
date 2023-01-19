// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
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
#include <QSettings>
#include <QStandardPaths>
#include <QString>
#include <QUrl>
#include <QVariant>
#include <QVariantList>
#include <QVector>
#include <vector>
#include <QtConcurrent/QtConcurrent>
#include <QtQml>

#include "ScreenPlayUtil/projectfile.h"

/*!
    \class Installed List Model
    \brief Lists all installed items from a given Path

*/
namespace ScreenPlayWorkshop {

class InstalledListModel : public QAbstractListModel {
    Q_OBJECT

    QML_ELEMENT
    Q_PROPERTY(QUrl absoluteStoragePath READ absoluteStoragePath WRITE setAbsoluteStoragePath NOTIFY absoluteStoragePathChanged)

public:
    explicit InstalledListModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    enum class ScreenPlayItem {
        Title = Qt::UserRole,
        Type,
        Preview,
        PreviewGIF,
        FolderName,
        FileId,
        AbsoluteStoragePath,
        PublishedFileID,
        Tags,
        SearchType,
    };
    Q_ENUM(ScreenPlayItem)

    QUrl absoluteStoragePath() const
    {
        return m_absoluteStoragePath;
    }
    void init();

public slots:
    void loadInstalledContent();
    QVariantMap get(QString folderName);
    void append(const QString& projectJsonFilePath);
    void setAbsoluteStoragePath(QUrl absoluteStoragePath)
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
    QVector<ScreenPlay::ProjectFile> m_screenPlayFiles;
    QUrl m_absoluteStoragePath;
    QFuture<void> m_loadContentFuture;
    QFutureWatcher<void> m_loadContentFutureWatcher;
};

}
