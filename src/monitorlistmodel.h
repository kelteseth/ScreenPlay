#ifndef MONITORLISTMODEL_H
#define MONITORLISTMODEL_H

#include <QAbstractListModel>
#include <QSize>
#include <QRect>
#include <QString>
#include <QVector>
#include <QScreen>
#include <QDebug>
#include <QApplication>


class Monitor;

class MonitorListModel : public QAbstractListModel
{
    Q_OBJECT


public:
    explicit MonitorListModel(QObject *parent = nullptr);

    QHash<int, QByteArray> roleNames() const override;
    Q_INVOKABLE QRect getAbsoluteDesktopSize();

    enum MonitorRole {
        NameRole,
        SizeRole,
        AvailableGeometryRole,
        AvailableVirtualGeometryRole,
        NumberRole,
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

    int size();
    bool getMonitorListItemAt(int position, Monitor *monitor);

    float GetHighestMonitorYValue();

private:
    QVector<Monitor> m_monitorList;
};

class Monitor {

public:
    Monitor();
    Monitor(QString manufacturer, QString model, QString name, QSize size, QRect availableGeometry, int number, QRect availableVirtualGeometry);

    QString m_id;
    QString m_name;
    QSize m_size;
    QRect m_availableGeometry;
    QRect m_availableVirtualGeometry;
    int m_number;
    bool m_isVirtualDesktop;

};
#endif // MONITORLISTMODEL_H
