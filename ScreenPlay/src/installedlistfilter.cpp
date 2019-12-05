#include "installedlistfilter.h"


namespace ScreenPlay {

/*!
    \class ScreenPlay::InstalledListFilter
    \inmodule ScreenPlay
    \brief  Proxy between Installed List Model and QML view to filter items.
            Maybe this class could be merged with the InstalledListModel...
*/

InstalledListFilter::InstalledListFilter(const shared_ptr<InstalledListModel>& ilm)
    : QSortFilterProxyModel()
    , m_ilm(ilm)
{
    setSourceModel(m_ilm.get());
    setFilterRole(InstalledListModel::InstalledRole::TitleRole);
    sortByRoleType("All");
}

void InstalledListFilter::sortByRoleType(QString type)
{
    if (type == "All") {
        setFilterRole(InstalledListModel::InstalledRole::TitleRole);
        setFilterWildcard("*");
    } else if (type == "Videos") {
        setFilterRole(InstalledListModel::InstalledRole::TypeRole);
        setFilterFixedString("videoWallpaper");
    } else if (type == "Widgets") {
        setFilterRole(InstalledListModel::InstalledRole::TypeRole);
        setFilterWildcard("*Widget");
    } else if (type == "Scenes") {
        setFilterRole(InstalledListModel::InstalledRole::TypeRole);
        setFilterFixedString("qmlWallpaper");
    }

    sort(0);
}

void InstalledListFilter::sortByName(QString name)
{
    setFilterRole(InstalledListModel::InstalledRole::TitleRole);
    setFilterCaseSensitivity(Qt::CaseInsensitive);
    setFilterFixedString(name);
    sort(0);
}

void InstalledListFilter::resetFilter()
{
    setFilterRole(InstalledListModel::InstalledRole::TitleRole);
    setFilterWildcard("*");
    sort(0);
}
}
