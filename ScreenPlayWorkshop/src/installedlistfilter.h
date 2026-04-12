// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#pragma once

#include <QSortFilterProxyModel>
#include <QtQml>

#include "installedlistmodel.h"

/*!
    \class ScreenPlayWorkshop::InstalledListFilter
    \inmodule ScreenPlayWorkshop
    \brief Proxy model that filters out items already published to the Workshop
           and provides sorting by title or last-modified date.
*/
namespace ScreenPlayWorkshop {

class InstalledListFilter : public QSortFilterProxyModel {
    Q_OBJECT
    QML_UNCREATABLE("")

    Q_PROPERTY(bool hideWorkshopItems READ hideWorkshopItems WRITE setHideWorkshopItems NOTIFY hideWorkshopItemsChanged)

public:
    explicit InstalledListFilter(QObject* parent = nullptr)
        : QSortFilterProxyModel(parent)
    {
    }

    bool hideWorkshopItems() const { return m_hideWorkshopItems; }

    void setHideWorkshopItems(const bool hide)
    {
        if (m_hideWorkshopItems == hide)
            return;
        m_hideWorkshopItems = hide;
        emit hideWorkshopItemsChanged();
        beginFilterChange();
        endFilterChange();
    }

    /*!
        \brief Sorts the underlying model by field and direction,
               then re-applies filtering.
    */
    Q_INVOKABLE void sort(InstalledListModel::SortField field, bool ascending)
    {
        auto* src = qobject_cast<InstalledListModel*>(sourceModel());
        if (!src)
            return;
        src->sort(field, ascending);
        invalidate();
    }

signals:
    void hideWorkshopItemsChanged();

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override
    {
        if (!m_hideWorkshopItems)
            return true;

        const auto idx = sourceModel()->index(sourceRow, 0, sourceParent);
        const auto isOnWorkshop = idx.data(
            static_cast<int>(InstalledListModel::ScreenPlayItem::IsOnWorkshop));
        return !isOnWorkshop.toBool();
    }

private:
    bool m_hideWorkshopItems = true;
};

}
