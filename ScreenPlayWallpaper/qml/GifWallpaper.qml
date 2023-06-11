import QtQuick
import ScreenPlayWallpaper

AnimatedImage {
    Component.onCompleted: Wallpaper.requestFadeIn()
}
