// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#pragma once

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QObject>
#include <QProcess>
#include <QQmlEngine>
#include <QString>
#include <optional>

#include "ScreenPlayCore/contenttypes.h"
#include "ScreenPlayCore/util.h"
#include "qcorotask.h"
#include "qml/qcoroqml.h"
#include "qml/qcoroqmltask.h"

namespace ScreenPlay {

class Result;
class Util;

class GodotHandler : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")
    Q_CLASSINFO("RegisterEnumClassesUnscoped", "false")

public:
    explicit GodotHandler(QObject* parent = nullptr);

    Q_INVOKABLE bool openGodotEditor(const QString& contentPath, const QString& godotEditorExecutablePath) const;
    Q_INVOKABLE QCoro::QmlTask exportGodotProject(const QString& absolutePath, const QString& godotEditorExecutablePath, const bool overwrite = false);
    Q_INVOKABLE bool godotProjectExportExists(const QString& absolutePath) const;
    Q_INVOKABLE bool isGodotWallpaper(const ScreenPlay::ContentTypes::InstalledType type) const;

private:
    std::optional<QFileInfo> getGodotProjectExportFile(const QString& absolutePath) const;
    std::optional<QJsonObject> readProjectJsonFromZip(const QString& zipFilePath) const;
    bool checkGodotVersionInZip(const QString& zipFilePath) const;

    Util m_util;
};

}
