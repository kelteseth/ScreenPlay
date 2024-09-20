// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#include "ScreenPlay/installedlistmodel.h"
#include "ScreenPlay/CMakeVariables.h"
#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QGuiApplication>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>
#include <QtConcurrent/QtConcurrent>

#include <QUrl>

namespace ScreenPlay {

/*!
    \class ScreenPlay::InstalledListModel
    \inmodule ScreenPlay
    \brief Lists all installed wallpapers, widgets etc. from a given Path.

    Currently we only support one path for the user content. Via a QFileSystemWatcher we
    automatically reload the list. Dynamic insert and remove is not yet implemented.
    Otherwhise it is a regular QAbstractListModel based list model.
*/

/*!
  \brief Constructor.
*/
InstalledListModel::InstalledListModel(
    const std::shared_ptr<GlobalVariables>& globalVariables,
    std::shared_ptr<Settings> settings,
    QObject* parent)
    : QAbstractListModel(parent)
    , m_globalVariables { globalVariables }
    , m_settings { settings }
{
}

/*!
    \brief Init function that needs to be called after the constructor because we need the GlobalVariables to finish loading.
*/
void InstalledListModel::init()
{
    QString projectsPath = m_globalVariables->localStoragePath().toLocalFile();
    QDirIterator projectFilesIter(projectsPath, { "*.qml", "*.html", "*.css", "*.js", "*.png", "project.json" }, QDir::Files | QDir::NoSymLinks, QDirIterator::Subdirectories);
    while (projectFilesIter.hasNext()) {
        m_fileSystemWatcher.addPath(projectFilesIter.next());
    }
    m_reloadLimiter.setInterval(500);
    m_reloadLimiter.setSingleShot(true);
    QObject::connect(&m_reloadLimiter, &QTimer::timeout, this, [this]() {
        reset();
    });

    auto restartTimer = [this]() {
        m_reloadLimiter.start();
    };

    QObject::connect(&m_fileSystemWatcher, &QFileSystemWatcher::directoryChanged, this, restartTimer);
    QObject::connect(&m_fileSystemWatcher, &QFileSystemWatcher::fileChanged, this, restartTimer);
}

/*!
    \brief Deleted the item from the local storage and removes it from the
    installed list. We wait for the qml engine to free all resources before
    we proceed. This like the preview.gif will be in use when clicking on an item
*/
bool InstalledListModel::deinstallItemAt(const QString& absoluteStoragePath)
{
    const QString path = Util().toLocal(absoluteStoragePath);
    int index = -1;
    for (int i = 0; i < m_screenPlayFiles.size(); ++i) {
        if (m_screenPlayFiles.at(i).projectJsonFilePath.path() == path) {
            index = i;
            break;
        }
    }

    if (index < 0 || index >= m_screenPlayFiles.count()) {
        qWarning() << "Remove folder error, invalid index " << index;
        return false;
    }

    beginRemoveRows(QModelIndex(), index, index);
    m_screenPlayFiles.removeAt(index);
    endRemoveRows();

    QTimer::singleShot(1000, this, [this, path]() {
        QDir dir(path);
        bool success = true;
        if (!dir.exists()) {
            qWarning() << "Directory does not exist!" << dir;
            return;
        }

        // We must pause the QFileSystemWatcher to not trigger
        // a reload for every removed file
        m_fileSystemWatcher.blockSignals(true);
        for (auto& item : dir.entryInfoList(QDir::Files)) {
            if (!QFile::remove(item.absoluteFilePath())) {
                qWarning() << "Unable to remove file:" << item;
                success = false;
                break;
            }
        }

        if (!success) {
            qWarning() << "Could not remove folder content at: " << path;
            loadInstalledContent();
        }

        if (!dir.rmdir(path)) {
            qWarning() << "Could not remove folder at: " << path;
            return;
        }

        // Add delay to the watcher, because it was trigger by
        // something when enabling after the removal.
        QTimer::singleShot(3000, this, [this]() {
            m_fileSystemWatcher.blockSignals(false);
        });
    });
    return true;
}

/*!
    \brief Returns the length of the installed items.
*/
int InstalledListModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;

    return m_screenPlayFiles.count();
}

/*!
    \brief Retruns the data member of the ProjectFile.
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
        case static_cast<int>(ScreenPlayItem::IsNew):
            return m_screenPlayFiles.at(row).isNew;
        case static_cast<int>(ScreenPlayItem::ContainsAudio):
            return m_screenPlayFiles.at(row).containsAudio;
        case static_cast<int>(ScreenPlayItem::LastModified):
            return m_screenPlayFiles.at(row).lastModified;
        case static_cast<int>(ScreenPlayItem::SearchType):
            return QVariant::fromValue(m_screenPlayFiles.at(row).searchType);
        }

    qWarning() << "Unable to fetch value for row type:" << role;
    return QVariant();
}

/*!
    \brief Returns the variable names for QML.
*/
QHash<int, QByteArray> InstalledListModel::roleNames() const
{
    return {
        { static_cast<int>(ScreenPlayItem::Title), "title" },
        { static_cast<int>(ScreenPlayItem::Type), "type" },
        { static_cast<int>(ScreenPlayItem::Preview), "preview" },
        { static_cast<int>(ScreenPlayItem::PreviewGIF), "previewGIF" },
        { static_cast<int>(ScreenPlayItem::FolderName), "folderName" },
        { static_cast<int>(ScreenPlayItem::FileId), "file" },
        { static_cast<int>(ScreenPlayItem::AbsoluteStoragePath), "absoluteStoragePath" },
        { static_cast<int>(ScreenPlayItem::PublishedFileID), "publishedFileID" },
        { static_cast<int>(ScreenPlayItem::Tags), "tags" },
        { static_cast<int>(ScreenPlayItem::SearchType), "searchType" },
        { static_cast<int>(ScreenPlayItem::IsNew), "isNew" },
        { static_cast<int>(ScreenPlayItem::ContainsAudio), "containsAudio" },
        { static_cast<int>(ScreenPlayItem::LastModified), "lastModified" }
    };
}

/*!
    \brief Append an ProjectFile to the list.
*/
void InstalledListModel::append(const QString& projectJsonFilePath)
{
    beginInsertRows(QModelIndex(), m_screenPlayFiles.size(), m_screenPlayFiles.size());
    ProjectFile projectFile;
    projectFile.projectJsonFilePath = QFileInfo(projectJsonFilePath);
    if (!projectFile.init()) {
        // qWarning() << "Invalid project at " << projectJsonFilePath;
        return;
    }
    m_screenPlayFiles.append(std::move(projectFile));
    endInsertRows();
}

/*!
    \brief Loads all installed and default content.
           - Skips if the loadContentFuture is already running.
           - Skips projects.json without a "type" field.
*/
void InstalledListModel::loadInstalledContent()
{
    if (m_isLoading) {
        qInfo() << "loadInstalledContent is already running. Skip.";
        return;
    }
    m_isLoading = true;
    auto unused = QtConcurrent::run([this]() {
        int counter = 0;
        auto loadFiles = [this, &counter](const QString path) { // capture counter by reference
            const QFileInfoList list = QDir(path).entryInfoList(QDir::NoDotAndDotDot | QDir::AllDirs);

            for (const QFileInfo& item : list) {
                const QString absoluteFilePath = path + "/" + item.baseName() + "/project.json";

                if (!QFile::exists(absoluteFilePath))
                    continue;

                append(absoluteFilePath);
                counter += 1;
            }
        };
        QString defaultContentPath;
        if (SCREENPLAY_DEPLOY_VERSION) {
            defaultContentPath = QGuiApplication::instance()->applicationDirPath() + "/Content";
        } else {
            defaultContentPath = QString(SCREENPLAY_SOURCE_DIR) + "/Content";
        }
        if (m_settings->showDefaultContent()) {
            loadFiles(defaultContentPath);
        }
        const QString installedPath = m_globalVariables->localStoragePath().toLocalFile();
        loadFiles(installedPath);

        setCount(counter);
        emit installedLoadingFinished();
        m_isLoading = false;
    });
}

/*!
    \brief Used for receiving values from qml. One must add all new fields
           when adding new roles to this model.
*/
QVariantMap InstalledListModel::get(const QString& folderName) const
{
    if (folderName.isEmpty()) {
        qCritical() << "Invalid (empty) folder name";
        return {};
    }

    if (m_screenPlayFiles.count() == 0)
        return {};

    for (const auto& item : m_screenPlayFiles) {
        if (item.folderName == folderName) {
            QVariantMap map;
            map.insert("title", item.title);
            map.insert("preview", item.preview);
            map.insert("previewGIF", item.previewGIF);
            map.insert("file", item.file);
            map.insert("type", QVariant::fromValue(item.type));
            map.insert("absoluteStoragePath", QUrl::fromLocalFile(item.projectJsonFilePath.dir().path()).toString());
            map.insert("publishedFileID", item.publishedFileID);
            map.insert("isNew", item.isNew);
            map.insert("containsAudio", item.containsAudio);
            map.insert("lastModified", item.lastModified);
            return map;
        }
    }

    qWarning() << "Installed item not found for: " << folderName;

    return {};
}

/*!
    \brief Removes all entires and loads it again.
*/
void InstalledListModel::reset()
{
    if (m_isLoading) {
        qInfo() << "loadInstalledContent is already running. Skip.";
        return;
    }
    beginResetModel();
    m_screenPlayFiles.clear();
    m_screenPlayFiles.squeeze();
    endResetModel();
    loadInstalledContent();
}

}

#include "moc_installedlistmodel.cpp"
