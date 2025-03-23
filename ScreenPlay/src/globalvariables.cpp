// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#include "ScreenPlay/globalvariables.h"
#include "CMakeVariables.h"
#include "ScreenPlay/steamdlcchecker.h"
#include <QGuiApplication>
#include <QMetaType>
#include <QStandardPaths>

namespace ScreenPlay {

/*!
    \class ScreenPlay::GlobalVariables
    \inmodule ScreenPlay
    \brief  Contains all variables that are globally needed.
*/

/*!
    \brief Constructs the global variables.
*/
GlobalVariables::GlobalVariables(QObject* parent)
    : QObject(parent)
{
    setLocalSettingsPath(QUrl { QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) });
    const bool startedViaSteam = QGuiApplication::instance()->arguments().contains("--steam");

    // Check if we're running the Steam version
    if (SCREENPLAY_STEAM_VERSION) {
        // Always check dlc status is started with the --steam flag, to check
        // for dlc status changes
        if (startedViaSteam) {
            SteamDLCChecker dlcChecker;
            auto versionResult = dlcChecker.checkDLCStatus();
            if (versionResult) {
                // Successfully determined version based on DLC ownership
                setVersion(versionResult.value());
                m_licenseManager.setVersion(versionResult.value());
                qInfo() << "Version set from Steam DLC status:" << QVariant::fromValue(m_version).toString();
            } else {
                // Failed to check DLC status, fall back to local license
                qWarning() << "Steam DLC check failed with error:" << static_cast<int>(versionResult.error());

                if (m_licenseManager.checkLocalLicense()) {
                    // Use version from local license as fallback
                    setVersion(m_licenseManager.getVersion());
                    qInfo() << "Fallback to local license, version:" << QVariant::fromValue(m_version).toString();
                } else {
                    // No valid local license, set default Steam version
                    setVersion(ScreenPlayEnums::Version::OpenSourceSteam);
                    qInfo() << "Using default OpenSourceSteam version";
                }
            }
        } else {
            // Not launched through Steam, ALWAYS check license on disk first
            if (m_licenseManager.checkLocalLicense()) {
                // License is valid, use the version from the license manager
                setVersion(m_licenseManager.getVersion());
                qInfo() << "Version set from local license:" << QVariant::fromValue(m_version).toString();
            } else {
                // No valid local license found, use default Steam version
                setVersion(ScreenPlayEnums::Version::OpenSourceSteam);
                qInfo() << "No valid license found, using default OpenSourceSteam version";
            }
        }
    } else {
        setVersion(ScreenPlayEnums::Version::OpenSourceStandalone);
    }

    // Log the determined version
    qInfo() << "ScreenPlay version set to:" << QVariant::fromValue(m_version).toString();
}

bool GlobalVariables::isBasicVersion() const
{
    return (m_version == ScreenPlayEnums::Version::OpenSourceSteam
        || m_version == ScreenPlayEnums::Version::OpenSourceStandalone);
}

bool GlobalVariables::isSteamVersion() const
{
    return (m_version == ScreenPlayEnums::Version::OpenSourceSteam
        || m_version == ScreenPlayEnums::Version::OpenSourceProSteam
        || m_version == ScreenPlayEnums::Version::OpenSourceUltraSteam);
}

bool GlobalVariables::isStandaloneVersion() const
{
    return (m_version == ScreenPlayEnums::Version::OpenSourceStandalone
        || m_version == ScreenPlayEnums::Version::OpenSourceProStandalone
        || m_version == ScreenPlayEnums::Version::OpenSourceUltraStandalone);
}

bool GlobalVariables::isProVersion() const
{
    return (m_version == ScreenPlayEnums::Version::OpenSourceProStandalone
        || m_version == ScreenPlayEnums::Version::OpenSourceProSteam);
}

bool GlobalVariables::isUltraVersion() const
{
    return (m_version == ScreenPlayEnums::Version::OpenSourceUltraStandalone
        || m_version == ScreenPlayEnums::Version::OpenSourceUltraSteam);
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

void GlobalVariables::setVersion(ScreenPlayEnums::Version version)
{
    if (m_version == version)
        return;
    m_version = version;
    emit versionChanged(m_version);
}
}

#include "moc_globalvariables.cpp"
