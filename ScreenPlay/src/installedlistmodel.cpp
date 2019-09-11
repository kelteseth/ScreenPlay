#include "installedlistmodel.h"

/*!
    \class Installed List Model
    \brief Lists all installed wallpapers, widgets etc. from a given Path

*/

namespace ScreenPlay {

InstalledListModel::InstalledListModel(
    const shared_ptr<GlobalVariables>& globalVariables,
    QObject* parent)
    : QAbstractListModel(parent)
    , m_globalVariables { globalVariables }
{
    QObject::connect(this, &InstalledListModel::addInstalledItem,
        this, &InstalledListModel::append, Qt::QueuedConnection);
}

int InstalledListModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;

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

void InstalledListModel::append(const QJsonObject& obj, const QString& folderName)
{

    beginInsertRows(QModelIndex(), m_screenPlayFiles.size(), m_screenPlayFiles.size());

    m_screenPlayFiles.append(ProjectFile(obj, folderName, m_globalVariables->localStoragePath()));
    setCount((m_count + 1));
    endInsertRows();
}

void InstalledListModel::loadInstalledContent()
{

    QtConcurrent::run([this]() {
        QJsonDocument jsonProject;
        QJsonParseError parseError;

        QFileInfoList list = QDir(m_globalVariables->localStoragePath().toLocalFile()).entryInfoList(QDir::NoDotAndDotDot | QDir::AllDirs);
        QString tmpPath;

        for (auto&& item : list) {
            tmpPath = m_globalVariables->localStoragePath().toLocalFile() + "/" + item.baseName() + "/project.json";

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

            if (jsonProject.isEmpty() || jsonProject.object().empty())
                continue;

            if (!obj.contains("file") || !obj.contains("type"))
                continue;

            QStringList availableTypes {
                "qmlWallpaper",
                "htmlWallpaper",
                "videoWallpaper",
                "godotWallpaper",

                "qmlWidget",
                "htmlWidget",
                "standaloneWidget"
            };

            if (availableTypes.contains(obj.value("type").toString()))
                emit addInstalledItem(obj, item.baseName());
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

void InstalledListModel::reset()
{
    beginResetModel();
    m_screenPlayFiles.clear();
    m_screenPlayFiles.squeeze();
    setCount(0);
    endResetModel();
}

void InstalledListModel::init()
{
    if (!m_fileSystemWatcher.addPath(m_globalVariables->localStoragePath().toLocalFile())) {
        qWarning() << "Could not setup file system watcher for changed files with path: " << m_globalVariables->localStoragePath().toLocalFile();
    }

    QObject::connect(&m_fileSystemWatcher, &QFileSystemWatcher::directoryChanged, this, [this](const QString& path) {
        qDebug() << path;
        reset();
        loadInstalledContent();
    });
    loadInstalledContent();
}
}
