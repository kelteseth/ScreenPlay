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
    void append(const QJsonObject);
    QHash<int, QByteArray> roleNames() const;

    Q_INVOKABLE void loadDrives();

    enum InstalledRole {
        TitleRole,
        ImageRole,

    };
    Q_ENUM(InstalledRole)

private:
    QList<ScreenPlayFile> _screenPlayFiles;
};

class ScreenPlayFile {

public:
    ScreenPlayFile();
    ScreenPlayFile(QJsonObject obj)
    {
        if (obj.contains("description"))
            _description = obj.value("description");

        if (obj.contains("file"))
            _file = obj.value("file");

        if (obj.contains("preview"))
            _preview = obj.value("preview");

        if (obj.contains("title"))
            _title = obj.value("title");
    }

    QVariant _description = "as";
    QVariant _file;
    QVariant _preview;
    QVariant _title= "aass";
    QUrl _absolutePath;

    QVariantList _tags; //TODO: Implement me!
};

#endif // INSTALLEDLISTMODEL_H
