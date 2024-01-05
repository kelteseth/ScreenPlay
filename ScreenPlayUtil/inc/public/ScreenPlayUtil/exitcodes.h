#pragma once

#include <QObject>
#include <QQmlEngine>

namespace ScreenPlay {

class WallpaperExit : public QObject {
    Q_OBJECT
    QML_ELEMENT
    Q_CLASSINFO("RegisterEnumClassesUnscoped", "false")
public:
    WallpaperExit(QObject* parent = nullptr);
    enum class Code {
        Ok = 0,
        Invalid_ArgumentSize,
        Invalid_ActiveScreensList,
        Invalid_InstalledType,
        Invalid_CheckWallpaperVisible,
        Invalid_Volume,
        Invalid_AppID,
        Invalid_Setup_ProjectParsingError,
        Invalid_Setup_Error,
        Invalid_PID,
        Invalid_Start_Windows_HandleError,
    };
    Q_ENUM(Code)
};

class WidgetExit : public QObject {
    Q_OBJECT
    QML_ELEMENT
    Q_CLASSINFO("RegisterEnumClassesUnscoped", "false")
public:
    WidgetExit(QObject* parent = nullptr);
    enum class Code {
        Ok = 0,
        Invalid_ArgumentSize,
        Invalid_Path,
        Invalid_InstalledType,
        Invalid_POSX,
        Invalid_POSY,
        Invalid_DEBUG,
        Invalid_AppID,
        Invalid_Setup_ProjectParsingError,
        Invalid_Setup_Error,
        Invalid_PID,
    };
    Q_ENUM(Code)
};
}
