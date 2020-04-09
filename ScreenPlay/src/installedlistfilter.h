#pragma once

#include "installedlistmodel.h"
#include <QRegExp>
#include <QSortFilterProxyModel>
#include <memory>

namespace ScreenPlay {

class InstalledListFilter : public QSortFilterProxyModel {
    Q_OBJECT

public:
    InstalledListFilter(const std::shared_ptr<InstalledListModel>& ilm);

public slots:
    void sortByRoleType(QString type);
    void sortByName(QString name);
    void resetFilter();

private:
    const std::shared_ptr<InstalledListModel> m_ilm;
};
}
