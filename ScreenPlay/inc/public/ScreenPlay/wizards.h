// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#pragma once

#include <QColor>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFont>
#include <QFuture>
#include <QFutureWatcher>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QLinearGradient>
#include <QObject>
#include <QPainter>
#include <QProcess>
#include <QQmlEngine>
#include <QScopedPointer>
#include <QString>
#include <QStringList>
#include <QTextOption>
#include <QTime>
#include <QTimer>
#include <QUrl>
#include <QtMath>

#include "ScreenPlay/CMakeVariables.h"
#include "ScreenPlay/globalvariables.h"
#include "ScreenPlayUtil/util.h"
#include "qcorotask.h"
#include "qml/qcoroqml.h"
#include "qml/qcoroqmltask.h"

#include <memory>
#include <optional>

namespace ScreenPlay {

class Wizards : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("CPP ONLY")
    Q_CLASSINFO("RegisterEnumClassesUnscoped", "false")

public:
    explicit Wizards(
        const std::shared_ptr<GlobalVariables>& globalVariables,
        QObject* parent = nullptr);

    enum class WizardResult {
        Ok,
        CopyError,
        WriteFileError,
        WriteProjectFileError,
        WriteLicenseFileError,
        CreateProjectFolderError,
        CopyPreviewThumbnailError,
        CopyFileError,
    };
    Q_ENUM(WizardResult)

    Q_INVOKABLE QCoro::QmlTask createQMLWidget(
        const QString& title,
        const QString& licenseName,
        const QString& licenseFile,
        const QString& createdBy,
        const QString& previewThumbnail,
        const QVector<QString>& tags);

    Q_INVOKABLE QCoro::QmlTask createHTMLWidget(
        const QString& title,
        const QString& licenseName,
        const QString& licenseFile,
        const QString& createdBy,
        const QString& previewThumbnail,
        const QVector<QString>& tags);

    Q_INVOKABLE QCoro::QmlTask createHTMLWallpaper(
        const QString& title,
        const QString& licenseName,
        const QString& licenseFile,
        const QString& createdBy,
        const QString& previewThumbnail,
        const QVector<QString>& tags);

    Q_INVOKABLE QCoro::QmlTask createQMLWallpaper(
        const QString& title,
        const QString& licenseName,
        const QString& licenseFile,
        const QString& createdBy,
        const QString& previewThumbnail,
        const QVector<QString>& tags);

    Q_INVOKABLE QCoro::QmlTask createGodotWallpaper(
        const QString& title,
        const QString& licenseName,
        const QString& licenseFile,
        const QString& createdBy,
        const QString& previewThumbnail,
        const QVector<QString>& tags);

    Q_INVOKABLE QCoro::QmlTask createGifWallpaper(
        const QString& title,
        const QString& licenseName,
        const QString& licenseFile,
        const QString& creator,
        const QString& file,
        const QVector<QString>& tags);

    Q_INVOKABLE QCoro::QmlTask createWebsiteWallpaper(
        const QString& title,
        const QString& previewThumbnail,
        const QUrl& url,
        const QVector<QString>& tags);

private:
    void createPreviewImage(const QString& name, const QString& targetPath);

private:
    const std::shared_ptr<GlobalVariables> m_globalVariables;
    const std::optional<QString> createTemporaryFolder() const;
    QFuture<void> m_wizardFuture;
    Util m_util;
    const QVector<QColor> m_gradientColors = {
        QColor(183, 28, 28), // #B71C1C
        QColor(27, 94, 32), // #1B5E20
        QColor(13, 71, 161), // #0D47A1
        QColor(255, 214, 0), // #FFD600
        QColor(74, 20, 140) // #4A148C
    };
};
}
