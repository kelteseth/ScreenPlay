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

template <typename T>
T QStringToEnum(const QString& key, const T defaultValue)
{
    auto metaEnum = QMetaEnum::fromType<T>();

    bool ok = false;
    T wantedEnum = static_cast<T>(metaEnum.keyToValue(key.toUtf8(), &ok));

    if (ok) {
        return wantedEnum;
    }

    return defaultValue;
}

class Settings : public QObject {
    Q_OBJECT

    Q_PROPERTY(bool anonymousTelemetry READ anonymousTelemetry WRITE setAnonymousTelemetry NOTIFY anonymousTelemetryChanged)
    Q_PROPERTY(bool silentStart READ silentStart WRITE setSilentStart NOTIFY silentStartChanged)
    Q_PROPERTY(bool autostart READ autostart WRITE setAutostart NOTIFY autostartChanged)
    Q_PROPERTY(bool highPriorityStart READ highPriorityStart WRITE setHighPriorityStart NOTIFY highPriorityStartChanged)
    Q_PROPERTY(bool checkWallpaperVisible READ checkWallpaperVisible WRITE setCheckWallpaperVisible NOTIFY checkWallpaperVisibleChanged)
    Q_PROPERTY(bool offlineMode READ offlineMode WRITE setOfflineMode NOTIFY offlineModeChanged)

    Q_PROPERTY(FillMode videoFillMode READ videoFillMode WRITE setVideoFillMode NOTIFY videoFillModeChanged)
    Q_PROPERTY(Language language READ language WRITE setLanguage NOTIFY languageChanged)

    Q_PROPERTY(QString decoder READ decoder WRITE setDecoder NOTIFY decoderChanged)
    Q_PROPERTY(QString gitBuildHash READ gitBuildHash WRITE setGitBuildHash NOTIFY gitBuildHashChanged)
    Q_PROPERTY(QString font READ font WRITE setFont NOTIFY fontChanged)

public:
    explicit Settings(
        const shared_ptr<GlobalVariables>& globalVariables,
        QObject* parent = nullptr);

    enum class FillMode {
        Stretch,
        Fill,
        Contain,
        Cover,
        Scale_Down
    };
    Q_ENUM(FillMode)

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

    FillMode videoFillMode() const
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

signals:
    void requestRetranslation();

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
    void checkWallpaperVisibleChanged(bool checkWallpaperVisible);
    void videoFillModeChanged(FillMode videoFillMode);
    void languageChanged(Language language);

    void fontChanged(QString font);

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
#endif
        }

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

    void setVideoFillMode(FillMode videoFillMode)
    {
        if (m_videoFillMode == videoFillMode)
            return;

        setqSetting("VideoFillMode", QVariant::fromValue(videoFillMode).toString());

        m_videoFillMode = videoFillMode;
        emit videoFillModeChanged(m_videoFillMode);
    }

    void setLanguage(Language language)
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

private:
    void restoreDefault(const QString& appConfigLocation, const QString& settingsFileType);

private:
    QSettings m_qSettings;
    QTranslator m_translator;

    const shared_ptr<GlobalVariables>& m_globalVariables;

    bool m_autostart { true };
    bool m_highPriorityStart { true };
    bool m_offlineMode { true };
    bool m_checkWallpaperVisible { false };
    bool m_silentStart { false };
    bool m_anonymousTelemetry { true };

    QString m_gitBuildHash;
    QString m_decoder;
    FillMode m_videoFillMode;
    Language m_language = Language::En;
    QString m_font {"Roboto"};
};
}
