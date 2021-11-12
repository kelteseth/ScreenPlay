import QtQuick
import QtQuick.Controls as QQC
import QtQuick.Window
import Qt5Compat.GraphicalEffects
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.wallpapers.image 2.0 as Wallpaper
import org.kde.kcm 1.1 as KCM
import org.kde.kirigami 2.4 as Kirigami
import org.kde.newstuff 1.1 as NewStuff

Column {
    id: root

    property alias cfg_StopWallpaperIfHidden: stopWallpaperIfHidden.checked

    anchors.fill: parent
    spacing: units.largeSpacing

    Row {
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: units.largeSpacing

        QQC.CheckBox {
            id: stopWallpaperIfHidden

            text: i18nd("plasma_applet_org.kde.image", "Stop animation when a window is maximized")
        }

    }

}
