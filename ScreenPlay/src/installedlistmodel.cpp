#include "installedlistmodel.h"

InstalledListModel::InstalledListModel(QObject* parent)
    : QAbstractListModel(parent)
{
    QObject::connect(this, &InstalledListModel::addInstalledItem,
        this, &InstalledListModel::append, Qt::QueuedConnection);
}

int InstalledListModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)
    return m_screenPlayFiles.count();
}

QVariant InstalledListModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    int row = index.row();
    if (row < 0 || row >= m_screenPlayFiles.count()) {
        return QVariant();
    }

    if (index.row() < rowCount())
        switch (role) {
        case TitleRole:
            return m_screenPlayFiles.at(index.row()).m_title;
        case PreviewRole:
            return m_screenPlayFiles.at(index.row()).m_preview;
        case PreviewGIFRole:
            return m_screenPlayFiles.at(index.row()).m_previewGIF;
        case TypeRole:
            return m_screenPlayFiles.at(index.row()).m_type;
        case FolderIdRole:
            return m_screenPlayFiles.at(index.row()).m_folderId;
        case FileIdRole:
            return m_screenPlayFiles.at(index.row()).m_file;
        case AbsoluteStoragePathRole:
            return m_screenPlayFiles.at(index.row()).m_absoluteStoragePath;
        case WorkshopIDRole:
            return m_screenPlayFiles.at(index.row()).m_workshopID;
        default:
            return QVariant();
        }
    return QVariant();
}

QHash<int, QByteArray> InstalledListModel::roleNames() const
{
    static const QHash<int, QByteArray> roles {
        { TitleRole, "screenTitle" },
        { TypeRole, "screenType" },
        { PreviewRole, "screenPreview" },
        { PreviewGIFRole, "screenPreviewGIF" },
        { FolderIdRole, "screenFolderId" },
        { FileIdRole, "screenFile" },
        { AbsoluteStoragePathRole, "screenAbsoluteStoragePath" },
        { WorkshopIDRole, "screenWorkshopID" },
    };
    return roles;
}

bool InstalledListModel::getProjectByAbsoluteStoragePath(QUrl* path, ProjectFile* spf)
{

    for (int i = 0; i < m_screenPlayFiles.count(); ++i) {
        if (m_screenPlayFiles.at(i).m_absoluteStoragePath == *path) {
            *spf = m_screenPlayFiles.at(i);
            return true;
        }
    }

    return false;
}

void InstalledListModel::append(const QJsonObject obj, const QString folderName)
{

    beginInsertRows(QModelIndex(), m_screenPlayFiles.size(), m_screenPlayFiles.size());

    ProjectFile tmpFile(obj, folderName, m_absoluteStoragePath);
    m_screenPlayFiles.append(tmpFile);

    endInsertRows();
}

void InstalledListModel::loadInstalledContent()
{

    QtConcurrent::run([this]() {
        QJsonDocument jsonProject;
        QJsonParseError parseError;

        QFileInfoList list = QDir(m_absoluteStoragePath.toLocalFile()).entryInfoList(QDir::NoDotAndDotDot | QDir::AllDirs);
        QString tmpPath;

        for (auto&& item : list) {
            tmpPath = m_absoluteStoragePath.toLocalFile() + "/" + item.baseName() + "/project.json";

            if (!QFile(tmpPath).exists())
                continue;

            QFile projectConfig;
            projectConfig.setFileName(tmpPath);
            projectConfig.open(QIODevice::ReadOnly | QIODevice::Text);
            QString projectConfigData = projectConfig.readAll();
            jsonProject = QJsonDocument::fromJson(projectConfigData.toUtf8(), &parseError);
            QJsonObject obj = jsonProject.object();

            if (!(parseError.error == QJsonParseError::NoError))
                continue;

            if (jsonProject.object().value("type").toString() == "scene")
                continue;

            if (jsonProject.isEmpty())
                continue;

            if (jsonProject.object().empty())
                continue;

            //Some settings dont have a file type
            QString fileEnding;
            if (obj.contains("file")) {
                fileEnding = obj.value("file").toString();
                if (!obj.contains("type")) {
                    obj.insert("type", "video");
                }

                if (fileEnding.endsWith(".webm") || (obj.value("type").toString() == "qmlScene" || fileEnding.endsWith(".html")))
                    emit addInstalledItem(obj, item.baseName());

                if (obj.value("type") == "qmlWidget" || obj.value("type") == "standalonewidget")
                    emit addInstalledItem(obj, item.baseName());
            }
        }

        emit installedLoadingFinished();
    });

}

QVariantMap InstalledListModel::get(QString folderId)
{

    QVariantMap map;
    if (m_screenPlayFiles.count() == 0)
        return map;

    for (int i = 0; i < m_screenPlayFiles.count(); i++) {

        if (m_screenPlayFiles[i].m_folderId == folderId) {
            map.insert("screenTitle", m_screenPlayFiles[i].m_title);
            map.insert("screenPreview", m_screenPlayFiles[i].m_preview);
            map.insert("screenPreviewGIF", m_screenPlayFiles[i].m_previewGIF);
            map.insert("screenFile", m_screenPlayFiles[i].m_file);
            map.insert("screenType", m_screenPlayFiles[i].m_type);
            map.insert("screenAbsoluteStoragePath", m_screenPlayFiles[i].m_absoluteStoragePath);
            map.insert("screenWorkshopID", m_screenPlayFiles[i].m_workshopID);
        }
    }

    return map;
}

int InstalledListModel::getAmountItemLoaded()
{
    return m_screenPlayFiles.count();
}

void InstalledListModel::reset()
{
    beginResetModel();
    m_screenPlayFiles.clear();
    m_screenPlayFiles.squeeze();
    endResetModel();
}
