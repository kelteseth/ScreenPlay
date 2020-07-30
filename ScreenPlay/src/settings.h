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
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFontDatabase>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QObject>
#include <QPair>
#include <QProcess>
#include <QProcessEnvironment>
#include <QQmlPropertyMap>
#include <QSettings>
#include <QStandardPaths>
#include <QString>
#include <QTextStream>
#include <QThread>
#include <QUrl>
#include <QVariant>
#include <QVector>
#include <QVersionNumber>
#include <QtConcurrent/QtConcurrent>
#include <QtGlobal>

#include <memory>
#include <optional>

#include "globalvariables.h"
#include "nlohmann/json.hpp"
#include "util.h"

#ifdef Q_OS_WIN
#include <qt_windows.h>
#endif

namespace ScreenPlay {
class ActiveProfile;

class Settings : public QObject {
    Q_OBJECT

    Q_PROPERTY(bool anonymousTelemetry READ anonymousTelemetry WRITE setAnonymousTelemetry NOTIFY anonymousTelemetryChanged)
    Q_PROPERTY(bool silentStart READ silentStart WRITE setSilentStart NOTIFY silentStartChanged)
    Q_PROPERTY(bool autostart READ autostart WRITE setAutostart NOTIFY autostartChanged)
    Q_PROPERTY(bool highPriorityStart READ highPriorityStart WRITE setHighPriorityStart NOTIFY highPriorityStartChanged)
    Q_PROPERTY(bool checkWallpaperVisible READ checkWallpaperVisible WRITE setCheckWallpaperVisible NOTIFY checkWallpaperVisibleChanged)
    Q_PROPERTY(bool offlineMode READ offlineMode WRITE setOfflineMode NOTIFY offlineModeChanged)

    Q_PROPERTY(ScreenPlay::FillMode::FillMode videoFillMode READ videoFillMode WRITE setVideoFillMode NOTIFY videoFillModeChanged)
    Q_PROPERTY(Language language READ language WRITE setLanguage NOTIFY languageChanged)
    Q_PROPERTY(Theme theme READ theme WRITE setTheme NOTIFY themeChanged)

    Q_PROPERTY(QString decoder READ decoder WRITE setDecoder NOTIFY decoderChanged)
    Q_PROPERTY(QString gitBuildHash READ gitBuildHash WRITE setGitBuildHash NOTIFY gitBuildHashChanged)
    Q_PROPERTY(QString font READ font WRITE setFont NOTIFY fontChanged)

public:
    explicit Settings(
        const std::shared_ptr<GlobalVariables>& globalVariables,
        QObject* parent = nullptr);

    enum class Language {
        En,
        De,
        Ru,
        Fr,
        Es,
        Ko,
        Vi
    };
    Q_ENUM(Language)

    enum class Theme {
        System,
        Dark,
        Light
    };
    Q_ENUM(Theme)

    bool offlineMode() const
    {
        return m_offlineMode;
    }

    bool getOfflineMode() const
    {
        return m_offlineMode;
    }

    bool autostart() const
    {
        return m_autostart;
    }

    bool highPriorityStart() const
    {
        return m_highPriorityStart;
    }

    QString decoder() const
    {
        return m_decoder;
    }

    QString gitBuildHash() const
    {
        return m_gitBuildHash;
    }

    bool silentStart() const
    {
        return m_silentStart;
    }

    bool anonymousTelemetry() const
    {
        return m_anonymousTelemetry;
    }

    bool checkWallpaperVisible() const
    {
        return m_checkWallpaperVisible;
    }

    ScreenPlay::FillMode::FillMode videoFillMode() const
    {
        return m_videoFillMode;
    }

    Language language() const
    {
        return m_language;
    }

public:
    void setupLanguage();

    QString font() const
    {
        return m_font;
    }

    Theme theme() const
    {
        return m_theme;
    }

signals:
    void requestRetranslation();
    void resetInstalledListmodel();

    void autostartChanged(bool autostart);
    void highPriorityStartChanged(bool highPriorityStart);
    void hasWorkshopBannerSeenChanged(bool hasWorkshopBannerSeen);
    void decoderChanged(QString decoder);
    void setMainWindowVisible(bool visible);
    void offlineModeChanged(bool offlineMode);
    void gitBuildHashChanged(QString gitBuildHash);
    void silentStartChanged(bool silentStart);
    void anonymousTelemetryChanged(bool anonymousTelemetry);
    void checkWallpaperVisibleChanged(bool checkWallpaperVisible);
    void videoFillModeChanged(ScreenPlay::FillMode::FillMode videoFillMode);
    void languageChanged(ScreenPlay::Settings::Language language);
    void fontChanged(QString font);
    void themeChanged(ScreenPlay::Settings::Theme theme);

public slots:
    void writeJsonFileFromResource(const QString& filename);
    void setupWidgetAndWindowPaths();
    bool retranslateUI();

    void setqSetting(const QString& key, const QVariant& value)
    {
        m_qSettings.setValue(key, value);
        m_qSettings.sync();
    }

    void setAutostart(bool autostart)
    {
        if (m_autostart == autostart)
            return;

#ifdef Q_OS_WIN
        QSettings settings("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
        if (autostart) {
            settings.setValue("ScreenPlay", QDir::toNativeSeparators(QCoreApplication::applicationFilePath()) + " -silent");
            settings.sync();
        } else {
            settings.remove("ScreenPlay");
        }
#endif

        setqSetting("Autostart", autostart);

        m_autostart = autostart;
        emit autostartChanged(m_autostart);
    }

    void setHighPriorityStart(bool highPriorityStart)
    {
        if (m_highPriorityStart == highPriorityStart)
            return;

        setqSetting("HighPriorityStart", highPriorityStart);

        m_highPriorityStart = highPriorityStart;
        emit highPriorityStartChanged(m_highPriorityStart);
    }

    void setLocalStoragePath(QUrl localStoragePath)
    {
        //Remove: "file:///"
        QJsonValue cleanedPath = QJsonValue(localStoragePath.toString());

        setqSetting("ScreenPlayContentPath", cleanedPath);

        m_globalVariables->setLocalStoragePath(cleanedPath.toString());
        emit resetInstalledListmodel();
    }

    void setDecoder(QString decoder)
    {
        if (m_decoder == decoder)
            return;

        m_decoder = decoder;

        emit decoderChanged(m_decoder);
    }

    void setOfflineMode(bool offlineMode)
    {
        if (m_offlineMode == offlineMode)
            return;

        m_offlineMode = offlineMode;
        emit offlineModeChanged(m_offlineMode);
    }

    void setGitBuildHash(QString gitBuildHash)
    {
        if (m_gitBuildHash == gitBuildHash)
            return;

        m_gitBuildHash = gitBuildHash;
        emit gitBuildHashChanged(m_gitBuildHash);
    }

    void setSilentStart(bool silentStart)
    {
        if (m_silentStart == silentStart)
            return;

        m_silentStart = silentStart;
        emit silentStartChanged(m_silentStart);
    }

    void setAnonymousTelemetry(bool anonymousTelemetry)
    {
        if (m_anonymousTelemetry == anonymousTelemetry)
            return;

        setqSetting("AnonymousTelemetry", anonymousTelemetry);

        m_anonymousTelemetry = anonymousTelemetry;
        emit anonymousTelemetryChanged(m_anonymousTelemetry);
    }

    void setCheckWallpaperVisible(bool checkWallpaperVisible)
    {
        if (m_checkWallpaperVisible == checkWallpaperVisible)
            return;

        setqSetting("CheckWallpaperVisible", checkWallpaperVisible);

        m_checkWallpaperVisible = checkWallpaperVisible;
        emit checkWallpaperVisibleChanged(m_checkWallpaperVisible);
    }

    void setVideoFillMode(ScreenPlay::FillMode::FillMode videoFillMode)
    {
        if (m_videoFillMode == videoFillMode)
            return;

        setqSetting("VideoFillMode", QVariant::fromValue(videoFillMode).toString());

        m_videoFillMode = videoFillMode;
        emit videoFillModeChanged(m_videoFillMode);
    }

    void setLanguage(ScreenPlay::Settings::Language language)
    {
        if (m_language == language)
            return;

        setqSetting("Language", QVariant::fromValue(language).toString());

        m_language = language;
        emit languageChanged(m_language);
    }

    void setFont(QString font)
    {
        if (m_font == font)
            return;

        m_font = font;
        emit fontChanged(m_font);
    }

    void setTheme(ScreenPlay::Settings::Theme theme)
    {
        if (m_theme == theme)
            return;

        setqSetting("Theme", QVariant::fromValue(theme).toString());

        m_theme = theme;
        emit themeChanged(m_theme);
    }

private:
    void restoreDefault(const QString& appConfigLocation, const QString& settingsFileType);

private:
    QSettings m_qSettings;
    QTranslator m_translator;

    const std::shared_ptr<GlobalVariables>& m_globalVariables;

    bool m_autostart { true };
    bool m_highPriorityStart { true };
    bool m_offlineMode { true };
    bool m_checkWallpaperVisible { false };
    bool m_silentStart { false };
    bool m_anonymousTelemetry { true };

    QString m_gitBuildHash;
    QString m_decoder;
    ScreenPlay::FillMode::FillMode m_videoFillMode { ScreenPlay::FillMode::FillMode::Cover };
    Language m_language { Language::En };
    Theme m_theme { Theme::System };
    QString m_font { "Roboto" };
};
}
