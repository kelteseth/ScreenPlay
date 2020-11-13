import QtQuick 2.11
import QtQuick.Controls 2.4 as QQC
import QtQuick.Window 2.0 
import QtGraphicalEffects 1.0

import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.wallpapers.image 2.0 as Wallpaper
import org.kde.kcm 1.1 as KCM
import org.kde.kirigami 2.4 as Kirigami
import org.kde.newstuff 1.1 as NewStuff



 Column {
	 id: root
	 anchors.fill:parent

	 property alias  cfg_StopWallpaperIfHidden: stopWallpaperIfHidden.checked
	 spacing: units.largeSpacing

	 Row {
		 anchors.horizontalCenter: parent.horizontalCenter
		 spacing: units.largeSpacing
		       QQC.CheckBox {
				 id: stopWallpaperIfHidden
				 text:  i18nd("plasma_applet_org.kde.image","Stop animation when a window is maximized");
			 }
	 }


 }

