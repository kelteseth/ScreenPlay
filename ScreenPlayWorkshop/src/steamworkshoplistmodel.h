// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#pragma once

#include <QAbstractListModel>
#include <QDebug>
#include <QSharedPointer>
#include <QVariant>
#include <QVector>
#include <QtConcurrent/QtConcurrent>
#include <QtQml/qqml.h>
#include <memory>

#include "workshopitem.h"
// Steam
#include "ScreenPlayCore/steamenumsgenerated.h"
#include "steam/steam_api.h"

namespace ScreenPlayWorkshop {

/*!
    \class ScreenPlayWorkshop::BannerInfo
    \inmodule ScreenPlayWorkshop
    \brief Aggregates all banner-related data for the first workshop item,
           returned as a single value type to QML.
*/
struct BannerInfo {
    Q_GADGET
    QML_VALUE_TYPE(bannerInfo)
    QML_STRUCTURED_VALUE
    Q_PROPERTY(QUrl imageUrl MEMBER imageUrl)
    Q_PROPERTY(QString title MEMBER title)
    Q_PROPERTY(QVariant publishedFileID MEMBER publishedFileID)
    Q_PROPERTY(quint64 subscriptionCount MEMBER subscriptionCount)
    Q_PROPERTY(QString creatorSteamID MEMBER creatorSteamID)

public:
    Q_INVOKABLE BannerInfo() = default;
    QUrl imageUrl;
    QString title;
    QVariant publishedFileID;
    quint64 subscriptionCount = 0;
    QString creatorSteamID;
};

class SteamWorkshopListModel : public QAbstractListModel {
    Q_OBJECT
    Q_PROPERTY(int pages READ pages WRITE setPages NOTIFY pagesChanged)
    Q_PROPERTY(int currentPage READ currentPage WRITE setCurrentPage NOTIFY currentPageChanged)
    Q_PROPERTY(QUrl bannerUrl READ bannerUrl NOTIFY bannerUrlChanged)
    Q_PROPERTY(bool hasMore READ hasMore NOTIFY hasMoreChanged)
    Q_PROPERTY(bool isLoading READ isLoading NOTIFY isLoadingChanged)

public:
    explicit SteamWorkshopListModel(AppId_t appID, QObject* parent = nullptr);
    QHash<int, QByteArray> roleNames() const override;

    enum WorkshopRole {
        TitleRole,
        ImageUrlRole,
        AdditionalPreviewUrlRole,
        AdditionalPreviewWebpUrlRole,
        PublishedFileIDRole,
        SubscriptionCountRole,
        TagsRole,
        IsOwnItemRole,
    };
    Q_ENUM(WorkshopRole)

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    void append(WorkshopItem item);

    int pages() const { return m_pages; }
    int currentPage() const { return m_currentPage; }
    QUrl bannerUrl() const { return m_workshopItemList.empty() ? QUrl {} : m_workshopItemList.at(0).m_previewImageUrl; }
    bool hasMore() const { return m_currentPage < m_pages; }
    bool isLoading() const { return m_isLoading; }

signals:
    void pagesChanged(int pages);
    void currentPageChanged(int currentPage);
    void bannerUrlChanged();
    void hasMoreChanged();
    void isLoadingChanged();

public slots:

    Q_INVOKABLE BannerInfo getBannerInfo() const
    {
        if (m_workshopItemList.empty())
            return {};
        const auto& item = m_workshopItemList.at(0);
        BannerInfo info;
        info.imageUrl = item.m_previewImageUrl;
        info.title = item.m_title;
        info.publishedFileID = item.m_publishedFileID;
        info.subscriptionCount = item.m_subscriptionCount;
        // steamID64 stored as string to preserve precision in QML
        info.creatorSteamID = QString::number(item.m_steamIDOwner);
        return info;
    }
    void clear();

    Q_INVOKABLE void removeByPublishedFileID(quint64 publishedFileID)
    {
        for (int i = 0; i < m_workshopItemList.size(); ++i) {
            if (m_workshopItemList.at(i).m_publishedFileID == publishedFileID) {
                beginRemoveRows(QModelIndex(), i, i);
                m_workshopItemList.removeAt(i);
                endRemoveRows();
                return;
            }
        }
    }

    void setPages(int pages)
    {
        if (m_pages == pages)
            return;

        m_pages = pages;
        emit pagesChanged(m_pages);
        emit hasMoreChanged();
    }

    void setCurrentPage(int currentPage)
    {
        if (m_currentPage == currentPage)
            return;

        m_currentPage = currentPage;
        emit currentPageChanged(m_currentPage);
        emit hasMoreChanged();
    }

    void setIsLoading(bool isLoading)
    {
        if (m_isLoading == isLoading)
            return;

        m_isLoading = isLoading;
        emit isLoadingChanged();
    }

    void reset()
    {
        clear();
        m_currentPage = 1;
        m_pages = 1;
        emit currentPageChanged(m_currentPage);
        emit pagesChanged(m_pages);
        emit hasMoreChanged();
    }

    void incrementPage()
    {
        m_currentPage++;
        emit currentPageChanged(m_currentPage);
        emit hasMoreChanged();
    }

private:
    UGCQueryHandle_t m_UGCSearchHandle = 0;
    QVector<WorkshopItem> m_workshopItemList;

    const quint64 m_appID = 0;
    int m_pages = 1;
    int m_currentPage = 1;
    bool m_isLoading = false;
};
}
