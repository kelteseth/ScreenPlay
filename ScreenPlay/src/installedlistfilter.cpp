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
    setFilterRole(static_cast<int>(InstalledListModel::ScreenPlayItem::Title));
}

/*!
  \brief Invoked when the user uses the quicksearch at the top right of the installed page.
  Uses the \a name to sort by name. This name is saved in the project.json title.
*/
void InstalledListFilter::sortByName(const QString& name)
{
    setFilterRole(static_cast<int>(InstalledListModel::ScreenPlayItem::Title));
    setFilterCaseSensitivity(Qt::CaseInsensitive);
    setFilterFixedString(name);
    sort(0);
}

void InstalledListFilter::sortBySearchType(const ScreenPlay::SearchType::SearchType searchType)
{
    if (searchType == SearchType::SearchType::All) {
        resetFilter();
        return;
    }
    setFilterRole(static_cast<int>(InstalledListModel::ScreenPlayItem::SearchType));
    setFilterFixedString(QVariant::fromValue(searchType).toString());
    sort(0);
}

/*!
 \brief Resets the filter and sorts by title.
*/
void InstalledListFilter::resetFilter()
{
    setFilterRole(static_cast<int>(InstalledListModel::ScreenPlayItem::Title));
    setFilterWildcard("*");
    sort(0);
}

}
