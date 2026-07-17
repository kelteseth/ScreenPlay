// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#pragma once

#include "ScreenPlayCore/globalenums.h"

namespace ScreenPlay {

// Returns true when a Vulkan instance can be created on this machine.
bool isVulkanAvailable();

// Applies the user selected graphics API to Qt Quick. Must be called after
// QGuiApplication exists but before the first QQuickWindow is created.
// Auto prefers Vulkan on Windows and falls back to the Qt default (D3D11)
// when no usable Vulkan driver is present. A QSG_RHI_BACKEND environment
// override always wins and disables this selection.
void applyGraphicsApi(ScreenPlayEnums::GraphicsApi api);
}
