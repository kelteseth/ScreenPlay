// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#include "ScreenPlay/globalvariables.h"

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
    setLocalSettingsPath(QUrl { QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) });
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
