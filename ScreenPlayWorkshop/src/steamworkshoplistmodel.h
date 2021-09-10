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
#include "cstring"
#include "stdlib.h"
#include "steam/steam_api.h"
#include "steam/steam_qt_enums_generated.h"

/*!
    \class Steam Workshop List Model
    \brief Gets filled and managed by the Steam Workshop Class

*/
namespace ScreenPlayWorkshop {

class SteamWorkshopListModel : public QAbstractListModel {
    Q_OBJECT
    Q_PROPERTY(int pages READ pages WRITE setPages NOTIFY pagesChanged)
    Q_PROPERTY(int currentPage READ currentPage WRITE setCurrentPage NOTIFY currentPageChanged)

public:
    explicit SteamWorkshopListModel(AppId_t appID, QObject* parent = nullptr);
    QHash<int, QByteArray> roleNames() const override;

    enum WorkshopRole {
        TitleRole,
        ImageUrlRole,
        AdditionalPreviewUrlRole,
        PublishedFileIDRole,
        SubscriptionCountRole,
    };
    Q_ENUM(WorkshopRole)

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    void append(WorkshopItem item);

    int pages() const { return m_pages; }
    int currentPage() const { return m_currentPage; }

signals:
    void pagesChanged(int pages);
    void currentPageChanged(int currentPage);

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
    unsigned int getBannerAmountSubscriber()
    {
        return m_workshopItemList.empty() ? int {} : m_workshopItemList.at(0).m_subscriptionCount;
    }

    void clear();

    void setPages(int pages)
    {
        if (m_pages == pages)
            return;

        m_pages = pages;
        emit pagesChanged(m_pages);
    }

    void setCurrentPage(int currentPage)
    {
        if (m_currentPage == currentPage)
            return;

        m_currentPage = currentPage;
        emit currentPageChanged(m_currentPage);
    }

private:
    UGCQueryHandle_t m_UGCSearchHandle = 0;
    QVector<WorkshopItem> m_workshopItemList;

    const unsigned int m_appID = 0;
    int m_pages = 1;
    int m_currentPage = 1;
};
}
