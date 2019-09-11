#include "monitorlistmodel.h"

namespace ScreenPlay {

MonitorListModel::MonitorListModel(QObject* parent)
    : QAbstractListModel(parent)
{
    loadMonitors();

    auto* guiAppInst = dynamic_cast<QGuiApplication*>(QGuiApplication::instance());
    connect(guiAppInst, &QGuiApplication::screenAdded, this, &MonitorListModel::screenAdded);
    connect(guiAppInst, &QGuiApplication::screenRemoved, this, &MonitorListModel::screenRemoved);
}

QHash<int, QByteArray> MonitorListModel::roleNames() const
{
    static const QHash<int, QByteArray> roles {
        { static_cast<int>(MonitorRole::AppID), "m_appID" },
        { static_cast<int>(MonitorRole::MonitorID), "m_monitorID" },
        { static_cast<int>(MonitorRole::Name), "m_name" },
        { static_cast<int>(MonitorRole::Size), "m_size" },
        { static_cast<int>(MonitorRole::AvailableGeometry), "m_availableGeometry" },
        { static_cast<int>(MonitorRole::AvailableVirtualGeometry), "m_availableVirtualGeometry" },
        { static_cast<int>(MonitorRole::Number), "m_number" },
        { static_cast<int>(MonitorRole::Geometry), "m_geometry" },
        { static_cast<int>(MonitorRole::Model), "m_model" },
        { static_cast<int>(MonitorRole::Manufacturer), "m_manufacturer" },
        { static_cast<int>(MonitorRole::IsWallpaperActive), "m_isWallpaperActive" },
        { static_cast<int>(MonitorRole::PreviewImage), "m_previewImage" },
    };
    return roles;
}

int MonitorListModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;

    return m_monitorList.count();
}

QVariant MonitorListModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    int row = index.row();
    if (row < 0 || row >= m_monitorList.count()) {
        return QVariant();
    }

    auto roleEnum = static_cast<MonitorRole>(role);

    if (index.row() < rowCount())
        switch (roleEnum) {
        case MonitorRole::AppID:
            return m_monitorList.at(index.row()).m_appID;
        case MonitorRole::MonitorID:
            return m_monitorList.at(index.row()).m_monitorID;
        case MonitorRole::Name:
            return m_monitorList.at(index.row()).m_name;
        case MonitorRole::Size:
            return m_monitorList.at(index.row()).m_size;
        case MonitorRole::AvailableGeometry:
            return m_monitorList.at(index.row()).m_availableGeometry;
        case MonitorRole::AvailableVirtualGeometry:
            return m_monitorList.at(index.row()).m_availableVirtualGeometry;
        case MonitorRole::Number:
            return m_monitorList.at(index.row()).m_number;
        case MonitorRole::Geometry:
            return m_monitorList.at(index.row()).m_geometry;
        case MonitorRole::Model:
            return m_monitorList.at(index.row()).m_model;
        case MonitorRole::Manufacturer:
            return m_monitorList.at(index.row()).m_manufacturer;
        case MonitorRole::IsWallpaperActive:
            return m_monitorList.at(index.row()).m_isWallpaperActive;
        case MonitorRole::PreviewImage:
            return m_monitorList.at(index.row()).m_previewImage;
        }

    return QVariant();
}

void MonitorListModel::loadMonitors()
{
    int offsetX = 0;
    int offsetY = 0;

    for (int i = 0; i < QApplication::screens().count(); i++) {
        QScreen* screen = QApplication::screens().at(i);
        if (screen->availableGeometry().x() < 0) {
            offsetX += (screen->availableGeometry().x() * -1);
        }
        if (screen->availableGeometry().y() < 0) {
            offsetY += (screen->availableGeometry().y() * -1);
        }
    }

    beginInsertRows(QModelIndex(), 0, rowCount());
    for (int i = 0; i < QApplication::screens().count(); i++) {
        QScreen* screen = QApplication::screens().at(i);

        // Sometimes we get invalid monitors on Windows. I don't know why...
        if (screen->geometry().width() == 0 || screen->geometry().height() == 0)
            continue;

        m_monitorList.append(
            Monitor(
                screen->manufacturer(),
                screen->model(),
                screen->name(),
                "", // No preview image for now
                screen->size(),
                QRect(screen->availableGeometry().x() + offsetX,
                    screen->availableGeometry().y() + offsetY,
                    screen->geometry().width(),
                    screen->geometry().height()),
                i,
                QRect(screen->availableVirtualGeometry().x(),
                    screen->availableVirtualGeometry().y(),
                    screen->availableVirtualGeometry().width(),
                    screen->availableVirtualGeometry().height()),
                QRect(screen->geometry().x() + offsetX,
                    screen->geometry().y() + offsetY,
                    screen->geometry().width(),
                    screen->geometry().height()),
                screen));
    }
    endInsertRows();

    emit monitorReloadCompleted();
}

void MonitorListModel::clearActiveWallpaper()
{
    for (Monitor& monitor : m_monitorList) {
        monitor.m_previewImage = "";
        monitor.m_appID = "";
        emit dataChanged(QModelIndex(),
            QModelIndex(),
            QVector<int> { static_cast<int>(MonitorRole::PreviewImage),
                static_cast<int>(MonitorRole::AppID) });
    }
}

void MonitorListModel::setWallpaperActiveMonitor(const QVector<int> monitors, const QString& appID, const QString& fullPreviewImagePath)
{
    int indexToChange {};
    for (const int monitor : monitors) {
        indexToChange++;
        m_monitorList[monitor].m_previewImage = fullPreviewImagePath;
        m_monitorList[monitor].m_appID = appID;
        emit dataChanged(
            index(indexToChange),
            index(indexToChange),
            QVector<int> {
                static_cast<int>(MonitorRole::PreviewImage),
                static_cast<int>(MonitorRole::AppID) } );

    }



    for (Monitor& monitor : m_monitorList) {
        qDebug() << "OOOOOOOOOOOOOO!"<< monitor.m_previewImage << monitor.m_appID;
    }
}

void MonitorListModel::reloadMonitors()
{
    beginResetModel();
    m_monitorList.clear();
    m_monitorList.squeeze();
    endResetModel();
    loadMonitors();
}

}
