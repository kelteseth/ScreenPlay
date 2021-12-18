/****************************************************************************
**
** Copyright (C) 2020 Elias Steurer (Kelteseth)
** Contact: https://screen-play.app
**
** This file is part of ScreenPlay. ScreenPlay is licensed under a dual license in
** order to ensure its sustainability. When you contribute to ScreenPlay
** you accept that your work will be available under the two following licenses:
**
** $SCREENPLAY_BEGIN_LICENSE$
**
** #### Affero General Public License Usage (AGPLv3)
** Alternatively, this file may be used under the terms of the GNU Affero
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file "ScreenPlay License.md" included in the
** packaging of this App. Please review the following information to
** ensure the GNU Affero Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/agpl-3.0.en.html.
**
** #### Commercial License
** This code is owned by Elias Steurer. By changing/adding to the code you agree to the
** terms written in:
**  * Legal/corporate_contributor_license_agreement.md - For corporate contributors
**  * Legal/individual_contributor_license_agreement.md - For individual contributors
**
** #### Additional Limitations to the AGPLv3 and Commercial Lincese
** This License does not grant any rights in the trademarks,
** service marks, or logos.
**
**
** $SCREENPLAY_END_LICENSE$
**
****************************************************************************/

#pragma once

#include <QAbstractListModel>
#include <QApplication>
#include <QDebug>
#include <QRect>
#include <QScreen>
#include <QSize>
#include <QString>
#include <QVector>

#include "projectsettingslistmodel.h"
#include "screenplaywallpaper.h"
#include "screenplaywidget.h"

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
