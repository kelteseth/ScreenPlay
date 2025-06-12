// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#include "ScreenPlayCore/steamenumsgenerated.h"
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);

    // Must be set so we can access the global ScreenPlay settings like install path.
    QGuiApplication::setOrganizationName("ScreenPlay");
    QGuiApplication::setOrganizationDomain("screen-play.app");
    QGuiApplication::setApplicationName("ScreenPlay");

    QQuickStyle::setStyle("Material");
    QQmlApplicationEngine qmlApplicationEngine;
    qmlApplicationEngine.loadFromModule("ScreenPlayWorkshop", "TestMain");
    return app.exec();
}
