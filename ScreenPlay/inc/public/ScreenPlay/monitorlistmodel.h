// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#pragma once

#include <QAbstractListModel>

#include <QDebug>
#include <QRect>
#include <QScreen>
#include <QSize>
#include <QString>
#include <QVector>

#include "ScreenPlay/projectsettingslistmodel.h"
#include "ScreenPlay/screenplaywallpaper.h"
#include "ScreenPlay/screenplaywidget.h"

#ifdef Q_OS_WIN
#include <qt_windows.h>
#endif
#include <memory>
#include <optional>

namespace ScreenPlay {

struct Monitor {

    Monitor(
        const int index,
        const QRect& geometry)
    {
        m_index = index;
        m_geometry = geometry;
    }

    int m_index { 0 };
    QRect m_geometry;
    std::shared_ptr<ScreenPlayWallpaper> m_activeWallpaper { nullptr };
};

class MonitorListModel : public QAbstractListModel {
    Q_OBJECT
    QML_ELEMENT

public:
    explicit MonitorListModel(QObject* parent = nullptr);

    enum class MonitorRole {
        AppID = Qt::UserRole,
        Index,
        Geometry,
        PreviewImage,
        InstalledType,
    };
    Q_ENUM(MonitorRole)

    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    void setWallpaperMonitor(
        const std::shared_ptr<ScreenPlayWallpaper>& wallpaper,
        const QVector<int> monitors);

    std::optional<QString> getAppIDByMonitorIndex(const int index) const;

signals:
    void monitorReloadCompleted();
    void setNewActiveMonitor(int index, QString path);
    void monitorConfigurationChanged();

public slots:
    void reset();
    void clearActiveWallpaper();
    void closeWallpaper(const QString& appID);
    QRect absoluteDesktopSize() const;

    void screenAdded(QScreen* screen)
    {
        emit monitorConfigurationChanged();
        reset();
    }
    void screenRemoved(QScreen* screen)
    {
        emit monitorConfigurationChanged();
        reset();
    }

private:
    void loadMonitors();

private:
    QVector<Monitor> m_monitorList;
};

}
