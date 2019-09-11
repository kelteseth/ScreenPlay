#pragma once

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

#include "projectsettingslistmodel.h"
//#include "screenplaywallpaper.h"
//#include "screenplaywidget.h"

#include <qt_windows.h>

/*!
    \class Monitor List Model
    \brief Listsmodel for all active monitors

*/
namespace ScreenPlay {

struct Monitor {

    Monitor(
        const QString& manufacturer,
        const QString& model,
        const QString& name,
        const QString& previewImage,
        const QSize& size,
        const QRect& availableGeometry,
        const int number,
        const QRect& availableVirtualGeometry,
        const QRect& geometry,
        QScreen* screen)
    {
        m_screen = screen;
        m_name = name;
        m_size = size;
        m_geometry = geometry;
        m_availableGeometry = availableGeometry;
        m_manufacturer = manufacturer;
        m_model = model;
        m_availableVirtualGeometry = availableVirtualGeometry;
        m_number = number;
        m_previewImage = previewImage;

        // TODO 32: Use a better way to create an id
        // because name and manufacturer are allways empty
        m_monitorID = QString::number(size.width())
            + "x" + QString::number(size.height())
            + "_" + QString::number(availableGeometry.x())
            + "x" + QString::number(availableGeometry.y());
    }

    QString m_appID;
    QString m_monitorID;
    QString m_name;
    QString m_manufacturer;
    QString m_model;
    QSize m_size;
    QString m_previewImage;

    QRect m_availableGeometry;
    QRect m_availableVirtualGeometry;
    QRect m_geometry;

    int m_number { 0 };

    bool m_isVirtualDesktop { false };
    bool m_isLooping { true };
    bool m_isWallpaperActive { false };

    QScreen* m_screen { nullptr };
};

class MonitorListModel : public QAbstractListModel {
    Q_OBJECT

public:
    explicit MonitorListModel(QObject* parent = nullptr);

    enum class MonitorRole {
        AppID = Qt::UserRole,
        MonitorID,
        Name,
        Size,
        AvailableGeometry,
        AvailableVirtualGeometry,
        Number,
        Geometry,
        Model,
        Manufacturer,
        PreviewImage,
        IsWallpaperActive,
    };
    Q_ENUM(MonitorRole)

    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    void setWallpaperActiveMonitor(const QVector<int> monitors,
        const QString& appID,
        const QString& fullPreviewImagePath);

signals:
    void monitorReloadCompleted();
    void setNewActiveMonitor(int index, QString path);
    void monitorListChanged();

public slots:
    void reloadMonitors();
    void clearActiveWallpaper();

    void screenAdded(QScreen* screen)
    {
        Q_UNUSED(screen)
        qDebug() << "screenAdded";
        reloadMonitors();
    }
    void screenRemoved(QScreen* screen)
    {
        Q_UNUSED(screen)
        qDebug() << "screenRemoved";
        reloadMonitors();
    }

    QRect getAbsoluteDesktopSize()
    {
        return m_monitorList.at(0).m_availableVirtualGeometry;
    }

    QString getAppIDByMonitor(const int monitor)
    {
        return m_monitorList.at(monitor).m_appID;
    }

private:
    void loadMonitors();

private:
    QVector<Monitor> m_monitorList;
    QVector<QSharedPointer<ProjectSettingsListModel>> m_plm;
};

}
