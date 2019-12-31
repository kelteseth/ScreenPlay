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
#include "globalvariables.h"
#include "util.h"

#include <memory>

namespace ScreenPlay {

using std::shared_ptr;

class InstalledListModel : public QAbstractListModel {
    Q_OBJECT

    Q_PROPERTY(int count READ count WRITE setCount NOTIFY countChanged)

public:
    explicit InstalledListModel(
        const shared_ptr<GlobalVariables>& globalVariables,
        QObject* parent = nullptr);

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
    void addInstalledItem(const QJsonObject, const QString);
    void installedLoadingFinished();
    void isLoadingContentChanged(bool isLoadingContent);
    void countChanged(int count);

private:
    QFileSystemWatcher m_fileSystemWatcher;
    QVector<ProjectFile> m_screenPlayFiles;
    int m_count{0};

    const shared_ptr<GlobalVariables>& m_globalVariables;
};
}
