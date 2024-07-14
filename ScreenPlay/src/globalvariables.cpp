// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#include "ScreenPlay/globalvariables.h"
#include "ScreenPlay/CMakeVariables.h"
#include <QMetaType>
#include <QStandardPaths>

namespace ScreenPlay {

/*!
    \class ScreenPlay::GlobalVariables
    \inmodule ScreenPlay
    \brief  Contains all variables that are globally needed.
*/

/*!
    \brief Constructs the global variabls.
*/
GlobalVariables::GlobalVariables(QObject* parent)
    : QObject(parent)
{
    if (SCREENPLAY_STEAM_VERSION) {
        setVersion(GlobalVariables::Version::OpenSourceSteam);
    } else {
        setVersion(GlobalVariables::Version::OpenSourceStandalone);
    }
    setVersion(GlobalVariables::Version::OpenSourceStandalone);
    setLocalSettingsPath(QUrl { QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) });
}

bool GlobalVariables::isBasicVersion() const
{
    return (m_version == Version::OpenSourceSteam
        || m_version == Version::OpenSourceStandalone);
}

bool GlobalVariables::isSteamVersion() const
{
    return (m_version == Version::OpenSourceSteam
        || m_version == Version::OpenSourceProSteam
        || m_version == Version::OpenSourceUltraSteam);
}

bool GlobalVariables::isStandaloneVersion() const
{
    return (m_version == Version::OpenSourceStandalone
        || m_version == Version::OpenSourceProStandalone
        || m_version == Version::OpenSourceUltraStandalone);
}

bool GlobalVariables::isProVersion() const
{
    return (m_version == Version::OpenSourceProStandalone
        || m_version == Version::OpenSourceProSteam);
}

bool GlobalVariables::isUltraVersion() const
{
    return (m_version == Version::OpenSourceUltraStandalone
        || m_version == Version::OpenSourceUltraSteam);
}

void GlobalVariables::setLocalStoragePath(QUrl localStoragePath)
{
    if (m_localStoragePath == localStoragePath)
        return;

    m_localStoragePath = localStoragePath;
    emit localStoragePathChanged(m_localStoragePath);
}

void GlobalVariables::setLocalSettingsPath(QUrl localSettingsPath)
{
    if (m_localSettingsPath == localSettingsPath)
        return;

    m_localSettingsPath = localSettingsPath;
    emit localSettingsPathChanged(m_localSettingsPath);
}

void GlobalVariables::setWallpaperExecutablePath(QUrl wallpaperExecutablePath)
{
    if (m_wallpaperExecutablePath == wallpaperExecutablePath)
        return;

    m_wallpaperExecutablePath = wallpaperExecutablePath;
    emit wallpaperExecutablePathChanged(m_wallpaperExecutablePath);
}

void GlobalVariables::setWidgetExecutablePath(QUrl widgetExecutablePath)
{
    if (m_widgetExecutablePath == widgetExecutablePath)
        return;

    m_widgetExecutablePath = widgetExecutablePath;
    emit widgetExecutablePathChanged(m_widgetExecutablePath);
}

void GlobalVariables::setGodotWallpaperExecutablePath(QUrl godotWallpaperExecutablePath)
{
    if (m_godotWallpaperExecutablePath == godotWallpaperExecutablePath)
        return;
    m_godotWallpaperExecutablePath = godotWallpaperExecutablePath;
    emit godotWallpaperExecutablePathChanged(m_godotWallpaperExecutablePath);
}

void GlobalVariables::setGodotEditorExecutablePath(QUrl godotEditorExecutablePath)
{
    if (m_godotEditorExecutablePath == godotEditorExecutablePath)
        return;
    m_godotEditorExecutablePath = godotEditorExecutablePath;
    emit godotEditorExecutablePathChanged(m_godotEditorExecutablePath);
}

void GlobalVariables::setVersion(Version version)
{
    if (m_version == version)
        return;
    m_version = version;
    emit versionChanged(m_version);
}
}

#include "moc_globalvariables.cpp"
