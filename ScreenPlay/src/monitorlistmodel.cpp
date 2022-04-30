#include "ScreenPlay/monitorlistmodel.h"

namespace ScreenPlay {

/*!
    \class ScreenPlay::MonitorListModel
    \inmodule ScreenPlay
    \brief MonitorListModel.

    Loads all available monitors and saves them in a QAbstractListModel inherited list model.
    This list model gets updated if:
    \list
        \li A user adds or removes a monitor
        \li A user adds or removes a wallpaper
        \li On startup based on profiles.json
    \endlist
    This listmodel is also needed for calculating the monitor preview in the UI.
*/

/*!
  \brief Constructor.
*/
MonitorListModel::MonitorListModel(QObject* parent)
    : QAbstractListModel(parent)
{
    loadMonitors();

    auto* guiAppInst = dynamic_cast<QApplication*>(QApplication::instance());
    connect(guiAppInst, &QApplication::screenAdded, this, &MonitorListModel::screenAdded);
    connect(guiAppInst, &QApplication::screenRemoved, this, &MonitorListModel::screenRemoved);
}

/*!
    \brief Returns the variable names for QML.
*/
QHash<int, QByteArray> MonitorListModel::roleNames() const
{
    static const QHash<int, QByteArray> roles {
        { static_cast<int>(MonitorRole::AppID), "m_appID" },
        { static_cast<int>(MonitorRole::Index), "m_index" },
        { static_cast<int>(MonitorRole::Geometry), "m_geometry" },
        { static_cast<int>(MonitorRole::PreviewImage), "m_previewImage" },
        { static_cast<int>(MonitorRole::InstalledType), "m_installedType" },
    };
    return roles;
}

/*!
    \brief Returns the amount of active monitors.
*/
int MonitorListModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;

    return m_monitorList.count();
}

/*!
    \brief Retruns the data member of the Monitor.
*/
QVariant MonitorListModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    int row = index.row();
    if (row < 0 || row >= m_monitorList.count()) {
        return QVariant();
    }

    auto roleEnum = static_cast<MonitorRole>(role);

    if (row < rowCount())
        switch (roleEnum) {
        case MonitorRole::AppID:
            if (m_monitorList.at(row).m_activeWallpaper) {
                return m_monitorList.at(row).m_activeWallpaper->appID();
            } else {
                return QVariant("");
            }
        case MonitorRole::Index:
            return m_monitorList.at(row).m_index;
        case MonitorRole::Geometry:
            return m_monitorList.at(row).m_geometry;
        case MonitorRole::InstalledType:
            if (m_monitorList.at(row).m_activeWallpaper) {
                return static_cast<int>(m_monitorList.at(row).m_activeWallpaper->type());
            } else {
                return { "" };
            }
        case MonitorRole::PreviewImage:
            if (m_monitorList.at(row).m_activeWallpaper) {
                QString absolutePath = m_monitorList.at(row).m_activeWallpaper->absolutePath();
                return absolutePath + "/" + m_monitorList.at(row).m_activeWallpaper->previewImage();
            } else {
                return QVariant("");
            }
        }

    return QVariant();
}

/*!
  \brief Reloads the listmodel of monitors.
*/
void MonitorListModel::loadMonitors()
{

#ifdef Q_OS_WIN
    QModelIndex index;
    ScreenPlayUtil::WinMonitorStats monitors;

    // This offset lets us center the monitor selection view in the center
    int offsetX = 0;
    int offsetY = 0;
    const int moinitorCount = monitors.iMonitors.size();

    for (int i = 0; i < moinitorCount; i++) {
        const int x = monitors.rcMonitors[i].left;
        const int y = monitors.rcMonitors[i].top;
        if (x < 0) {
            offsetX += (x * -1);
        }
        if (y < 0) {
            offsetY += (y * -1);
        }
    }

    for (int i = 0; i < moinitorCount; i++) {
        const int width = std::abs(monitors.rcMonitors[i].right - monitors.rcMonitors[i].left);
        const int height = std::abs(monitors.rcMonitors[i].top - monitors.rcMonitors[i].bottom);
        const int x = monitors.rcMonitors[i].left;
        const int y = monitors.rcMonitors[i].top;
        QRect geometry(
            x + offsetX,
            y + offsetY,
            width,
            height);
        beginInsertRows(index, m_monitorList.size(), m_monitorList.size());
        m_monitorList.append(Monitor { i, geometry });
        endInsertRows();
    }
#else
    QModelIndex index;
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

    for (int i = 0; i < QApplication::screens().count(); i++) {
        QScreen* screen = QApplication::screens().at(i);

        // Sometimes we get invalid monitors on Windows. I don't know why...
        if (screen->geometry().width() == 0 || screen->geometry().height() == 0)
            continue;

        beginInsertRows(index, m_monitorList.size(), m_monitorList.size());
        m_monitorList.append(Monitor { i, screen->geometry() });
        endInsertRows();
    }
#endif

    emit monitorReloadCompleted();
}

/*!
  \brief Clears the listmodel.
*/
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
                static_cast<int>(MonitorRole::InstalledType),
                static_cast<int>(MonitorRole::AppID) });
        ++i;
    }
}

/*!
  \brief Removes the preview image and appID inside an monitor item.
*/
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
                        static_cast<int>(MonitorRole::InstalledType),
                        static_cast<int>(MonitorRole::AppID) });
            }
        }
        ++i;
    }
}

/*!
 * \brief MonitorListModel::getAbsoluteDesktopSize
 * \return
 */
QRect MonitorListModel::absoluteDesktopSize() const
{
    auto* app = static_cast<QApplication*>(QGuiApplication::instance());
    return app->screens().at(0)->availableVirtualGeometry();
}

/*!
  \brief Sets a shared_ptr to the monitor list. This should be used to set and
         remove the shared_ptr.
*/
void MonitorListModel::setWallpaperMonitor(const std::shared_ptr<ScreenPlayWallpaper>& wallpaper, const QVector<int> monitors)
{

    for (const int monitor : monitors) {
        m_monitorList[monitor].m_activeWallpaper = wallpaper;

        emit dataChanged(
            index(monitor, 0),
            index(monitor, 0),
            QVector<int> {
                static_cast<int>(MonitorRole::PreviewImage),
                static_cast<int>(MonitorRole::InstalledType),
                static_cast<int>(MonitorRole::AppID) });
    }
}

/*!
  \brief If successful this function returns an AppID. Otherwhise std::nullopt.
*/
std::optional<QString> MonitorListModel::getAppIDByMonitorIndex(const int index) const
{
    for (auto& monitor : m_monitorList) {
        if (monitor.m_index == index && monitor.m_activeWallpaper) {
            return { monitor.m_activeWallpaper->appID() };
        }
    }
    return std::nullopt;
}

/*!
  \brief Removes all items and loads them vida loadMonitors() again.
*/
void MonitorListModel::reset()
{
    beginResetModel();
    m_monitorList.clear();
    m_monitorList.squeeze();
    endResetModel();
    loadMonitors();
}

}
