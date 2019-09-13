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
            if (m_monitorList.at(index.row()).m_activeWallpaper) {
                return m_monitorList.at(index.row()).m_activeWallpaper->appID();
            } else {
                return QVariant("");
            }
        case MonitorRole::MonitorID: {
            QScreen* screen = m_monitorList.at(index.row()).m_screen;

            QVariant id = QString::number(screen->size().width())
                + "x" + QString::number(screen->size().height())
                + "_" + QString::number(screen->availableGeometry().x())
                + "x" + QString::number(screen->availableGeometry().y());

            return id;
        }
        case MonitorRole::Name:
            return m_monitorList.at(index.row()).m_screen->name();
        case MonitorRole::Size:
            return m_monitorList.at(index.row()).m_screen->size();
        case MonitorRole::AvailableGeometry:
            return m_monitorList.at(index.row()).m_availableGeometry;
        case MonitorRole::AvailableVirtualGeometry:
            return m_monitorList.at(index.row()).m_screen->availableVirtualGeometry();
        case MonitorRole::Number:
            return m_monitorList.at(index.row()).m_number;
        case MonitorRole::Geometry:
            return m_monitorList.at(index.row()).m_screen->geometry();
        case MonitorRole::Model:
            return m_monitorList.at(index.row()).m_screen->model();
        case MonitorRole::Manufacturer:
            return m_monitorList.at(index.row()).m_screen->manufacturer();
        case MonitorRole::PreviewImage:
            if (m_monitorList.at(index.row()).m_activeWallpaper) {
                return m_monitorList.at(index.row()).m_activeWallpaper->previewImage();
            } else {
                return QVariant("");
            }
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

        QRect availableVirtualGeometry(
            screen->geometry().x() + offsetX,
            screen->geometry().y() + offsetY,
            screen->geometry().width(),
            screen->geometry().height());
        m_monitorList.append(Monitor { i, availableVirtualGeometry, screen });
    }
    endInsertRows();

    emit monitorReloadCompleted();
}

void MonitorListModel::clearActiveWallpaper()
{
    int i { 0 };
    for (Monitor& monitor : m_monitorList) {
        monitor.m_activeWallpaper = nullptr;
        emit dataChanged(
            index(i, 0),
            index(i, 0),
            QVector<int> {
                static_cast<int>(MonitorRole::PreviewImage),
                static_cast<int>(MonitorRole::AppID) });
        ++i;
    }
}

void MonitorListModel::closeWallpaper(const QString& appID)
{
    int i {};
    for (auto& item : m_monitorList) {
        if (item.m_activeWallpaper) {
            if (item.m_activeWallpaper->appID() == appID) {
                item.m_activeWallpaper = nullptr;
                emit dataChanged(
                    index(i, 0),
                    index(i, 0),
                    QVector<int> {
                        static_cast<int>(MonitorRole::PreviewImage),
                        static_cast<int>(MonitorRole::AppID) });
            }
        }
        ++i;
    }
}

void MonitorListModel::setWallpaperActiveMonitor(const std::shared_ptr<ScreenPlayWallpaper>& wallpaper, const QVector<int> monitors)
{

    for (const int monitor : monitors) {
        m_monitorList[monitor].m_activeWallpaper = wallpaper;

        emit dataChanged(
            index(monitor, 0),
            index(monitor, 0),
            QVector<int> {
                static_cast<int>(MonitorRole::PreviewImage),
                static_cast<int>(MonitorRole::AppID) });
    }
}

std::optional<QString> MonitorListModel::getAppIDByMonitorIndex(const int index) const
{
    for (auto& monitor : m_monitorList) {
        if (monitor.m_number == index && monitor.m_activeWallpaper) {
            return { monitor.m_activeWallpaper->appID() };
        }
    }
    return std::nullopt;
}

void MonitorListModel::reset()
{
    beginResetModel();
    m_monitorList.clear();
    m_monitorList.squeeze();
    endResetModel();
    loadMonitors();
}

}
