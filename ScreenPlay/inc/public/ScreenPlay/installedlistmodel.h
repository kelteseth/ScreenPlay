// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#pragma once

#include <QAbstractListModel>
#include <QByteArray>
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileSystemWatcher>
#include <QFuture>
#include <QFutureWatcher>
#include <QHash>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>
#include <QString>
#include <QUrl>
#include <QVector>
#include <QtConcurrent/QtConcurrent>

#include "ScreenPlay/globalvariables.h"
#include "ScreenPlay/profilelistmodel.h"
#include "ScreenPlay/util.h"
#include "ScreenPlayUtil/projectfile.h"

#include <memory>

namespace ScreenPlay {

class InstalledListModel : public QAbstractListModel {
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(int count READ count WRITE setCount NOTIFY countChanged)

public:
    explicit InstalledListModel(
        const std::shared_ptr<GlobalVariables>& globalVariables,
        QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    enum class ScreenPlayItem {
        Title = Qt::UserRole,
        Type,
        Preview,
        PreviewGIF,
        FolderId,
        FileId,
        AbsoluteStoragePath,
        PublishedFileID,
        Tags,
        SearchType,
        LastModified,
        IsNew,
    };
    Q_ENUM(ScreenPlayItem)

    int count() const
    {
        return m_count;
    }

public slots:
    QVariantMap get(const QString& folderName) const;

    void loadInstalledContent();
    void append(const QJsonObject&, const QString&, const bool isNew, const QDateTime& lastModified);
    void reset();
    void init();
    void deinstallItemAt(const QString& absoluteStoragePath);

    void setCount(int count)
    {
        if (m_count == count)
            return;

        m_count = count;
        emit countChanged(m_count);
    }

signals:
    void installedLoadingFinished();
    void countChanged(int count);

private:
    QFileSystemWatcher m_fileSystemWatcher;
    QVector<ProjectFile> m_screenPlayFiles;
    int m_count { 0 };
    std::atomic_bool m_isLoading { false };

    const std::shared_ptr<GlobalVariables>& m_globalVariables;
};
}
