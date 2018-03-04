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

class Monitor;

class MonitorListModel : public QAbstractListModel {
    Q_OBJECT

public:
    explicit MonitorListModel(QGuiApplication* guiapp, QObject* parent = nullptr);

    QHash<int, QByteArray> roleNames() const override;
    Q_INVOKABLE QRect getAbsoluteDesktopSize();
    Q_INVOKABLE void reloadMonitors();
    Q_INVOKABLE int size();

    enum MonitorRole {
        IDRole,
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

    bool getMonitorListItemAt(int position, Monitor* monitor);
    void loadMonitors();
    void screenAdded(QScreen* screen);
    void screenRemoved(QScreen* screen);
    void reset();

    void setWallpaperActiveMonitor(QScreen* screen, QString fullPreviewImagePath);

private:
    QVector<Monitor> m_monitorList;
    QGuiApplication* m_qGuiApplication;
    QVector<QSharedPointer<ProjectSettingsListModel>> m_plm;

signals:
    void monitorReloadCompleted();
    void setNewActiveMonitor(int index, QString path);
};

class Monitor {

public:
    Monitor();
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
