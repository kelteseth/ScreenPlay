#include "installedlistmodel.h"

InstalledListModel::InstalledListModel(QObject* parent)
    : QAbstractListModel(parent)
{
}

int InstalledListModel::rowCount(const QModelIndex& parent) const
{
    // For list models only the root node (an invalid parent) should return the
    // list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not
    // become a tree model.
    return _screenPlayFiles.count();
}

QVariant InstalledListModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() < rowCount())
        switch (role) {
        case TitleRole:
            return _screenPlayFiles.at(index.row())._title;
        case ImageRole:
            return _screenPlayFiles.at(index.row())._description;
        default:
            return QVariant();
        }
    return QVariant();
}

QHash<int, QByteArray> InstalledListModel::roleNames() const
{
    static const QHash<int, QByteArray> roles{
        { TitleRole, "title" },
        { ImageRole, "image" },
    };
    return roles;
}

void InstalledListModel::append(const QJsonObject obj)
{
    int row = 0;

    beginInsertRows(QModelIndex(), row, row);

    ScreenPlayFile tmpFile(obj);
    _screenPlayFiles.append(tmpFile);

    endInsertRows();
}

void InstalledListModel::loadDrives()
{
    QString writablePath = QStandardPaths::writableLocation(QStandardPaths::StandardLocation::DataLocation);

    if (!QDir(writablePath).exists()) {
        if (!QDir().mkdir(writablePath)) {
            qWarning("ERROR: Cloud not create install dir");
            return;
        }
    }

    QString tmp(writablePath + "/Installed/");
    QJsonDocument jsonProject;
    QJsonParseError parseError;

    QFileInfoList list = QDir(tmp).entryInfoList(QDir::NoDotAndDotDot | QDir::AllDirs);
    QString tmpPath;

    for (auto&& item : list) {
        tmpPath = tmp + item.baseName() + "/project.json";

        if (!QFile(tmpPath).exists())
            continue;

        QFile projectConfig;
        projectConfig.setFileName(tmpPath);
        projectConfig.open(QIODevice::ReadOnly | QIODevice::Text);
        QString projectConfigData = projectConfig.readAll();
        jsonProject = QJsonDocument::fromJson(projectConfigData.toUtf8(), &parseError);

        if (!(parseError.error == QJsonParseError::NoError))
            continue;

        append(jsonProject.object());
    }
}
