#pragma once

#include <QSortFilterProxyModel>
#include <QRegExp>

#include "installedlistmodel.h"


class InstalledListFilter : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    InstalledListFilter(InstalledListModel *ilm);

    Q_INVOKABLE void sortByRoleType(QString type);
    Q_INVOKABLE void sortByName(QString name);
    Q_INVOKABLE void resetFilter();

};


