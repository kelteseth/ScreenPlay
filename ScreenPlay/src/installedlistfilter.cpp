#include "installedlistfilter.h"

/*!
    \class Installed List Filder
    \brief  Proxy between Installed List Model and QML view to filter items.
            Maybe this class could be merged with the InstalledListModel...

    \todo
            - Expand filter functionality

*/
namespace ScreenPlay {
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
    } else if (type == "Wallpaper") {
        setFilterRole(InstalledListModel::InstalledRole::TypeRole);
        setFilterWildcard("*Wallpaper");
    } else if (type == "Widgets") {
        setFilterRole(InstalledListModel::InstalledRole::TypeRole);
        setFilterWildcard("*Widget");
    }
    if (type == "Scenes") {
        setFilterRole(InstalledListModel::InstalledRole::TypeRole);
        setFilterFixedString("qmlScene");
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
