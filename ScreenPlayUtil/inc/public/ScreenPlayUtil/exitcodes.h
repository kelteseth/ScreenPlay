#pragma once

namespace ScreenPlay {

Q_NAMESPACE

enum class WallpaperExitCode {
    Ok = 0,
    Invalid_ArgumentSize,
    Invalid_ActiveScreensList,
    Invalid_InstalledType,
    Invalid_CheckWallpaperVisible,
    Invalid_Volume,
    Invalid_AppID,
    Invalid_Setup_ProjectParsingError,
    Invalid_Setup_Error,
    Invalid_Start_Windows_HandleError,
};
Q_ENUM_NS(WallpaperExitCode)
}