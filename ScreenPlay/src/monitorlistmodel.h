#pragma once

#include "projectsettingslistmodel.h"
#include <QAbstractListModel>
#include <QApplication>
#include <QDebug>
#include <QGuiApplication>
#include <QRect>
#include <QScreen>
#include <QSharedPointer>
#include <QSize>
#include <QString>
#include <QVector>

#include <qt_windows.h>

/*!
    \class Monitor List Model
    \brief Listsmodel for all active monitors

    \todo
            - Add event when monitor count changed.

*/
namespace ScreenPlay {
struct Monitor;

class MonitorListModel : public QAbstractListModel {
    Q_OBJECT

public:
    explicit MonitorListModel(QObject* parent = nullptr);

    QHash<int, QByteArray> roleNames() const override;

    enum MonitorRole {
        IDRole = Qt::UserRole,
        NameRole,
        SizeRole,
        AvailableGeometryRole,
        AvailableVirtualGeometryRole,
        NumberRole,
        GeometryRole,
        ModelRole,
        ManufacturerRole,
        PreviewImageRole,
        IsWallpaperActiveRole,
    };
    Q_ENUM(MonitorRole)

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    void loadMonitors();
    void screenAdded(QScreen* screen);
    void screenRemoved(QScreen* screen);

    void setWallpaperActiveMonitor(QScreen* screen, QString fullPreviewImagePath);

public slots:
    QRect getAbsoluteDesktopSize();
    void reloadMonitors();
    int size();
    void wallpaperRemoved();

private:
    QVector<Monitor> m_monitorList;
    const QGuiApplication* const m_qGuiApplication;
    QVector<QSharedPointer<ProjectSettingsListModel>> m_plm;

signals:
    void monitorReloadCompleted();
    void setNewActiveMonitor(int index, QString path);
};

struct Monitor {

    Monitor() {}
    Monitor(QString manufacturer, QString model, QString name, QSize size, QRect availableGeometry, int number, QRect availableVirtualGeometry, QRect geometry, QScreen* screen);

    QString m_id;
    QString m_name;
    QString m_manufacturer;
    QString m_model;
    QSize m_size;
    QRect m_availableGeometry;
    QRect m_availableVirtualGeometry;
    QRect m_geometry;
    int m_number;
    bool m_isVirtualDesktop;

    bool m_isWallpaperActive = false;
    QString m_wallpaperPreviewPath;

    QScreen* m_screen = nullptr;
};
}
