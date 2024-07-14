// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#pragma once

#include <QAbstractListModel>
#include <QByteArray>
#include <QDir>
#include <QDirIterator>
#include <QFileSystemWatcher>
#include <QHash>
#include <QString>
#include <QVector>
#include <memory>

#include "ScreenPlay/globalvariables.h"
#include "ScreenPlay/settings.h"
#include "ScreenPlayUtil/projectfile.h"

namespace ScreenPlay {

class InstalledListModel : public QAbstractListModel {
    Q_OBJECT
    QML_UNCREATABLE("")

    Q_PROPERTY(int count READ count WRITE setCount NOTIFY countChanged)

public:
    explicit InstalledListModel(
        const std::shared_ptr<GlobalVariables>& globalVariables,
        std::shared_ptr<Settings> settings,
        QObject* parent = nullptr);

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
        LastModified,
        IsNew,
        ContainsAudio,
    };
    Q_ENUM(ScreenPlayItem)

    int count() const
    {
        return m_count;
    }
    Q_INVOKABLE QVariantMap get(const QString& folderName) const;

public slots:

    void loadInstalledContent();
    void append(const QString& projectJsonFilePath);
    void reset();
    void init();
    bool deinstallItemAt(const QString& absoluteStoragePath);

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
    QTimer m_reloadLimiter;
    std::atomic_bool m_isLoading { false };

    const std::shared_ptr<GlobalVariables>& m_globalVariables;
    std::shared_ptr<Settings> m_settings;
};
}
