// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#pragma once

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFuture>
#include <QFutureWatcher>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QObject>
#include <QProcess>
#include <QQmlEngine>
#include <QScopedPointer>
#include <QString>
#include <QStringList>
#include <QTime>
#include <QTimer>
#include <QUrl>
#include <QtMath>

#include "ScreenPlay/createimportvideo.h"
#include "ScreenPlay/globalvariables.h"
#include "ScreenPlay/util.h"

#include <memory>
#include <optional>

namespace ScreenPlay {

class Wizards : public QObject {
    Q_OBJECT
    QML_ELEMENT
public:
    explicit Wizards(const std::shared_ptr<GlobalVariables>& globalVariables, QObject* parent = nullptr);
    Wizards() { }

    enum class WizardResult {
        Ok,
        CopyError,
        WriteProjectFileError,
        WriteLicenseFileError,
        CreateProjectFolderError,
        CopyPreviewThumbnailError,
        CopyFileError,
    };
    Q_ENUM(WizardResult)

public slots:
    void createQMLWidget(
        const QString& title,
        const QString& licenseName,
        const QString& licenseFile,
        const QString& createdBy,
        const QString& previewThumbnail,
        const QVector<QString>& tags);

    void createHTMLWidget(
        const QString& title,
        const QString& licenseName,
        const QString& licenseFile,
        const QString& createdBy,
        const QString& previewThumbnail,
        const QVector<QString>& tags);

    void createHTMLWallpaper(
        const QString& title,
        const QString& licenseName,
        const QString& licenseFile,
        const QString& createdBy,
        const QString& previewThumbnail,
        const QVector<QString>& tags);

    void createQMLWallpaper(
        const QString& title,
        const QString& licenseName,
        const QString& licenseFile,
        const QString& createdBy,
        const QString& previewThumbnail,
        const QVector<QString>& tags);


    void createGodotWallpaper(
        const QString& title,
        const QString& licenseName,
        const QString& licenseFile,
        const QString& createdBy,
        const QString& previewThumbnail,
        const QVector<QString>& tags);

    void createGifWallpaper(
        const QString& title,
        const QString& licenseName,
        const QString& licenseFile,
        const QString& creator,
        const QString& file,
        const QVector<QString>& tags);

    void createWebsiteWallpaper(
        const QString& title,
        const QString& previewThumbnail,
        const QUrl& url,
        const QVector<QString>& tags);

signals:
    void widgetCreationFinished(const Wizards::WizardResult result);
    void widgetCreationFinished(const Wizards::WizardResult result, const QVariant value);

private:
    const std::shared_ptr<GlobalVariables> m_globalVariables;
    const std::optional<QString> createTemporaryFolder() const;

private:
    QFuture<void> m_wizardFuture;
};
}
