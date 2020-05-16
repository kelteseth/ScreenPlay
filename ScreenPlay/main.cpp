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

#include <QApplication>

#include "app.h"

int main(int argc, char* argv[])
{
    // Buggy with every Qt version except 5.14.0!
    // Displays wrong DPI scaled monitor resolution
    // 4k with 150% scaling to FULL HD on Windows
    // https://bugreports.qt.io/browse/QTBUG-81694
    #if defined(Q_OS_LINUX) || defined(Q_OS_OSX)
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    #endif
    QApplication::setAttribute(Qt::AA_ShareOpenGLContexts);

    QApplication qtGuiApp(argc, argv);

    ScreenPlay::App app;

    if (app.m_isAnotherScreenPlayInstanceRunning) {
        return 0;
    } else {
        app.init();
        return qtGuiApp.exec();
    }
}
