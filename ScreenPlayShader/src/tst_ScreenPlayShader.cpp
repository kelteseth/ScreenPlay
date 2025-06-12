// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickView>

int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine qmlApplicationEngine;
    qmlApplicationEngine.loadFromModule("ScreenPlayShader", "TestMain");
    return app.exec();
}
