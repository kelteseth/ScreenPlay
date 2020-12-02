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

#include <memory>
#include <optional>

#include "createimportvideo.h"
#include "globalvariables.h"
#include "util.h"

#include <QObject>

namespace ScreenPlay {

class Wizards : public QObject {
    Q_OBJECT
public:
    explicit Wizards(const std::shared_ptr<GlobalVariables>& globalVariables, QObject* parent = nullptr);
    Wizards() { }

    enum class WizardResult {
        Ok,
        CopyError,
        WriteProjectFileError,
        CreateProjectFolderError,
        CopyPreviewThumbnailError,
    };
    Q_ENUM(WizardResult)

public slots:
    void createQMLWidget(
        const QString& localStoragePath,
        const QString& title,
        const QString& previewThumbnail,
        const QString& createdBy,
        const QString& license,
        const QVector<QString>& tags);

    void createHTMLWidget(
        const QString& localStoragePath,
        const QString& title,
        const QString& previewThumbnail,
        const QString& createdBy,
        const QString& license,
        const QVector<QString>& tags);

    void createHTMLWallpaper(
        const QString& localStoragePath,
        const QString& title,
        const QString& previewThumbnail,
        const QString& license,
        const QVector<QString>& tags);

    void createQMLWallpaper(
        const QString& title,
        const QString& previewThumbnail,
        const QString& license,
        const QVector<QString>& tags);

signals:
    void widgetCreationFinished(const Wizards::WizardResult result);
    void widgetCreationFinished(const Wizards::WizardResult result, const QVariant value);

private:
    const std::shared_ptr<GlobalVariables> m_globalVariables;
    const std::optional<QString> createTemporaryFolder() const;
};
}
