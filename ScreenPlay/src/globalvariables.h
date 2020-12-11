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

#include <QMetaType>
#include <QObject>
#include <QQmlEngine>
#include <QStandardPaths>
#include <QUrl>
#include <QVersionNumber>

namespace ScreenPlay {

/*!
    \class ScreenPlay::GlobalVariables
    \inmodule ScreenPlay
    \brief GlobalVariables.

    A header only class used only for storing some global variables like localStoragePath.

*/

namespace InstalledType {
    Q_NAMESPACE

    enum class InstalledType {
        Unknown,

        VideoWallpaper,
        QMLWallpaper,
        HTMLWallpaper,
        GodotWallpaper,
        GifWallpaper,
        WebsiteWallpaper,

        QMLWidget,
        HTMLWidget,
    };
    Q_ENUM_NS(InstalledType)

    static bool isWallpaper(const InstalledType type)
    {
        return (type == InstalledType::VideoWallpaper
            || type == InstalledType::QMLWallpaper
            || type == InstalledType::HTMLWallpaper
            || type == InstalledType::GifWallpaper
            || type == InstalledType::WebsiteWallpaper
            || type == InstalledType::GodotWallpaper);
    }

    static bool isWidget(const InstalledType type)
    {
        return (type == InstalledType::QMLWidget || type == InstalledType::HTMLWidget);
    }
}

namespace SearchType {
    Q_NAMESPACE

    enum class SearchType {
        All,
        Text,
        Scene, //QML, HTML, Godot, Gif, Website wallpaper
        Wallpaper,
        Widget,
    };
    Q_ENUM_NS(SearchType)

}

namespace FillMode {
    Q_NAMESPACE

    enum class FillMode {
        Stretch,
        Fill,
        Contain,
        Cover,
        Scale_Down
    };
    Q_ENUM_NS(FillMode)

}

class GlobalVariables : public QObject {
    Q_OBJECT

    Q_PROPERTY(QVersionNumber version READ version CONSTANT)
    Q_PROPERTY(QUrl localStoragePath READ localStoragePath WRITE setLocalStoragePath NOTIFY localStoragePathChanged)
    Q_PROPERTY(QUrl localSettingsPath READ localSettingsPath WRITE setLocalSettingsPath NOTIFY localSettingsPathChanged)
    Q_PROPERTY(QUrl wallpaperExecutablePath READ wallpaperExecutablePath WRITE setWallpaperExecutablePath NOTIFY wallpaperExecutablePathChanged)
    Q_PROPERTY(QUrl widgetExecutablePath READ widgetExecutablePath WRITE setWidgetExecutablePath NOTIFY widgetExecutablePathChanged)

public:
    explicit GlobalVariables(QObject* parent = nullptr);

    /*!
        \property GlobalVariables::localStoragePath
        \brief Returns the localStoragePath.
    */
    QUrl localStoragePath() const
    {
        return m_localStoragePath;
    }
    /*!
        \property GlobalVariables::localSettingsPath
        \brief Returns the localSettingsPath.
    */
    QUrl localSettingsPath() const
    {
        return m_localSettingsPath;
    }
    /*!
        \property GlobalVariables::wallpaperExecutablePath
        \brief  Returns the wallpaperExecutablePath. This only differes in development builds.
    */
    QUrl wallpaperExecutablePath() const
    {
        return m_wallpaperExecutablePath;
    }
    /*!
        \property GlobalVariables::widgetExecutablePath
        \brief Returns the widgetExecutablePath. This only differes in development builds.
    */
    QUrl widgetExecutablePath() const
    {
        return m_widgetExecutablePath;
    }
    /*!
        \property GlobalVariables::m_version
        \brief Returns the current app version. Not yet used.
    */
    QVersionNumber version() const
    {
        return m_version;
    }

    static QStringList getAvailableWallpaper()
    {
        return {
            "qmlWallpaper",
            "htmlWallpaper",
            "videoWallpaper",
            "godotWallpaper",
        };
    }
    static QStringList getAvailableWidgets()
    {
        return {
            "qmlWidget",
            "htmlWidget",
        };
    }

    static QStringList getAvailableTypes()
    {
        return { getAvailableWallpaper() + getAvailableWidgets() };
    }

signals:
    void localStoragePathChanged(QUrl localStoragePath);
    void localSettingsPathChanged(QUrl localSettingsPath);
    void wallpaperExecutablePathChanged(QUrl wallpaperExecutablePath);
    void widgetExecutablePathChanged(QUrl widgetExecutablePath);

public slots:

    void setLocalStoragePath(QUrl localStoragePath)
    {
        if (m_localStoragePath == localStoragePath)
            return;

        m_localStoragePath = localStoragePath;
        emit localStoragePathChanged(m_localStoragePath);
    }

    void setLocalSettingsPath(QUrl localSettingsPath)
    {
        if (m_localSettingsPath == localSettingsPath)
            return;

        m_localSettingsPath = localSettingsPath;
        emit localSettingsPathChanged(m_localSettingsPath);
    }

    void setWallpaperExecutablePath(QUrl wallpaperExecutablePath)
    {
        if (m_wallpaperExecutablePath == wallpaperExecutablePath)
            return;

        m_wallpaperExecutablePath = wallpaperExecutablePath;
        emit wallpaperExecutablePathChanged(m_wallpaperExecutablePath);
    }

    void setWidgetExecutablePath(QUrl widgetExecutablePath)
    {
        if (m_widgetExecutablePath == widgetExecutablePath)
            return;

        m_widgetExecutablePath = widgetExecutablePath;
        emit widgetExecutablePathChanged(m_widgetExecutablePath);
    }

private:
    QUrl m_localStoragePath;
    QUrl m_localSettingsPath;
    QUrl m_wallpaperExecutablePath;
    QUrl m_widgetExecutablePath;
    QVersionNumber m_version { 1, 0, 0 };
};
}
