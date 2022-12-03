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

#include <QColor>
#include <QDebug>
#include <QList>
#include <QObject>
#include <QPoint>
#include <QSettings>
#include <QString>
#include <QSize>
#include <qqml.h>
#include <qt_windows.h>

class WindowsDesktopProperties : public QObject {
    Q_OBJECT

public:
    explicit WindowsDesktopProperties(QObject* parent = nullptr);

    Q_PROPERTY(int windowsVersion READ windowsVersion WRITE setWindowsVersion NOTIFY windowsVersionChanged)
    Q_PROPERTY(QSize defaultWallpaperSize READ defaultWallpaperSize WRITE setDefaultWallpaperSize NOTIFY defaultWallpaperSizeChanged)

    Q_PROPERTY(QString wallpaperPath READ wallpaperPath WRITE setWallpaperPath NOTIFY wallpaperPathChanged)
    Q_PROPERTY(QPoint position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(bool isTiled READ isTiled WRITE setIsTiled NOTIFY isTiledChanged)
    Q_PROPERTY(int wallpaperStyle READ wallpaperStyle WRITE setWallpaperStyle NOTIFY wallpaperStyleChanged)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)

    /* WallpaperStyle integer values in the windows registry
     * 10 -> fill
     * 6  -> fit
     * 2  -> stretch
     * 0  -> tile   -> isTiled true
     * 0  -> center -> isTiled false WTF Windows?
     * 22 -> span
     */

    QString wallpaperPath() const { return m_wallpaperPath; }
    QPoint position() const { return m_position; }
    bool isTiled() const { return m_isTiled; }
    int wallpaperStyle() const { return m_wallpaperStyle; }
    QColor color() const { return m_color; }
    int windowsVersion() const { return m_windowsVersion; }
    QSize defaultWallpaperSize() const { return m_defaultWallpaperSize; }

signals:
    void wallpaperPathChanged(QString wallpaperPath);
    void positionChanged(QPoint position);
    void isTiledChanged(bool isTiled);
    void wallpaperStyleChanged(int wallpaperStyle);
    void colorChanged(QColor color);
    void windowsVersionChanged(int windowsVersion);

    void defaultWallpaperSizeChanged();

public slots:
    void setDefaultWallpaperSize(const QSize& newDefaultWallpaperSize)
    {
        if (m_defaultWallpaperSize == newDefaultWallpaperSize)
            return;
        m_defaultWallpaperSize = newDefaultWallpaperSize;
        emit defaultWallpaperSizeChanged();
    }

    void setWallpaperPath(QString wallpaperPath)
    {
        if (m_wallpaperPath == wallpaperPath)
            return;

        m_wallpaperPath = wallpaperPath;
        emit wallpaperPathChanged(m_wallpaperPath);
    }
    void setPosition(QPoint position)
    {
        if (m_position == position)
            return;

        m_position = position;
        emit positionChanged(m_position);
    }
    void setIsTiled(bool isTiled)
    {
        if (m_isTiled == isTiled)
            return;

        m_isTiled = isTiled;
        emit isTiledChanged(m_isTiled);
    }
    void setWallpaperStyle(int wallpaperStyle)
    {
        if (m_wallpaperStyle == wallpaperStyle)
            return;

        m_wallpaperStyle = wallpaperStyle;
        emit wallpaperStyleChanged(m_wallpaperStyle);
    }
    void setColor(QColor color)
    {
        if (m_color == color)
            return;

        m_color = color;
        emit colorChanged(m_color);
    }

    void setWindowsVersion(int windowsVersion)
    {
        if (m_windowsVersion == windowsVersion)
            return;

        m_windowsVersion = windowsVersion;
        emit windowsVersionChanged(m_windowsVersion);
    }

private:
    QString m_wallpaperPath;
    QPoint m_position;
    bool m_isTiled;
    int m_wallpaperStyle;
    QColor m_color;
    int m_windowsVersion;
    QSize m_defaultWallpaperSize;
};
