#include "installedlistfilter.h"

namespace ScreenPlay {

/*!
    \class ScreenPlay::InstalledListFilter
    \inmodule ScreenPlay
    \brief  Proxy between Installed List Model and QML view to filter items.

    This class could be merged with the InstalledListModel in the future.
*/

/*!
  \brief Sets the default role type to "All" to display all available content. Needs a given
  \a ilm (InstalledListModel).
*/
InstalledListFilter::InstalledListFilter(const std::shared_ptr<InstalledListModel>& ilm)
    : QSortFilterProxyModel()
    , m_ilm(ilm)
{
    setSourceModel(m_ilm.get());
    setFilterRole(InstalledListModel::InstalledRole::TitleRole);
    sortByRoleType("All");
}

/*!
 \brief Set the filter proxy to sort by the given \a type. This can be:
 \list
    \li All
    \li Videos
    \li Widgets
    \li Scenes
 \endlist
*/
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

/*!
  \brief Invoked when the user uses the quicksearch at the top right of the installed page.
  Uses the \a name to sort by name. This name is saved in the project.json title.
*/
void InstalledListFilter::sortByName(QString name)
{
    setFilterRole(InstalledListModel::InstalledRole::TitleRole);
    setFilterCaseSensitivity(Qt::CaseInsensitive);
    setFilterFixedString(name);
    sort(0);
}

/*!
 \brief Resets the filter and sorts by title.
*/
void InstalledListFilter::resetFilter()
{
    setFilterRole(InstalledListModel::InstalledRole::TitleRole);
    setFilterWildcard("*");
    sort(0);
}
}
