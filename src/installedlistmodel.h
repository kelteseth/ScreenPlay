#ifndef INSTALLEDLISTMODEL_H
#define INSTALLEDLISTMODEL_H

#include <QAbstractListModel>
#include <QByteArray>
#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QList>
#include <QStandardPaths>
#include <QString>
#include <QUrl>
#include <QtConcurrent/QtConcurrent>

class ScreenPlayFile;

class InstalledListModel : public QAbstractListModel {
    Q_OBJECT

public:
    explicit InstalledListModel(QObject* parent = 0);

    // Basic functionality:
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index,
        int role = Qt::DisplayRole) const override;
    void append(const QJsonObject, const QString);
    QHash<int, QByteArray> roleNames() const;

    Q_INVOKABLE void loadScreens();
    Q_INVOKABLE QVariantMap get(QString folderId);
    Q_PROPERTY(QString _screensPath READ name CONSTANT)

    enum InstalledRole {
        TitleRole,
        PreviewRole,
        FolderIdRole,
    };
    Q_ENUM(InstalledRole)

    QString name() const
    {
        return _screensPath;
    }

private:
    QList<ScreenPlayFile> _screenPlayFiles;
    QString _screensPath;
};

class ScreenPlayFile {

public:
    ScreenPlayFile();
    ScreenPlayFile(QJsonObject obj, QString folderName)
    {
        if (obj.contains("description"))
            _description = obj.value("description");

        if (obj.contains("file"))
            _file = obj.value("file");

        if (obj.contains("preview"))
            _preview = obj.value("preview");

        if (obj.contains("title"))
            _title = obj.value("title");

        _folderId = folderName;
    }

    QVariant _description;
    QVariant _file;
    QVariant _preview;
    QVariant _title;
    QString _folderId;
    QUrl _absolutePath;

    QVariantList _tags; //TODO: Implement me!
};

#endif // INSTALLEDLISTMODEL_H
