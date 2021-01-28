/****************************************************************************
**
** Copyright (C) 2020 Elias Steurer (Kelteseth)
** Contact: https://screen-play.app
**
** This file is part of ScreenPlay. ScreenPlay is licensed under a dual license in
** order to ensure its sustainability. When you contribute to ScreenPlay
** you accept that your work will be available under the two following licenses:
**
** $SCREENPLAY_BEGIN_LICENSE$
**
** #### Affero General Public License Usage (AGPLv3)
** Alternatively, this file may be used under the terms of the GNU Affero
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file "ScreenPlay License.md" included in the
** packaging of this App. Please review the following information to
** ensure the GNU Affero Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/agpl-3.0.en.html.
**
** #### Commercial License
** This code is owned by Elias Steurer. By changing/adding to the code you agree to the
** terms written in:
**  * Legal/corporate_contributor_license_agreement.md - For corporate contributors
**  * Legal/individual_contributor_license_agreement.md - For individual contributors
**
** #### Additional Limitations to the AGPLv3 and Commercial Lincese
** This License does not grant any rights in the trademarks,
** service marks, or logos.
**
**
** $SCREENPLAY_END_LICENSE$
**
****************************************************************************/

#pragma once
#include <qobjectdefs.h>

namespace ScreenPlay {
/*!
    \class ScreenPlay::GlobalVariables
    \inmodule ScreenPlay
    \brief GlobalVariables.

    A header only class used only for storing some global variables like localStoragePath.

*/

namespace SearchType {
    Q_NAMESPACE

    enum class SearchType {
        All,
        Text,
        Scene, //QML, HTML, Godot, Gif, Website wallpaper
        Wallpaper,
        Widget,
    };
    Q_ENUM_NS(SearchType)

}

namespace FillMode {
    Q_NAMESPACE

    enum class FillMode {
        Stretch,
        Fill,
        Contain,
        Cover,
        Scale_Down
    };
    Q_ENUM_NS(FillMode)

}

namespace InstalledType {
    Q_NAMESPACE

    // When changing the enum, one also needs to change:
    // GlobalVariables::getAvailableWallpaper
    // GlobalVariables::getAvailableWidgets
    // Common/Util.js isWallpaper() and isWidget()
    // ScreenPlayWallpaper: BaseWindow::parseWallpaperType()
    enum class InstalledType {
        Unknown,
        //Wallpaper
        VideoWallpaper,
        QMLWallpaper,
        HTMLWallpaper,
        GodotWallpaper,
        GifWallpaper,
        WebsiteWallpaper,
        //Widgets
        QMLWidget,
        HTMLWidget,
    };
    Q_ENUM_NS(InstalledType)

}
}
