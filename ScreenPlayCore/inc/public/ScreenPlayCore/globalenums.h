// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#pragma once
#include <QObject>
#include <QQmlEngine>
#include <QtCore/qmetatype.h>

namespace ScreenPlay {

class ScreenPlayEnums : public QObject {
    Q_OBJECT
    QML_ELEMENT
    Q_CLASSINFO("RegisterEnumClassesUnscoped", "false")

public:
    /*!
        \brief Application state enum representing the complete lifecycle of a wallpaper or widget process.
    */
    enum class AppState {
        // Initial States
        NotSet, // Default uninitialized state, should transition to Inactive

        // Starting States
        Starting, // Process launching, attempting initial connection
        StartingFailed, // Failed to start process or connect within timeout period

        // Active States
        Active, // Process running and connected successfully

        // Error States (while running)
        Timeout, // Lost connection, process may still be running
        Crashed, // Process terminated unexpectedly (non-zero exit code)
        ErrorOccurred, // General error occurred while running

        // Transition States
        PostActiveHandling, // Temporary state while handling post-Active recovery

        // Closing States
        Closing, // Shutdown initiated, waiting for graceful exit
        ClosingFailed, // Failed to close gracefully within timeout
        ClosedGracefully, /// Exit code 0
    };
    Q_ENUM(AppState)
    /*!
        \brief
    */
    enum class Version {
        OpenSourceStandalone,
        OpenSourceSteam,
        OpenSourceProStandalone,
        OpenSourceProSteam,
        OpenSourceUltraStandalone,
        OpenSourceUltraSteam
    };
    Q_ENUM(Version)

    /*!
        \brief Graphics API enum for wallpaper rendering.
    */
    enum class GraphicsApi {
        Auto, // Let Qt decide (default)
        OpenGL, // Force OpenGL
        DirectX11 // Force DirectX11 (Windows only)
    };
    Q_ENUM(GraphicsApi)
};

}
