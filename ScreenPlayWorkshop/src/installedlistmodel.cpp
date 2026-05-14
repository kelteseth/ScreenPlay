// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#include "installedlistmodel.h"
#include <ScreenPlayCore/contenttypes.h>

#include <QLoggingCategory>

Q_LOGGING_CATEGORY(workshopInstalled, "screenplay.workshop.installed")

namespace ScreenPlayWorkshop {

/*!
    \class ScreenPlayWorkshop::InstalledListModel
    \inmodule ScreenPlayWorkshop
    \brief Lists all installed items from a given path.
*/

/*!
    \fn InstalledListModel::InstalledListModel(QObject *parent)
    \brief Constructs an empty InstalledListModel with the given \a parent.
           Call \c init() or \c init(const QUrl &) to populate the model.
*/
InstalledListModel::InstalledListModel(QObject* parent)
    : QAbstractListModel(parent)
{
}

/*!
    \fn void InstalledListModel::init()
    \brief Initialises the model using the content path stored in QSettings
           (\c ScreenPlayContentPath). Calls \c loadInstalledContent() to
           scan for installed items.
*/
void InstalledListModel::init()
{
    QSettings settings;
    if (!settings.value("ScreenPlayContentPath").isNull()) {
        m_absoluteStoragePath = QUrl::fromUserInput(settings.value("ScreenPlayContentPath").toString());
    }

    loadInstalledContent();
}

/*!
    \fn void InstalledListModel::init(const QUrl &contentPath)
    \brief Initialises the model with an explicit \a contentPath and calls
           \c loadInstalledContent() to populate the list.
*/
void InstalledListModel::init(const QUrl& contentPath)
{
    m_absoluteStoragePath = contentPath;
    loadInstalledContent();
}

/*!
    \fn int InstalledListModel::rowCount(const QModelIndex &parent) const
    \brief Returns the number of installed items in the model.
           Returns 0 for any valid \a parent (flat list).
*/
int InstalledListModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;

    return m_screenPlayFiles.count();
}

/*!
    \fn QVariant InstalledListModel::data(const QModelIndex &index, int role) const
    \brief Returns the data for the item at \a index under the given \a role.
           Returns an invalid QVariant for out-of-bounds indices or unknown roles.
*/
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

/*!
    \fn QHash<int, QByteArray> InstalledListModel::roleNames() const
    \brief Returns the role-name map used by QML delegates to access item
           properties such as \c m_title, \c m_preview, and \c m_type.
*/
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

/*!
    \fn void InstalledListModel::append(const QString &projectJsonFilePath)
    \brief Parses the project at \a projectJsonFilePath and appends it to the
           model. Logs a warning and returns early if the project is invalid.
*/
void InstalledListModel::append(const QString& projectJsonFilePath)
{
    using namespace ScreenPlay;
    ProjectFile projectFile;
    projectFile.projectJsonFilePath = QFileInfo(projectJsonFilePath);
    if (!projectFile.init()) {
        qCWarning(workshopInstalled) << "Invalid project at " << projectJsonFilePath;
        return;
    }
    beginInsertRows(QModelIndex(), m_screenPlayFiles.size(), m_screenPlayFiles.size());
    m_screenPlayFiles.append(std::move(projectFile));
    endInsertRows();
}

/*!
    \fn void InstalledListModel::loadInstalledContent()
    \brief Scans \c m_absoluteStoragePath for \c project.json files on a
           background thread via QtConcurrent and appends each found item on
           the main thread. Emits \c installedLoadingFinished when done.
           Does nothing if a scan is already in progress.
*/
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
        emit installedLoadingFinished(); }, Qt::SingleShotConnection);

    m_loadContentFutureWatcher.setFuture(m_loadContentFuture);
}

/*!
    \fn QVariantMap InstalledListModel::get(QString folderName)
    \brief Finds the installed item whose folder name matches \a folderName
           and returns its properties as a map. Returns an empty map if no
           matching item is found.
*/
QVariantMap InstalledListModel::get(QString folderName)
{
    const auto it = std::ranges::find_if(m_screenPlayFiles,
        [&folderName](const ScreenPlay::ProjectFile& f) { return f.folderName == folderName; });

    if (it == m_screenPlayFiles.end())
        return {};

    return {
        { "m_title", it->title },
        { "m_preview", it->preview },
        { "m_previewGIF", it->previewGIF },
        { "m_previewWebP", it->previewWebP },
        { "m_file", it->file },
        { "m_type", QVariant::fromValue(it->type) },
        { "m_absoluteStoragePath", QUrl::fromLocalFile(it->projectJsonFilePath.dir().path()) },
        { "m_publishedFileID", it->publishedFileID },
    };
}

/*!
    \fn void InstalledListModel::reset()
    \brief Clears all items from the model and reloads installed content
           from \c m_absoluteStoragePath.
*/
void InstalledListModel::reset()
{
    beginResetModel();
    m_screenPlayFiles.clear();
    m_screenPlayFiles.squeeze();
    endResetModel();
    loadInstalledContent();
}

/*!
    \fn void InstalledListModel::sort(SortField field, bool ascending)
    \brief Sorts the model in-place by \a field (\c Title or \c LastModified).
           Pass \c true for \a ascending for A→Z / oldest-first ordering.
*/
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
