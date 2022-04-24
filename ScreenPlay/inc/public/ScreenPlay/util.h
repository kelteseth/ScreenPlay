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

#include <QApplication>
#include <QClipboard>
#include <QCoreApplication>
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>
#include <QProcess>
#include <QScopeGuard>
#include <QString>
#include <QTextStream>
#include <QVersionNumber>
#include <QtConcurrent/QtConcurrent>
#include <qqml.h>

#include "ScreenPlayUtil/util.h"
#include "ScreenPlay/globalvariables.h"

#include <fstream>
#include <iostream>
#include <optional>

namespace ScreenPlay {

template <typename T>
T QStringToEnum(const QString& key, const T defaultValue)
{
    auto metaEnum = QMetaEnum::fromType<T>();

    bool ok = false;
    T wantedEnum = static_cast<T>(metaEnum.keyToValue(key.toUtf8(), &ok));

    if (ok) {
        return wantedEnum;
    } else {
        qWarning() << "Unable to convert QStringToEnum. Key: " << key;
    }

    return defaultValue;
}

class Util : public QObject {
    Q_OBJECT

    Q_PROPERTY(QString debugMessages READ debugMessages NOTIFY debugMessagesChanged)

public:
    explicit Util(QNetworkAccessManager* networkAccessManager, QObject* parent = nullptr);

    QString debugMessages() const
    {
        return m_debugMessages;
    }

signals:
    void requestNavigation(QString nav);
    void requestNavigationActive(bool isActive);
    void requestToggleWallpaperConfiguration();
    void setSidebarItem(QString folderName, ScreenPlay::InstalledType::InstalledType type);
    void allLicenseLoaded(QString licensesText);
    void allDataProtectionLoaded(QString dataProtectionText);
    void debugMessagesChanged(QString debugMessages);

public slots:
    void copyToClipboard(const QString& text) const;
    void openFolderInExplorer(const QString& url) const;
    QString toLocal(const QString& url);

    void requestAllLicenses();
    void requestDataProtection();

    static void logToGui(QtMsgType type, const QMessageLogContext& context, const QString& msg);
    static bool writeJsonObjectToFile(const QString& absoluteFilePath, const QJsonObject& object, bool truncate = true);
    static bool writeSettings(const QJsonObject& obj, const QString& absolutePath);
    static bool writeFile(const QString& text, const QString& absolutePath);
    static bool writeFileFromQrc(const QString& qrcPath, const QString& absolutePath);
    static bool copyPreviewThumbnail(QJsonObject& obj, const QString& previewThumbnail, const QString& destination);

    void setNavigation(QString nav)
    {
        emit requestNavigation(nav);
    }

    // When we create a wallpaper the main navigation gets disabled
    void setNavigationActive(bool isActive)
    {
        emit requestNavigationActive(isActive);
    }

    void setToggleWallpaperConfiguration()
    {
        emit requestToggleWallpaperConfiguration();
    }

    void appendDebugMessages(QString debugMessages)
    {
        if (m_debugMessages.size() > 10000) {
            m_debugMessages = "###### DEBUG CLEARED ######";
        }

        m_debugMessages += debugMessages;
        emit debugMessagesChanged(m_debugMessages);
    }

private:
    QNetworkAccessManager* m_networkAccessManager { nullptr };

    QString m_debugMessages {};
    QFuture<void> m_requestAllLicensesFuture;
};

// Used for redirect content from static logToGui to setDebugMessages
static Util* utilPointer { nullptr };
}
