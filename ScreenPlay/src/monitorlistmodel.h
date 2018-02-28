#pragma once

#include <QAbstractListModel>
#include <QSize>
#include <QRect>
#include <QString>
#include <QVector>
#include <QScreen>
#include <QDebug>
#include <QApplication>
#include <QGuiApplication>

class Monitor;

class MonitorListModel : public QAbstractListModel
{
    Q_OBJECT


public:
    explicit MonitorListModel(QGuiApplication *guiapp, QObject *parent = nullptr);

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
    };
    Q_ENUM(MonitorRole)

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role = Qt::EditRole) override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Editable:
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole) override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

    // Add data:
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    // Remove data:
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    void loadMonitors();

    bool getMonitorListItemAt(int position, Monitor *monitor);

    void screenAdded(QScreen *screen);
    void screenRemoved(QScreen *screen);

    void reset();

private:
    QVector<Monitor> m_monitorList;
    QGuiApplication* m_qGuiApplication;

signals:
    void monitorReloadCompleted();
};

class Monitor {

public:
    Monitor();
    Monitor(QString manufacturer, QString model, QString name, QSize size, QRect availableGeometry, int number, QRect availableVirtualGeometry,QRect geometry, QScreen *screen);

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
    QScreen* m_screen = nullptr;


};

