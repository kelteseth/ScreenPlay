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

#include <QApplication>
#include <QDebug>
#include <QObject>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickView>
#include <QScreen>
#include <QSettings>
#include <QString>
#include <QTimer>
#include <QVector>
#include <memory>

#include <qt_windows.h>

#include "basewindow.h"
#include "windowsdesktopproperties.h"

struct MonitorRects {
    std::vector<QRect> rcMonitors;

    MonitorRects()
    {
        EnumDisplayMonitors(0, 0, MonitorEnum, (LPARAM)this);
    }

    static BOOL CALLBACK MonitorEnum(HMONITOR hMon, HDC hdc, LPRECT lprcMonitor, LPARAM pData)
    {
        MonitorRects* pThis = reinterpret_cast<MonitorRects*>(pData);
        RECT monitor = (RECT)*lprcMonitor;
        pThis->rcMonitors.push_back(QRect(QPoint { monitor.top, monitor.left }, QPoint { monitor.bottom, monitor.right }));

        return TRUE;
    }
};

class WinWindow : public BaseWindow {
    Q_OBJECT

    Q_PROPERTY(WindowsDesktopProperties* windowsDesktopProperties READ windowsDesktopProperties WRITE setWindowsDesktopProperties NOTIFY windowsDesktopPropertiesChanged)

public:
    explicit WinWindow(
        const QVector<int>& activeScreensList,
        const QString& projectPath,
        const QString& id,
        const QString& volume,
        const QString& fillmode,
        const bool checkWallpaperVisible);

    WindowsDesktopProperties* windowsDesktopProperties() const { return m_windowsDesktopProperties.get(); }

public slots:
    void setVisible(bool show) override;
    void destroyThis() override;
    void terminate();
    void clearComponentCache();

    void setWindowsDesktopProperties(WindowsDesktopProperties* windowsDesktopProperties)
    {
        if (m_windowsDesktopProperties.get() == windowsDesktopProperties)
            return;

        m_windowsDesktopProperties.reset(windowsDesktopProperties);
        emit windowsDesktopPropertiesChanged(m_windowsDesktopProperties.get());
    }

signals:
    void windowsDesktopPropertiesChanged(WindowsDesktopProperties* windowsDesktopProperties);

private:
    void calcOffsets();
    void setupWallpaperForOneScreen(int activeScreen);
    void setupWallpaperForAllScreens();
    void setupWallpaperForMultipleScreens(const QVector<int>& activeScreensList);
    void setupWindowMouseHook();
    bool searchWorkerWindowToParentTo();

private slots:
    void checkForFullScreenWindow();

private:
    int m_windowOffsetX = 0;
    int m_windowOffsetY = 0;
    QQuickView m_window;
    HWND m_windowHandle;
    HWND m_windowHandleWorker;
    QTimer m_checkForFullScreenWindowTimer;
    std::unique_ptr<WindowsDesktopProperties> m_windowsDesktopProperties;
};
