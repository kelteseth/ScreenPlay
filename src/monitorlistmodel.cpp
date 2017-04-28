#include "monitorlistmodel.h"

MonitorListModel::MonitorListModel(QObject *parent)
    : QAbstractListModel(parent)
{
    for (int i = 0; i < QApplication::screens().count(); i++) {
        QScreen* screen = QApplication::screens().at(i);
        _monitorList.append(Monitor(screen->name(),screen->size(),screen->availableGeometry(),i));
    }
}

QHash<int, QByteArray> MonitorListModel::roleNames() const
{
    static const QHash<int, QByteArray> roles{
        { NameRole, "monitorName" },
        { SizeRole, "monitorSize" },
        { AvailableGeometryRole, "monitorAvailableGeometryRole" },
        { NumberRole, "monitorNumber" },
    };
    return roles;
}

QVariant MonitorListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    // FIXME: Implement me!
}

bool MonitorListModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    if (value != headerData(section, orientation, role)) {
        // FIXME: Implement me!
        emit headerDataChanged(orientation, section, section);
        return true;
    }
    return false;
}

int MonitorListModel::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid())
        return 0;

    return _monitorList.count();
}

QVariant MonitorListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() < rowCount())
        switch (role) {
        case NameRole:
            return _monitorList.at(index.row())._name;
        case SizeRole:
            return _monitorList.at(index.row())._size;
        case AvailableGeometryRole:
            return _monitorList.at(index.row())._availableGeometry;
        case NumberRole:
            return _monitorList.at(index.row())._number;
        default:
            return QVariant();
        }

    return QVariant();
}

bool MonitorListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (data(index, role) != value) {
        // FIXME: Implement me!
        emit dataChanged(index, index, QVector<int>() << role);
        return true;
    }
    return false;
}

Qt::ItemFlags MonitorListModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return Qt::ItemIsEditable; // FIXME: Implement me!
}

bool MonitorListModel::insertRows(int row, int count, const QModelIndex &parent)
{
    beginInsertRows(parent, row, row + count - 1);
    // FIXME: Implement me!
    endInsertRows();
}

bool MonitorListModel::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row + count - 1);
    // FIXME: Implement me!
    endRemoveRows();
}

Monitor::Monitor()
{

}

Monitor::Monitor(QString name, QSize size, QRect availableGeometry, int number)
{
    _name = name;
    _size = size;
    _availableGeometry = availableGeometry;
    _number = number;
}
