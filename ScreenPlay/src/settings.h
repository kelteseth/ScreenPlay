#pragma once

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFontDatabase>
#include <QGuiApplication>
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

using std::shared_ptr,
    std::make_shared;

class Settings : public QObject {
    Q_OBJECT

    Q_PROPERTY(QVersionNumber version READ version)

    Q_PROPERTY(bool anonymousTelemetry READ anonymousTelemetry WRITE setAnonymousTelemetry NOTIFY anonymousTelemetryChanged)
    Q_PROPERTY(bool silentStart READ silentStart WRITE setSilentStart NOTIFY silentStartChanged)
    Q_PROPERTY(bool autostart READ autostart WRITE setAutostart NOTIFY autostartChanged)
    Q_PROPERTY(bool highPriorityStart READ highPriorityStart WRITE setHighPriorityStart NOTIFY highPriorityStartChanged)
    Q_PROPERTY(bool checkWallpaperVisible READ checkWallpaperVisible WRITE setCheckWallpaperVisible NOTIFY checkWallpaperVisibleChanged)
    Q_PROPERTY(bool offlineMode READ offlineMode WRITE setOfflineMode NOTIFY offlineModeChanged)

    Q_PROPERTY(QString decoder READ decoder WRITE setDecoder NOTIFY decoderChanged)
    Q_PROPERTY(QString gitBuildHash READ gitBuildHash WRITE setGitBuildHash NOTIFY gitBuildHashChanged)
    Q_PROPERTY(QString language READ language WRITE setLanguage NOTIFY languageChanged)

public:
    explicit Settings(
        const shared_ptr<GlobalVariables>& globalVariables,
        QObject* parent = nullptr);

    QVersionNumber version() const
    {
        return m_version;
    }

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

    QString language() const
    {
        return m_language;
    }

    bool checkWallpaperVisible() const
    {
        return m_checkWallpaperVisible;
    }

signals:
    void autostartChanged(bool autostart);
    void highPriorityStartChanged(bool highPriorityStart);
    void hasWorkshopBannerSeenChanged(bool hasWorkshopBannerSeen);
    void decoderChanged(QString decoder);
    void setMainWindowVisible(bool visible);
    void offlineModeChanged(bool offlineMode);
    void gitBuildHashChanged(QString gitBuildHash);
    void resetInstalledListmodel();
    void silentStartChanged(bool silentStart);
    void anonymousTelemetryChanged(bool anonymousTelemetry);
    void languageChanged(QString language);
    void checkWallpaperVisibleChanged(bool checkWallpaperVisible);

public slots:
    bool writeSingleSettingConfig(QString name, QVariant value);
    void writeJsonFileFromResource(const QString& filename);
    void setupWidgetAndWindowPaths();
    void setupLanguage();
    void setqSetting(const QString& key, const QString& value)
    {
        m_qSettings.setValue(key, value);
        m_qSettings.sync();
    }

    void setAutostart(bool autostart)
    {
        if (m_autostart == autostart)
            return;

        if (autostart) {
#ifdef Q_OS_WIN

            QSettings settings("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
            settings.setValue("ScreenPlay", QDir::toNativeSeparators(QCoreApplication::applicationFilePath()) + " -silent");
            settings.sync();
#endif
        } else {
#ifdef Q_OS_WIN
            QSettings settings("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
            settings.remove("ScreenPlay");
#endif
        }

        m_autostart = autostart;
        writeSingleSettingConfig("autostart", autostart);
        emit autostartChanged(m_autostart);
    }

    void setHighPriorityStart(bool highPriorityStart)
    {
        if (m_highPriorityStart == highPriorityStart)
            return;

        m_highPriorityStart = highPriorityStart;
        writeSingleSettingConfig("highPriorityStart", highPriorityStart);
        emit highPriorityStartChanged(m_highPriorityStart);
    }

    void setLocalStoragePath(QUrl localStoragePath)
    {
        //Remove: "file:///"
        QJsonValue cleanedPath = QJsonValue(localStoragePath.toString());

        writeSingleSettingConfig("absoluteStoragePath", cleanedPath);

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

        m_anonymousTelemetry = anonymousTelemetry;
        emit anonymousTelemetryChanged(m_anonymousTelemetry);
    }

    void setLanguage(QString language)
    {
        if (m_language == language)
            return;

        m_language = language;
        emit languageChanged(m_language);
    }

    void setCheckWallpaperVisible(bool checkWallpaperVisible)
    {
        if (m_checkWallpaperVisible == checkWallpaperVisible)
            return;

        writeSingleSettingConfig("checkWallpaperVisible", checkWallpaperVisible);
        m_checkWallpaperVisible = checkWallpaperVisible;
        emit checkWallpaperVisibleChanged(m_checkWallpaperVisible);
    }

private:
    void restoreDefault(const QString& appConfigLocation, const QString& settingsFileType);

private:
    QVersionNumber m_version;
    QSettings m_qSettings;
    QTranslator m_translator;

    const shared_ptr<GlobalVariables>& m_globalVariables;

    bool m_autostart { true };
    bool m_highPriorityStart { true };
    bool m_offlineMode { true };
    bool m_checkWallpaperVisible { true };
    bool m_silentStart { false };
    bool m_anonymousTelemetry { true };

    QString m_gitBuildHash;
    QString m_decoder { "" };
    QString m_language;
};
}
