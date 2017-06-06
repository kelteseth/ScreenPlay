#include "profilelistmodel.h"

ProfileListModel::ProfileListModel(QObject* parent)
    : QAbstractListModel(parent)
{
}

int ProfileListModel::rowCount(const QModelIndex& parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid())
        return 0;

    return m_profileList.count();
}

QVariant ProfileListModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    int row = index.row();
    if (row < 0 || row >= m_profileList.count()) {
        return QVariant();
    }

    switch (role) {
    case NameRole:
        return QVariant();
    }
    return QVariant();
}

QHash<int, QByteArray> ProfileListModel::roleNames() const
{
    return m_roleNames;
}

void ProfileListModel::loadProfiles()
{

    QString writablePath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) ;
    if (!QDir(writablePath).exists()) {
        if (!QDir().mkdir(writablePath)) {
            qWarning("ERROR: Cloud not create install dir");

            return;
        }
    } else {

    }

}


