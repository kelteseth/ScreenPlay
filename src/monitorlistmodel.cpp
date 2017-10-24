#include "monitorlistmodel.h"

MonitorListModel::MonitorListModel(QObject* parent)
    : QAbstractListModel(parent)
{
    loadMonitors();
}

QHash<int, QByteArray> MonitorListModel::roleNames() const
{
    static const QHash<int, QByteArray> roles{
        { NameRole, "monitorName" },
        { SizeRole, "monitorSize" },
        { AvailableGeometryRole, "monitorAvailableGeometry" },
        { AvailableVirtualGeometryRole, "monitorAvailableVirtualGeometry" },
        { NumberRole, "monitorNumber" },
    };
    return roles;
}

QRect MonitorListModel::getAbsoluteDesktopSize()
{
    return m_monitorList.at(0).m_availableVirtualGeometry;
}

QVariant MonitorListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    // FIXME: Implement me!
}

bool MonitorListModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant& value, int role)
{
    if (value != headerData(section, orientation, role)) {
        // FIXME: Implement me!
        emit headerDataChanged(orientation, section, section);
        return true;
    }
    return false;
}

int MonitorListModel::rowCount(const QModelIndex& parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid())
        return 0;

    return m_monitorList.count();
}

QVariant MonitorListModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() < rowCount())
        switch (role) {
        case NameRole:
            return m_monitorList.at(index.row()).m_name;
        case SizeRole:
            return m_monitorList.at(index.row()).m_size;
        case AvailableGeometryRole:
            return m_monitorList.at(index.row()).m_availableGeometry;
        case AvailableVirtualGeometryRole:
            return m_monitorList.at(index.row()).m_availableVirtualGeometry;
        case NumberRole:
            return m_monitorList.at(index.row()).m_number;
        default:
            return QVariant();
        }

    return QVariant();
}

bool MonitorListModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (data(index, role) != value) {
        // FIXME: Implement me!
        emit dataChanged(index, index, QVector<int>() << role);
        return true;
    }
    return false;
}

Qt::ItemFlags MonitorListModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return Qt::ItemIsEditable; // FIXME: Implement me!
}

bool MonitorListModel::insertRows(int row, int count, const QModelIndex& parent)
{
    beginInsertRows(parent, row, row + count - 1);
    // FIXME: Implement me!
    endInsertRows();
}

bool MonitorListModel::removeRows(int row, int count, const QModelIndex& parent)
{
    beginRemoveRows(parent, row, row + count - 1);
    // FIXME: Implement me!
    endRemoveRows();
}

void MonitorListModel::loadMonitors()
{
    for (int i = 0; i < QApplication::screens().count(); i++) {
        QScreen* screen = QApplication::screens().at(i);

        m_monitorList.append(Monitor(screen->manufacturer(),
            screen->model(),
            screen->name(),
            screen->size(),
            screen->availableGeometry(),
            // More convenient for the user if the first monitor == 1
            i + 1,
            screen->availableVirtualGeometry()));
    }
}

int MonitorListModel::size()
{
    return m_monitorList.size();
}


bool MonitorListModel::getMonitorListItemAt(int position, Monitor* monitor)
{
    if (position < 0 && position > m_monitorList.size()) {
        return false;
    } else {
         *monitor = m_monitorList.at(position);
        return true;
    }
}

float MonitorListModel::GetHighestMonitorYValue()
{
    /*for (int i = 0; i < vector.size(); ++i)
    {
        m_monitorList.at(i).screen->availableVirtualGeometry().x();
        m_monitorList.at(i).screen->availableGeometry().y();
    }*/
    // m_monitorList.
    return 0.0f;
}

Monitor::Monitor()
{
}

Monitor::Monitor(QString manufacturer, QString model, QString name, QSize size, QRect availableGeometry, int number, QRect availableVirtualGeometry)
{

    m_name = name;
    m_size = size;
    m_availableGeometry = availableGeometry;

    m_availableVirtualGeometry = availableVirtualGeometry;
    m_number = number;
    // FIXME: Use a better way to create an id
    // because name and manufacturer are allways empty
    m_id = /*name + "_" +*/ QString::number(size.width()) + "x" + QString::number(size.height()) + "_" + QString::number(availableGeometry.x()) + "x" + QString::number(availableGeometry.y());
}
