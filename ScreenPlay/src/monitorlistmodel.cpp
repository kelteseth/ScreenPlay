// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#include "ScreenPlay/monitorlistmodel.h"
#include "ScreenPlay/screenplaywallpaper.h"

#ifdef Q_OS_WIN
#include "windowsintegration.h"
#endif
#include <QGuiApplication>

#include <QDebug>
#include <QRandomGenerator>

#include "ScreenPlay/CMakeVariables.h"
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
    static_assert(SCREENPLAY_DEPLOY_VERSION == 0 || !std::is_same_v<decltype(m_useMockMonitors), bool>,
        "Mock monitors should not be available in deploy version");

    auto* guiAppInst = dynamic_cast<QGuiApplication*>(QGuiApplication::instance());
    connect(guiAppInst, &QGuiApplication::screenAdded, this, &MonitorListModel::screenAdded);
    connect(guiAppInst, &QGuiApplication::screenRemoved, this, &MonitorListModel::screenRemoved);

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
    }
    return QVariant();
}

/*!
  \brief Reloads the listmodel of monitors.
*/
void MonitorListModel::loadMonitors()
{
    if (m_useMockMonitors) {

        // Generate a random index
        const int selectedMockIndex = QRandomGenerator::global()->bounded(m_mockMonitorList.size());
        const auto& mockMonitorList = m_mockMonitorList[selectedMockIndex];
        qDebug() << "Using mock" << selectedMockIndex
                 << "of" << m_mockMonitorList.size()
                 << " with monitor count:" << mockMonitorList.count();
        beginInsertRows(index(rowCount()), rowCount(), rowCount() + mockMonitorList.count() - 1);
        // Use the randomly selected mock monitor setup
        for (const auto& monitor : mockMonitorList) {
            m_monitorList.append(monitor);
            qDebug() << "Adding mock monitor: " << monitor.m_monitorIndex << monitor.m_geometry;
        }
        endInsertRows();

        emit monitorReloadCompleted();
        return;
    }

#ifdef Q_OS_WIN
    auto monitors = WindowsIntegration().getAllMonitors();

    // This offset lets us center the monitor selection view in the center
    int offsetX = 0;
    int offsetY = 0;

    for (auto& monitor : monitors) {
        const int x = monitor.position.left;
        const int y = monitor.position.top;
        if (x < 0) {
            offsetX += (x * -1);
        }
        if (y < 0) {
            offsetY += (y * -1);
        }
    }

    for (int i = 0; auto& monitor : monitors) {
        const int width = std::abs(monitor.position.right - monitor.position.left);
        const int height = std::abs(monitor.position.top - monitor.position.bottom);
        const int x = monitor.position.left;
        const int y = monitor.position.top;
        QRect geometry(
            x + offsetX,
            y + offsetY,
            width,
            height);
        beginInsertRows(index(rowCount()), m_monitorList.size(), m_monitorList.size());
        m_monitorList.append(Monitor { i, geometry });
        endInsertRows();
        i++;
    }
#else
    int offsetX = 0;
    int offsetY = 0;

    for (int i = 0; i < QGuiApplication::screens().count(); i++) {
        QScreen* screen = QGuiApplication::screens().at(i);
        if (screen->availableGeometry().x() < 0) {
            offsetX += (screen->availableGeometry().x() * -1);
        }
        if (screen->availableGeometry().y() < 0) {
            offsetY += (screen->availableGeometry().y() * -1);
        }
    }

    for (int i = 0; i < QGuiApplication::screens().count(); i++) {
        QScreen* screen = QGuiApplication::screens().at(i);

        // Sometimes we get invalid monitors on Windows. I don't know why...
        if (screen->geometry().width() == 0 || screen->geometry().height() == 0)
            continue;

        beginInsertRows(index(rowCount()), m_monitorList.size(), m_monitorList.size());
        m_monitorList.append(Monitor { i, screen->geometry() });
        endInsertRows();
    }
#endif

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
    if (role == static_cast<int>(MonitorRole::PreviewImage)) {
        m_monitorList[index.column()].m_wallpaperPreviewImage = value.toString();
        emit dataChanged(index, index, { role });
    }
    if (role == static_cast<int>(MonitorRole::AppID)) {
        m_monitorList[index.column()].m_appID = value.toString();
        emit dataChanged(index, index, { role });
    }
    if (role == static_cast<int>(MonitorRole::AppState)) {
        m_monitorList[index.column()].m_appState = static_cast<ScreenPlayEnums::AppState>(value.toInt());
        emit dataChanged(index, index, { role });
    }
    if (role == static_cast<int>(MonitorRole::InstalledType)) {
        m_monitorList[index.column()].m_installedType = static_cast<ContentTypes::InstalledType>(value.toInt());
        emit dataChanged(index, index, { role });
    }
    return true;
}
}

#include "moc_monitorlistmodel.cpp"
