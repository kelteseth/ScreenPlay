#pragma once

#include <QRegExp>
#include <QSortFilterProxyModel>
#include "installedlistmodel.h"

/*!
    \class Installed List Filder
    \brief Proxy between Installed List Model and QML view to filter items

*/

class InstalledListFilter : public QSortFilterProxyModel {
    Q_OBJECT
public:
    InstalledListFilter(InstalledListModel* ilm);

public slots:
    void sortByRoleType(QString type);
    void sortByName(QString name);
    void resetFilter();
};
