// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#include "installedlistmodel.h"
#include <ScreenPlayCore/contenttypes.h>

namespace ScreenPlayWorkshop {

InstalledListModel::InstalledListModel(QObject* parent)
    : QAbstractListModel(parent)
{
}

void InstalledListModel::init()
{
    QSettings settings;
    if (!settings.value("ScreenPlayContentPath").isNull()) {
        m_absoluteStoragePath = QUrl::fromUserInput(settings.value("ScreenPlayContentPath").toString());
    }

    loadInstalledContent();
}

void InstalledListModel::init(const QUrl& contentPath)
{
    m_absoluteStoragePath = contentPath;
    loadInstalledContent();
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

    const int row = index.row();
    if (row < 0 || row >= m_screenPlayFiles.count()) {
        return QVariant();
    }

    if (row < rowCount())
        switch (role) {
        case static_cast<int>(ScreenPlayItem::Title):
            return m_screenPlayFiles.at(row).title;
        case static_cast<int>(ScreenPlayItem::Preview):
            return m_screenPlayFiles.at(row).preview;
        case static_cast<int>(ScreenPlayItem::PreviewGIF):
            return m_screenPlayFiles.at(row).previewGIF;
        case static_cast<int>(ScreenPlayItem::PreviewWebP):
            return m_screenPlayFiles.at(row).previewWebP;
        case static_cast<int>(ScreenPlayItem::Type):
            return QVariant::fromValue(m_screenPlayFiles.at(row).type);
        case static_cast<int>(ScreenPlayItem::FolderName):
            return m_screenPlayFiles.at(row).folderName;
        case static_cast<int>(ScreenPlayItem::FileId):
            return m_screenPlayFiles.at(row).file;
        case static_cast<int>(ScreenPlayItem::AbsoluteStoragePath):
            return QUrl::fromLocalFile(m_screenPlayFiles.at(row).projectJsonFilePath.dir().path());
        case static_cast<int>(ScreenPlayItem::PublishedFileID):
            return m_screenPlayFiles.at(row).publishedFileID;
        case static_cast<int>(ScreenPlayItem::Tags):
            return m_screenPlayFiles.at(row).tags;
        case static_cast<int>(ScreenPlayItem::SearchType):
            return QVariant::fromValue(m_screenPlayFiles.at(row).searchType);
        case static_cast<int>(ScreenPlayItem::LastModified):
            return m_screenPlayFiles.at(row).lastModified;
        case static_cast<int>(ScreenPlayItem::IsOnWorkshop): {
            const auto& pfid = m_screenPlayFiles.at(row).publishedFileID;
            if (!pfid.isValid())
                return false;
            // Steam PublishedFileId_t is uint64 – the value may be stored
            // as either a JSON number or a string. toULongLong() handles
            // both representations correctly.
            bool ok = false;
            const auto id = pfid.toULongLong(&ok);
            return ok && id > 0;
        }
        case static_cast<int>(ScreenPlayItem::TypeString):
            return ScreenPlay::ContentTypes::toString(m_screenPlayFiles.at(row).type);
        default:
            return QVariant();
        }
    return QVariant();
}

QHash<int, QByteArray> InstalledListModel::roleNames() const
{
    return {
        { static_cast<int>(ScreenPlayItem::Title), "m_title" },
        { static_cast<int>(ScreenPlayItem::Type), "m_type" },
        { static_cast<int>(ScreenPlayItem::Preview), "m_preview" },
        { static_cast<int>(ScreenPlayItem::PreviewGIF), "m_previewGIF" },
        { static_cast<int>(ScreenPlayItem::PreviewWebP), "m_previewWebP" },
        { static_cast<int>(ScreenPlayItem::FolderName), "m_folderName" },
        { static_cast<int>(ScreenPlayItem::FileId), "m_file" },
        { static_cast<int>(ScreenPlayItem::AbsoluteStoragePath), "m_absoluteStoragePath" },
        { static_cast<int>(ScreenPlayItem::PublishedFileID), "m_publishedFileID" },
        { static_cast<int>(ScreenPlayItem::Tags), "m_tags" },
        { static_cast<int>(ScreenPlayItem::SearchType), "m_searchType" },
        { static_cast<int>(ScreenPlayItem::LastModified), "m_lastModified" },
        { static_cast<int>(ScreenPlayItem::IsOnWorkshop), "m_isOnWorkshop" },
        { static_cast<int>(ScreenPlayItem::TypeString), "m_typeString" },
    };
}

void InstalledListModel::append(const QString& projectJsonFilePath)
{
    beginInsertRows(QModelIndex(), m_screenPlayFiles.size(), m_screenPlayFiles.size());
    using namespace ScreenPlay;
    ProjectFile projectFile;
    projectFile.projectJsonFilePath = QFileInfo(projectJsonFilePath);
    if (!projectFile.init()) {
        qWarning() << "Invalid project at " << projectJsonFilePath;
        return;
    }
    m_screenPlayFiles.append(std::move(projectFile));
    endInsertRows();
}

void InstalledListModel::loadInstalledContent()
{
    if (m_loadContentFutureWatcher.isRunning())
        return;

    // Collect valid paths on a background thread to avoid blocking the UI,
    // then append to the model on the main thread via the watcher's finished signal
    // to keep all QAbstractListModel mutations on the correct thread.
    const auto basePath = m_absoluteStoragePath.toLocalFile();
    m_loadContentFuture = QtConcurrent::run([basePath]() {
        QStringList paths;
        const auto list = QDir(basePath).entryInfoList(QDir::NoDotAndDotDot | QDir::AllDirs);
        for (const auto& item : list) {
            const QString absoluteFilePath = basePath + "/" + item.baseName() + "/project.json";
            if (QFile::exists(absoluteFilePath))
                paths.append(absoluteFilePath);
        }
        return paths;
    });

    QObject::connect(&m_loadContentFutureWatcher, &QFutureWatcher<QStringList>::finished, this, [this]() {
        for (const auto& path : m_loadContentFutureWatcher.result())
            append(path);
        emit installedLoadingFinished();
    }, Qt::SingleShotConnection);

    m_loadContentFutureWatcher.setFuture(m_loadContentFuture);
}

QVariantMap InstalledListModel::get(QString folderName)
{

    if (m_screenPlayFiles.count() == 0)
        return {};

    QVariantMap map;

    for (int i = 0; i < m_screenPlayFiles.count(); i++) {

        if (m_screenPlayFiles[i].folderName == folderName) {
            map.insert("m_title", m_screenPlayFiles[i].title);
            map.insert("m_preview", m_screenPlayFiles[i].preview);
            map.insert("m_previewGIF", m_screenPlayFiles[i].previewGIF);
            map.insert("m_previewWebP", m_screenPlayFiles[i].previewWebP);
            map.insert("m_file", m_screenPlayFiles[i].file);
            map.insert("m_type", QVariant::fromValue(m_screenPlayFiles[i].type));
            map.insert("m_absoluteStoragePath", QUrl::fromLocalFile(m_screenPlayFiles[i].projectJsonFilePath.dir().path()));
            map.insert("m_publishedFileID", m_screenPlayFiles[i].publishedFileID);
            return map;
        }
    }

    return map;
}

void InstalledListModel::reset()
{
    beginResetModel();
    m_screenPlayFiles.clear();
    m_screenPlayFiles.squeeze();
    endResetModel();
    loadInstalledContent();
}

void InstalledListModel::sort(SortField field, bool ascending)
{
    if (m_screenPlayFiles.isEmpty())
        return;

    beginResetModel();
    std::sort(m_screenPlayFiles.begin(), m_screenPlayFiles.end(),
        [field, ascending](const ScreenPlay::ProjectFile& a, const ScreenPlay::ProjectFile& b) {
            if (field == SortField::LastModified) {
                return ascending ? a.lastModified < b.lastModified
                                 : a.lastModified > b.lastModified;
            }
            // SortField::Title
            const auto cmp = a.title.compare(b.title, Qt::CaseInsensitive);
            return ascending ? cmp < 0 : cmp > 0;
        });
    endResetModel();
}
}

#include "moc_installedlistmodel.cpp"
