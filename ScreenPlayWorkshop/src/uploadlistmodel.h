// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#pragma once

#include <QAbstractListModel>
#include <QString>
#include <QUrl>
#include <QVector>

#include "ScreenPlayCore/steamenumsgenerated.h"
#include "steamworkshopitem.h"

namespace ScreenPlayWorkshop {

class UploadListModel : public QAbstractListModel {
    Q_OBJECT

public:
    UploadListModel() { }

    enum class UploadListModelRole {
        NameRole = Qt::UserRole + 1,
        AbsolutePathRole,
        UploadProgressRole,
        AbsolutePreviewImagePath,
        Status,
        UploadState,
    };
    Q_ENUM(UploadListModelRole)

    int rowCount(const QModelIndex& parent = QModelIndex()) const override
    {
        if (parent.isValid())
            return 0;

        return m_uploadListModelItems.size();
    }

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override
    {
        if (!index.isValid())
            return QVariant();

        int row = index.row();
        if (row < 0 || row >= m_uploadListModelItems.size()) {
            return QVariant();
        }

        auto roleType = static_cast<UploadListModelRole>(role);

        if (row < rowCount())
            switch (roleType) {
            case UploadListModelRole::NameRole:
                return m_uploadListModelItems.at(row)->name();
            case UploadListModelRole::AbsolutePathRole:
                return m_uploadListModelItems.at(row)->absolutePath();
            case UploadListModelRole::UploadProgressRole:
                return m_uploadListModelItems.at(row)->uploadProgress();
            case UploadListModelRole::AbsolutePreviewImagePath:
                return m_uploadListModelItems.at(row)->absolutePreviewImagePath();
            case UploadListModelRole::Status:
                return static_cast<int>(m_uploadListModelItems.at(row)->status());
            case UploadListModelRole::UploadState:
                return static_cast<int>(m_uploadListModelItems.at(row)->uploadState());
            }
        return QVariant();
    }

    QHash<int, QByteArray> roleNames() const override
    {
        return {
            { static_cast<int>(UploadListModelRole::NameRole), "m_name" },
            { static_cast<int>(UploadListModelRole::AbsolutePathRole), "m_absolutePath" },
            { static_cast<int>(UploadListModelRole::UploadProgressRole), "m_uploadProgress" },
            { static_cast<int>(UploadListModelRole::AbsolutePreviewImagePath), "m_absolutePreviewImagePath" },
            { static_cast<int>(UploadListModelRole::Status), "m_status" },
            { static_cast<int>(UploadListModelRole::UploadState), "m_uploadState" },
        };
    }

signals:
    void uploadCompleted();
    void itemUploadCompleted(QVariant publishedFileId, bool successful);
    void userNeedsToAcceptWorkshopLegalAgreement();

public slots:

    Q_INVOKABLE void clearWhenFinished()
    {
        beginResetModel();
        m_uploadListModelItems.clear();
        endResetModel();
    }
    void append(const QString& name, const QString& path, const quint64 appID)
    {
        auto item = std::make_unique<SteamWorkshopItem>(name, path, appID);

        const auto roles = QVector<int> { static_cast<int>(UploadListModelRole::UploadProgressRole),
            static_cast<int>(UploadListModelRole::NameRole),
            static_cast<int>(UploadListModelRole::AbsolutePreviewImagePath),
            static_cast<int>(UploadListModelRole::Status),
            static_cast<int>(UploadListModelRole::UploadState) };

        // Capture roles by value to avoid dangling reference when signal fires after append() returns
        const auto onDataChanged = [this, roles]() { emit this->dataChanged(index(0, 0), index(rowCount() - 1, 0), roles); };

        QObject::connect(item.get(), &SteamWorkshopItem::userNeedsToAcceptWorkshopLegalAgreement, this, &UploadListModel::userNeedsToAcceptWorkshopLegalAgreement);
        QObject::connect(item.get(), &SteamWorkshopItem::uploadProgressChanged, this, onDataChanged);
        QObject::connect(item.get(), &SteamWorkshopItem::uploadStateChanged, this, onDataChanged);
        QObject::connect(item.get(), &SteamWorkshopItem::nameChanged, this, onDataChanged);
        QObject::connect(item.get(), &SteamWorkshopItem::absolutePreviewImagePathChanged, this, onDataChanged);
        QObject::connect(item.get(), &SteamWorkshopItem::uploadComplete, this, [this, itemPtr = item.get()](bool successful) {
            emit this->itemUploadCompleted(itemPtr->publishedFileId(), successful);
        });
        QObject::connect(item.get(), &SteamWorkshopItem::statusChanged, this, [=](ScreenPlayCore::Steam::EResult status) {
            onDataChanged();

            if (m_uploadListModelItems.empty()) {
                qWarning() << "uploadListModel items empty during statusChanged check";
                return;
            }

            bool allItemsUploaded = std::all_of(m_uploadListModelItems.cbegin(), m_uploadListModelItems.cend(), [](const auto& item) {
                const auto status = item->status();
                return status == ScreenPlayCore::Steam::EResult::K_EResultOK || status == ScreenPlayCore::Steam::EResult::K_EResultFail;
            });

            qInfo() << "statusChanged: allItemsUploaded =" << allItemsUploaded << "itemCount =" << m_uploadListModelItems.size();

            if (allItemsUploaded) {
                qInfo() << "Emitting uploadCompleted signal";
                emit this->uploadCompleted();
            }
        });

        beginInsertRows(QModelIndex(), rowCount(), rowCount());
        item->createWorkshopItem();
        m_uploadListModelItems.push_back(std::move(item));
        endInsertRows();
    }

private:
    std::vector<std::unique_ptr<SteamWorkshopItem>> m_uploadListModelItems;
};

}
