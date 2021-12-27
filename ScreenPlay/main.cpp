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

#include "app.h"
#include <QApplication>
#include <QCommandLineParser>
#include <QDebug>

#include <sentry.h>
#define DOCTEST_CONFIG_IMPLEMENT
#define DOCTEST_CONFIG_SUPER_FAST_ASSERTS
#include <doctest/doctest.h>

int main(int argc, char* argv[])
{
    Q_INIT_RESOURCE(Resources);

    QApplication qtGuiApp(argc, argv);

    // Unit tests
    doctest::Context context;
    context.setOption("abort-after", 5); // stop test execution after 5 failed assertions
    context.setOption("order-by", "name"); // sort the test cases by their name
    context.setOption("no-breaks", true); // don't break in the debugger when assertions fail
    context.setOption("no-run", true); // No tests are executed by default
    context.applyCommandLine(argc, argv); // Every setOption call after applyCommandLine overrides the command line arguments
    const int testResult = context.run();
    if (context.shouldExit())
        return testResult;

    ScreenPlay::App app;

    if (app.m_isAnotherScreenPlayInstanceRunning) {
        return 0;
    } else {
        app.init();
        const int status = qtGuiApp.exec();
        sentry_shutdown();
        return status;
    }
}
