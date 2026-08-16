// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#include "ScreenPlayCore/graphicsapi.h"

#include <QDebug>
#include <QQuickWindow>
#include <QtGlobal>
#include <QtGui/qtguiglobal.h>

// Qt only defines QVulkanInstance when the platform has vulkan enabled *and*
// the Vulkan SDK headers are actually installed (qvulkaninstance.h gates the
// class on both QT_CONFIG(vulkan) and __has_include(<vulkan/vulkan.h>)). Mirror
// that here so the build works on machines without the Vulkan headers.
#if defined(QT_FEATURE_vulkan) && QT_FEATURE_vulkan == 1 && __has_include(<vulkan/vulkan.h>)
#include <QVulkanInstance>
#define SP_HAS_VULKAN 1
#endif

namespace ScreenPlay {

bool isVulkanAvailable()
{
#ifdef SP_HAS_VULKAN
    QVulkanInstance probe;
    const bool available = probe.create();
    probe.destroy();
    return available;
#else
    return false;
#endif
}

void applyGraphicsApi(ScreenPlayEnums::GraphicsApi api)
{
    if (qEnvironmentVariableIsSet("QSG_RHI_BACKEND")) {
        qInfo() << "QSG_RHI_BACKEND override active:" << qgetenv("QSG_RHI_BACKEND")
                << "- skipping graphics API selection";
        return;
    }

    using GraphicsApi = ScreenPlayEnums::GraphicsApi;
    switch (api) {
    case GraphicsApi::Vulkan:
        if (isVulkanAvailable()) {
            QQuickWindow::setGraphicsApi(QSGRendererInterface::Vulkan);
            qInfo() << "Graphics API set to Vulkan";
        } else {
            qWarning() << "Vulkan requested but no usable Vulkan driver found, using Qt default";
        }
        break;
    case GraphicsApi::DirectX11:
#ifdef Q_OS_WIN
        QQuickWindow::setGraphicsApi(QSGRendererInterface::Direct3D11);
        qInfo() << "Graphics API set to Direct3D11";
#else
        qWarning() << "DirectX11 is only available on Windows, using Qt default";
#endif
        break;
    case GraphicsApi::DirectX12:
#ifdef Q_OS_WIN
        // Qt WebEngine cannot composite into a D3D12 scene graph, so HTML
        // wallpapers drop to a software path on this backend.
        QQuickWindow::setGraphicsApi(QSGRendererInterface::Direct3D12);
        qInfo() << "Graphics API set to Direct3D12";
#else
        qWarning() << "DirectX12 is only available on Windows, using Qt default";
#endif
        break;
    case GraphicsApi::OpenGL:
        QQuickWindow::setGraphicsApi(QSGRendererInterface::OpenGL);
        qInfo() << "Graphics API set to OpenGL";
        break;
    case GraphicsApi::Auto:
#ifdef Q_OS_WIN
        // Vulkan keeps DWM frame pacing intact while a wallpaper is parented
        // under WorkerW; the D3D11 default loses independent flip there and
        // the main window stutters at mixed refresh cadences.
        if (isVulkanAvailable()) {
            QQuickWindow::setGraphicsApi(QSGRendererInterface::Vulkan);
            qInfo() << "Graphics API set to Vulkan (Auto)";
        } else {
            qInfo() << "Graphics API Auto: no Vulkan driver, using Qt default (Direct3D11)";
        }
#else
        qInfo() << "Graphics API set to Auto (Qt default)";
#endif
        break;
    }
}
}
