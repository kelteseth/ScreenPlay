// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#include "installedlistmodel.h"

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
        { static_cast<int>(ScreenPlayItem::FolderName), "m_folderName" },
        { static_cast<int>(ScreenPlayItem::FileId), "m_file" },
        { static_cast<int>(ScreenPlayItem::AbsoluteStoragePath), "m_absoluteStoragePath" },
        { static_cast<int>(ScreenPlayItem::PublishedFileID), "m_publishedFileID" },
        { static_cast<int>(ScreenPlayItem::Tags), "m_tags" },
        { static_cast<int>(ScreenPlayItem::SearchType), "m_searchType" },
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

    m_loadContentFuture = QtConcurrent::run([this]() {
        QFileInfoList list = QDir(m_absoluteStoragePath.toLocalFile()).entryInfoList(QDir::NoDotAndDotDot | QDir::AllDirs);

        for (const auto& item : list) {
            const QString absoluteFilePath = m_absoluteStoragePath.toLocalFile() + "/" + item.baseName() + "/project.json";

            if (!QFile::exists(absoluteFilePath))
                continue;

            append(absoluteFilePath);
        }

        emit installedLoadingFinished();
    });
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
}

#include "moc_installedlistmodel.cpp"
