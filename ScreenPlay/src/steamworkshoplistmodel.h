#pragma once

#include <QAbstractListModel>
#include <QDebug>
#include <QSharedPointer>
#include <QVector>

#include "workshopitem.h"

/*!
    \class Steam Workshop List Model
    \brief Gets filled and managed by the Steam Workshop Class

*/


class SteamWorkshopListModel : public QAbstractListModel {
    Q_OBJECT

public:
    explicit SteamWorkshopListModel(QObject* parent = nullptr);
    QHash<int, QByteArray> roleNames() const override;
    enum WorkshopRole {
        TitleRole,
        ImageUrlRole,
        IDRole,
    };
    Q_ENUM(WorkshopRole)

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant& value, int role = Qt::EditRole) override;

    // Basic functionality:
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;



    // Editable:
    bool setData(const QModelIndex& index, const QVariant& value,
        int role = Qt::EditRole) override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

    // Add data:
    bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;

    // Remove data:
    bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;

public slots:
    void setBannerWorkshopItem(unsigned int id, QString title, QUrl imgUrl, int numSubscriptions);
    void append(unsigned int id, QString title, QUrl imgUrl, int numSubscriptions);
    void addWorkshopItem(WorkshopItem wi);

   QUrl getBannerUrl();
   QString getBannerText();
   unsigned int getBannerID();
   void clear();
private:
    QVector<QSharedPointer<WorkshopItem>> m_workshopItemList;
    WorkshopItem m_workshopBannerItem;
};
