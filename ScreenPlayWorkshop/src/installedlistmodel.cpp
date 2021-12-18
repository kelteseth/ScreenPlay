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
            return m_screenPlayFiles.at(row).m_title;
        case static_cast<int>(ScreenPlayItem::Preview):
            return m_screenPlayFiles.at(row).m_preview;
        case static_cast<int>(ScreenPlayItem::PreviewGIF):
            return m_screenPlayFiles.at(row).m_previewGIF;
        case static_cast<int>(ScreenPlayItem::Type):
            return QVariant::fromValue(m_screenPlayFiles.at(row).m_type);
        case static_cast<int>(ScreenPlayItem::FolderId):
            return m_screenPlayFiles.at(row).m_folderId;
        case static_cast<int>(ScreenPlayItem::FileId):
            return m_screenPlayFiles.at(row).m_file;
        case static_cast<int>(ScreenPlayItem::AbsoluteStoragePath):
            return m_screenPlayFiles.at(row).m_absoluteStoragePath;
        case static_cast<int>(ScreenPlayItem::PublishedFileID):
            return m_screenPlayFiles.at(row).m_publishedFileID;
        case static_cast<int>(ScreenPlayItem::Tags):
            return m_screenPlayFiles.at(row).m_tags;
        case static_cast<int>(ScreenPlayItem::SearchType):
            return QVariant::fromValue(m_screenPlayFiles.at(row).m_searchType);
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
        { static_cast<int>(ScreenPlayItem::FolderId), "m_folderId" },
        { static_cast<int>(ScreenPlayItem::FileId), "m_file" },
        { static_cast<int>(ScreenPlayItem::AbsoluteStoragePath), "m_absoluteStoragePath" },
        { static_cast<int>(ScreenPlayItem::PublishedFileID), "m_publishedFileID" },
        { static_cast<int>(ScreenPlayItem::Tags), "m_tags" },
        { static_cast<int>(ScreenPlayItem::SearchType), "m_searchType" },
    };
}

void InstalledListModel::append(const QJsonObject& obj, const QString& folderName, const QDateTime& lastModified)
{
    beginInsertRows(QModelIndex(), m_screenPlayFiles.size(), m_screenPlayFiles.size());
    m_screenPlayFiles.append(ScreenPlay::ProjectFile(obj, folderName, m_absoluteStoragePath, false, lastModified));
    endInsertRows();
}

void InstalledListModel::loadInstalledContent()
{
    if(m_loadContentFutureWatcher.isRunning())
        return;

   m_loadContentFuture =  QtConcurrent::run([this]() {
        QFileInfoList list = QDir(m_absoluteStoragePath.toLocalFile()).entryInfoList(QDir::NoDotAndDotDot | QDir::AllDirs);

        for (const auto& item : list) {

            const QString absoluteFilePath = m_absoluteStoragePath.toLocalFile() + "/" + item.baseName() + "/project.json";

            if (!QFile::exists(absoluteFilePath))
                continue;

            if (auto obj = ScreenPlayUtil::openJsonFileToObject(absoluteFilePath)) {

                if (obj->isEmpty())
                    continue;

                if (!obj->contains("type"))
                    continue;

                if (obj->contains("workshopid"))
                    continue;

                if (ScreenPlayUtil::getAvailableTypes().contains(obj->value("type").toString(), Qt::CaseSensitivity::CaseInsensitive))
                    append(*obj, item.baseName(), item.lastModified());
            }
        }

        emit installedLoadingFinished();
    });
    m_loadContentFutureWatcher.setFuture(m_loadContentFuture);
}

QVariantMap InstalledListModel::get(QString folderId)
{

    if (m_screenPlayFiles.count() == 0)
        return {};

    QVariantMap map;

    for (int i = 0; i < m_screenPlayFiles.count(); i++) {

        if (m_screenPlayFiles[i].m_folderId == folderId) {
            map.insert("m_title", m_screenPlayFiles[i].m_title);
            map.insert("m_preview", m_screenPlayFiles[i].m_preview);
            map.insert("m_previewGIF", m_screenPlayFiles[i].m_previewGIF);
            map.insert("m_file", m_screenPlayFiles[i].m_file);
            map.insert("m_type", QVariant::fromValue(m_screenPlayFiles[i].m_type));
            map.insert("m_absoluteStoragePath", m_screenPlayFiles[i].m_absoluteStoragePath);
            map.insert("m_publishedFileID", m_screenPlayFiles[i].m_publishedFileID);
            return map;
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
    loadInstalledContent();
}
}
