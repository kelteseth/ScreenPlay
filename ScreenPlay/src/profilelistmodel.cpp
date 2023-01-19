// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#include "ScreenPlay/profilelistmodel.h"

#include <QDirIterator>
#include <QFileInfoList>
#include <QJsonDocument>
#include <QJsonObject>
#include <QPair>

namespace ScreenPlay {

/*!
    \class ScreenPlay::ProfileListModel
    \inmodule ScreenPlay
    \brief Used to save all active wallpapers and widgets position and configurations after a restart.

    Not yet in used class. Otherwhise it is a regular QAbstractListModel based list model.

*/

/*!
  Constructor
*/
ProfileListModel::ProfileListModel(const std::shared_ptr<GlobalVariables>& globalVariables, QObject* parent)
    : QAbstractListModel(parent)
    , m_globalVariables { globalVariables }
{
}

/*!
    \brief .
*/
int ProfileListModel::rowCount(const QModelIndex& parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid())
        return 0;

    return m_profileList.count();
}

/*!
    \brief .
*/
QVariant ProfileListModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    int row = index.row();
    if (row < 0 || row >= m_profileList.count()) {
        return QVariant();
    }

    if (row < 0 || row >= m_profileList.count()) {
        return QVariant();
    }

    switch (role) {
    case NameRole:
        return QVariant();
    }
    return QVariant();
}

/*!
    \brief .
*/
QHash<int, QByteArray> ProfileListModel::roleNames() const
{
    QHash<int, QByteArray> m_roleNames;
    return m_roleNames;
}

/*!
    \brief .
*/
bool ProfileListModel::getProfileByName(QString id, Profile* profile)
{
    for (int i = 0; i < m_profileList.size(); i++) {
        if (m_profileList.at(i).m_id == id) {
            *profile = m_profileList.at(i);
            return true;
        }
    }
    return false;
}

/*!
    \brief .
*/
void ProfileListModel::append(const Profile& profile)
{
}
}
