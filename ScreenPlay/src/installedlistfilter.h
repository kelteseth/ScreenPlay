#pragma once

#include <QRegExp>
#include <QSortFilterProxyModel>
#include <memory>
#include "installedlistmodel.h"
namespace ScreenPlay {

class InstalledListFilter : public QSortFilterProxyModel {
    Q_OBJECT

public:
    InstalledListFilter(const shared_ptr<InstalledListModel>& ilm);

public slots:
    void sortByRoleType(QString type);
    void sortByName(QString name);
    void resetFilter();
private:
    const shared_ptr<InstalledListModel> m_ilm;
};
}
