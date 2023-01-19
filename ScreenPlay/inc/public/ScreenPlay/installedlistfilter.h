// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#pragma once

#include <QRegularExpression>
#include <QSortFilterProxyModel>
#include <memory>

#include "ScreenPlay/globalvariables.h"
#include "ScreenPlay/installedlistmodel.h"

namespace ScreenPlay {

class InstalledListFilter : public QSortFilterProxyModel {
    Q_OBJECT
    QML_ELEMENT

public:
    InstalledListFilter(const std::shared_ptr<InstalledListModel>& ilm);

public slots:
    void sortBySearchType(const ScreenPlay::SearchType::SearchType searchType);
    void setSortOrder(const Qt::SortOrder sortOrder);
    void sortByName(const QString& name);
    void resetFilter();

signals:
    void sortChanged();

private:
    const std::shared_ptr<InstalledListModel> m_ilm;
    ScreenPlay::SearchType::SearchType m_searchType = ScreenPlay::SearchType::SearchType::All;
    Qt::SortOrder m_sortOrder = Qt::SortOrder::DescendingOrder;
};
}
