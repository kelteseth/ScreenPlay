// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
import QtQuick
import ScreenPlayWallpaper

AnimatedImage {
    Component.onCompleted: Wallpaper.requestFadeIn()
}
