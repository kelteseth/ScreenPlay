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
#include "ScreenPlayCore/contenttypes.h"
#include "ScreenPlayCore/globalenums.h"

namespace ScreenPlay {

struct Monitor {

    Monitor(
        const int monitorIndex,
        const QRect& geometry,
        const QString& name = "")
    {
        m_monitorIndex = monitorIndex;
        m_geometry = geometry;
        m_name = name;
    }

    int m_monitorIndex { 0 };
    QRect m_geometry;
    QString m_wallpaperPreviewImage;
    QString m_appID;
    QString m_name;
    ScreenPlayEnums::AppState m_appState = ScreenPlayEnums::AppState::Inactive;
    ContentTypes::InstalledType m_installedType = ContentTypes::InstalledType::Unknown;
};

class MonitorListModel : public QAbstractListModel {
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")
    Q_PROPERTY(int selectedIndex READ selectedIndex WRITE setSelectedIndex NOTIFY selectedIndexChanged FINAL)
    Q_CLASSINFO("RegisterEnumClassesUnscoped", "false")

public:
    explicit MonitorListModel(QObject* parent = nullptr);

    enum class MonitorRole {
        AppID = Qt::UserRole,
        AppState,
        // Explicit _Monitor_Index so we do not
        // accidentally use a list model index!
        MonitorIndex,
        Geometry,
        PreviewImage,
        InstalledType,
        Name,
    };
    Q_ENUM(MonitorRole)

    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    Q_INVOKABLE QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    Q_INVOKABLE void reset();
    Q_INVOKABLE QRect absoluteDesktopSize() const;
    Q_INVOKABLE QSize totalDesktopSize() const;

    bool setData(const QModelIndex& index, const QVariant& value, int role) override;
    static QVector<Monitor> getSystemMonitors();

    int selectedIndex() const;
public slots:
    void setSelectedIndex(int newSelectedIndex);

signals:
    void monitorReloadCompleted();
    void setNewActiveMonitor(int index, QString path);
    void monitorConfigurationChanged();

    void selectedIndexChanged(int selectedIndex);

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
    int m_selectedIndex = -1;
};
}
