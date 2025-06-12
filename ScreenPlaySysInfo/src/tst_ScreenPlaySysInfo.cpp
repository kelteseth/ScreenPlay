// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#include <QGuiApplication>
#include <QQmlApplicationEngine>

int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine qmlApplicationEngine;
    qmlApplicationEngine.loadFromModule("ScreenPlaySysInfo", "TestMain");
    return app.exec();
}
