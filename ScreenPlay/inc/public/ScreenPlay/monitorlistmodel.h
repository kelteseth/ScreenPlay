// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#pragma once

#include <QAbstractListModel>

#include <QRect>
#include <QScreen>
#include <QSize>
#include <QString>
#include <QVector>
#include <memory>
#include <optional>

#include "ScreenPlay/wallpapertimelinesection.h"
#include "ScreenPlayUtil/contenttypes.h"

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
    QString m_wallpaperPreviewImage;
    QString m_appID;
    ContentTypes::InstalledType m_installedType = ContentTypes::InstalledType::Unknown;
};

class MonitorListModel : public QAbstractListModel {
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")
    Q_CLASSINFO("RegisterEnumClassesUnscoped", "false")

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

    std::optional<QString> getAppIDByMonitorIndex(const int index) const;
    Q_INVOKABLE void reset();

    Q_INVOKABLE QRect absoluteDesktopSize() const;
    Q_INVOKABLE QSize totalDesktopSize() const;

    bool setData(const QModelIndex& index, const QVariant& value, int role) override;
signals:
    void monitorReloadCompleted();
    void setNewActiveMonitor(int index, QString path);
    void monitorConfigurationChanged();

private slots:

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
    bool m_useMockMonitors = false;
    QVector<QVector<Monitor>> m_mockMonitorList;
};
}
