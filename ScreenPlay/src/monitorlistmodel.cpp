// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#include "ScreenPlay/monitorlistmodel.h"
#include "ScreenPlay/screenplaywallpaper.h"

#ifdef Q_OS_WIN
#include "ScreenPlayCore/windowsutils.h"
#endif
#include <QGuiApplication>

#include <QDebug>
#include <QRandomGenerator>

#include "CMakeVariables.h"
#include <type_traits>

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
    if (!SCREENPLAY_DEPLOY_VERSION) {
        // Setup 1: Two Full HD monitors
        m_mockMonitorList.append({ Monitor(0, QRect(0, 0, 1920, 1080)),
            Monitor(1, QRect(1920, 0, 1920, 1080)) });

        // Setup 2: One 4K monitor and one Full HD above
        m_mockMonitorList.append({ Monitor(0, QRect(0, 0, 3840, 2160)),
            Monitor(1, QRect((-3840 / 2), 3840, 1920, 1080)) });

        // Setup 3: One WQHD and one Full HD
        m_mockMonitorList.append({ Monitor(0, QRect(0, 0, 2560, 1440)),
            Monitor(1, QRect(2560, 0, 1920, 1080)) });

        // Setup 4: Three Full HD monitors (two horizontal, one vertical)
        m_mockMonitorList.append({ Monitor(0, QRect(0, 0, 1920, 1080)),
            Monitor(1, QRect(1920, 0, 1920, 1080)),
            Monitor(2, QRect(3840, 0, 1080, 1920)) });

        // Setup 5: One ultrawide and one regular monitor
        m_mockMonitorList.append({ Monitor(0, QRect(0, 0, 3440, 1440)),
            Monitor(1, QRect(3440, 0, 1920, 1080)) });
    }

    auto* guiAppInst = dynamic_cast<QGuiApplication*>(QGuiApplication::instance());
    connect(guiAppInst, &QGuiApplication::screenAdded, this, &MonitorListModel::screenAdded);
    connect(guiAppInst, &QGuiApplication::screenRemoved, this, &MonitorListModel::screenRemoved);

    loadMonitors();
}

/*!
    \brief Returns the variable names for QML.
*/
QHash<int, QByteArray> MonitorListModel::roleNames() const
{
    static const QHash<int, QByteArray> roles {
        { static_cast<int>(MonitorRole::AppID), "appID" },
        { static_cast<int>(MonitorRole::AppState), "appState" },
        { static_cast<int>(MonitorRole::MonitorIndex), "monitorIndex" },
        { static_cast<int>(MonitorRole::Geometry), "geometry" },
        { static_cast<int>(MonitorRole::PreviewImage), "previewImage" },
        { static_cast<int>(MonitorRole::InstalledType), "installedType" },
        { static_cast<int>(MonitorRole::Name), "name" },
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

    if (row > rowCount())
        return QVariant();

    switch (roleEnum) {
    case MonitorRole::AppID:
        return m_monitorList.at(row).m_appID;
    case MonitorRole::AppState:
        return QVariant::fromValue(m_monitorList.at(row).m_appState);
    case MonitorRole::MonitorIndex:
        return m_monitorList.at(row).m_monitorIndex;
    case MonitorRole::Geometry:
        return m_monitorList.at(row).m_geometry;
    case MonitorRole::InstalledType:
        return QVariant::fromValue(m_monitorList.at(row).m_installedType);
    case MonitorRole::PreviewImage:
        return m_monitorList.at(row).m_wallpaperPreviewImage;
    case MonitorRole::Name:
        return m_monitorList.at(row).m_name;
    }
    return QVariant();
}

/*!
  \brief Reloads the listmodel of monitors.
*/
void MonitorListModel::loadMonitors()
{
    if (m_useMockMonitors) {
        const int selectedMockIndex = QRandomGenerator::global()->bounded(m_mockMonitorList.size());
        const auto& mockMonitorList = m_mockMonitorList[selectedMockIndex];
        qDebug() << "Using mock" << selectedMockIndex
                 << "of" << m_mockMonitorList.size()
                 << " with monitor count:" << mockMonitorList.count();

        beginInsertRows(index(rowCount()), rowCount(), rowCount() + mockMonitorList.count() - 1);
        for (const auto& monitor : mockMonitorList) {
            m_monitorList.append(monitor);
            qDebug() << "Adding mock monitor: " << monitor.m_monitorIndex << monitor.m_geometry;
        }
        endInsertRows();
    } else {
        auto monitors = getSystemMonitors();
        beginInsertRows(index(rowCount()), m_monitorList.size(), m_monitorList.size() + monitors.size() - 1);
        m_monitorList = monitors;
        endInsertRows();
    }

    emit monitorReloadCompleted();
}

/*!
 * \brief MonitorListModel::getAbsoluteDesktopSize
 * \return
 */
QSize MonitorListModel::totalDesktopSize() const
{
    QRect totalRect;
    for (const auto& monitor : m_monitorList) {
        totalRect = totalRect.united(monitor.m_geometry);
    }
    return totalRect.size();
}

QRect MonitorListModel::absoluteDesktopSize() const
{
    QRect totalRect;
    for (const auto& monitor : m_monitorList) {
        totalRect = totalRect.united(monitor.m_geometry);
    }
    return totalRect;
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

bool MonitorListModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    // qDebug() << "setData" << index << value << static_cast<MonitorRole>(role);

    if (!index.isValid() || index.row() >= m_monitorList.size())
        return false;

    if (role == static_cast<int>(MonitorRole::PreviewImage)) {
        m_monitorList[index.row()].m_wallpaperPreviewImage = value.toString();
        emit dataChanged(index, index, { role });
        return true;
    }
    if (role == static_cast<int>(MonitorRole::AppID)) {
        m_monitorList[index.row()].m_appID = value.toString();
        emit dataChanged(index, index, { role });
        return true;
    }
    if (role == static_cast<int>(MonitorRole::AppState)) {
        m_monitorList[index.row()].m_appState = static_cast<ScreenPlayEnums::AppState>(value.toInt());
        emit dataChanged(index, index, { role });
        return true;
    }
    if (role == static_cast<int>(MonitorRole::InstalledType)) {
        m_monitorList[index.row()].m_installedType = static_cast<ContentTypes::InstalledType>(value.toInt());
        emit dataChanged(index, index, { role });
        return true;
    }
    return false;
}

QVector<Monitor> MonitorListModel::getSystemMonitors()
{
    QVector<Monitor> monitors;

    // Calculate offset for centering monitor selection view
    int offsetX = 0;
    int offsetY = 0;

#ifdef Q_OS_WIN
    // Qt has been prooven to be unreliable
    WindowsUtils windowsUtils;
    std::vector<WindowsMonitor> winMonitors = windowsUtils.getAllMonitors();

    // Calculate offsets for Windows
    for (const auto& monitor : winMonitors) {
        const int x = monitor.position.left;
        const int y = monitor.position.top;
        if (x < 0) {
            offsetX += (x * -1);
        }
        if (y < 0) {
            offsetY += (y * -1);
        }
    }

    // Create monitor objects
    int index = 0;
    for (const auto& monitor : winMonitors) {
        const int width = std::abs(monitor.position.right - monitor.position.left);
        const int height = std::abs(monitor.position.top - monitor.position.bottom);
        const int x = monitor.position.left;
        const int y = monitor.position.top;

        QRect geometry(x + offsetX, y + offsetY, width, height);
        monitors.append(Monitor(index++, geometry, QString::fromStdString(monitor.name)));
    }
#else
    // Calculate offsets for other platforms
    for (int i = 0; i < QGuiApplication::screens().count(); i++) {
        QScreen* screen = QGuiApplication::screens().at(i);
        if (screen->availableGeometry().x() < 0) {
            offsetX += (screen->availableGeometry().x() * -1);
        }
        if (screen->availableGeometry().y() < 0) {
            offsetY += (screen->availableGeometry().y() * -1);
        }
    }

    // Create monitor objects
    for (int i = 0; i < QGuiApplication::screens().count(); i++) {
        QScreen* screen = QGuiApplication::screens().at(i);

        // Skip invalid monitors
        if (screen->geometry().width() == 0 || screen->geometry().height() == 0)
            continue;

        monitors.append(Monitor(i, screen->geometry()));
    }
#endif

    return monitors;
}

int MonitorListModel::selectedIndex() const
{
    return m_selectedIndex;
}

void MonitorListModel::setSelectedIndex(int newSelectedIndex)
{
    if (m_selectedIndex == newSelectedIndex)
        return;
    m_selectedIndex = newSelectedIndex;
    emit selectedIndexChanged(m_selectedIndex);
}
}

#include "moc_monitorlistmodel.cpp"
