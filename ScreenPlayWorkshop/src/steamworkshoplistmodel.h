// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#pragma once

#include <QAbstractListModel>
#include <QDebug>
#include <QSharedPointer>
#include <QVariant>
#include <QVector>
#include <QtConcurrent/QtConcurrent>
#include <memory>

#include "workshopitem.h"
// Steam
#include "ScreenPlayCore/steamenumsgenerated.h"
#include "cstring"
#include "stdlib.h"
#include "steam/steam_api.h"

/*!
    \class Steam Workshop List Model
    \brief Gets filled and managed by the Steam Workshop Class

*/
namespace ScreenPlayWorkshop {

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
        PublishedFileIDRole,
        SubscriptionCountRole,
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

    QUrl getBannerUrl()
    {
        return m_workshopItemList.empty() ? QUrl {} : m_workshopItemList.at(0).m_previewImageUrl;
    }
    QString getBannerText()
    {
        return m_workshopItemList.empty() ? QString {} : m_workshopItemList.at(0).m_title;
    }
    QVariant getBannerID()
    {
        return m_workshopItemList.empty() ? QVariant {} : m_workshopItemList.at(0).m_publishedFileID;
    }
    quint64 getBannerAmountSubscriber()
    {
        return m_workshopItemList.empty() ? int {} : m_workshopItemList.at(0).m_subscriptionCount;
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
