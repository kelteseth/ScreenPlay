#pragma once

#include <QSortFilterProxyModel>
#include <QRegExp>

#include "installedlistmodel.h"


class InstalledListFilter : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    InstalledListFilter(InstalledListModel *ilm);

public slots:
    void sortByRoleType(QString type);
    void sortByName(QString name);
    void resetFilter();

};


