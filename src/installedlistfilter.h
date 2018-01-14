#pragma once

#include <QSortFilterProxyModel>
#include <QRegExp>

#include "installedlistmodel.h"


class InstalledListFilter : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    InstalledListFilter(InstalledListModel *ilm);
    Q_INVOKABLE void sortBy(QString type);

};


