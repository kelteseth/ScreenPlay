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

#include "installedlistmodel.h"
#include "monitorlistmodel.h"
#include "profile.h"
#include "profilelistmodel.h"
#include "projectsettingslistmodel.h"
#include "sdkconnector.h"

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
    Q_PROPERTY(bool autostart READ autostart WRITE setAutostart NOTIFY autostartChanged)
    Q_PROPERTY(bool highPriorityStart READ highPriorityStart WRITE setHighPriorityStart NOTIFY highPriorityStartChanged)
    Q_PROPERTY(bool sendStatistics READ sendStatistics WRITE setSendStatistics NOTIFY sendStatisticsChanged)
    Q_PROPERTY(bool pauseWallpaperWhenIngame READ pauseWallpaperWhenIngame WRITE setPauseWallpaperWhenIngame NOTIFY pauseWallpaperWhenIngameChanged)
    Q_PROPERTY(bool offlineMode READ offlineMode WRITE setOfflineMode NOTIFY offlineModeChanged)
    Q_PROPERTY(QUrl localStoragePath READ localStoragePath WRITE setLocalStoragePath NOTIFY localStoragePathChanged)
    Q_PROPERTY(QString decoder READ decoder WRITE setDecoder NOTIFY decoderChanged)

    Q_PROPERTY(int activeWallpaperCounter READ activeWallpaperCounter WRITE setActiveWallpaperCounter NOTIFY activeWallpaperCounterChanged)
    Q_PROPERTY(int activeWidgetsCounter READ activeWidgetsCounter WRITE setActiveWidgetsCounter NOTIFY activeWidgetsCounterChanged)

public:
    explicit Settings(
        const shared_ptr<InstalledListModel>& ilm,
        const shared_ptr<ProfileListModel>& plm,
        const shared_ptr<MonitorListModel>& mlm,
        const shared_ptr<SDKConnector>& sdkc,
        QObject* parent = nullptr);
    ~Settings() {}

    QVersionNumber version() const
    {
        return m_version;
    }

    QUrl screenPlayWindowPath() const
    {
        return m_screenPlayWindowPath;
    }

    QUrl localStoragePath() const
    {
        return m_localStoragePath;
    }

    int activeWallpaperCounter() const
    {
        return m_activeWallpaperCounter;
    }

    bool pauseWallpaperWhenIngame() const
    {
        return m_pauseWallpaperWhenIngame;
    }

    bool offlineMode() const
    {
        return m_offlineMode;
    }

    QUrl getScreenPlayWindowPath() const
    {
        return m_screenPlayWindowPath;
    }

    void setScreenPlayWindowPath(const QUrl& screenPlayWindowPath)
    {
        m_screenPlayWindowPath = screenPlayWindowPath;
    }

    QUrl getScreenPlayBasePath() const
    {
        return m_screenPlayBasePath;
    }

    void setScreenPlayBasePath(QUrl screenPlayBasePath)
    {
        m_screenPlayBasePath = screenPlayBasePath;
    }

    QUrl getScreenPlayWidgetPath() const
    {
        return m_screenPlayWidgetPath;
    }

    void setScreenPlayWidgetPath(const QUrl& screenPlayWidgetPath)
    {
        m_screenPlayWidgetPath = screenPlayWidgetPath;
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

    bool sendStatistics() const
    {
        return m_sendStatistics;
    }

    QString decoder() const
    {
        return m_decoder;
    }

    int activeWidgetsCounter() const
    {
        return m_activeWidgetsCounter;
    }

signals:
    void autostartChanged(bool autostart);
    void highPriorityStartChanged(bool highPriorityStart);
    void sendStatisticsChanged(bool status);
    void localStoragePathChanged(QUrl localStoragePath);
    void hasWorkshopBannerSeenChanged(bool hasWorkshopBannerSeen);
    void decoderChanged(QString decoder);
    void setMainWindowVisible(bool visible);
    void activeWallpaperCounterChanged(int activeWallpaperCounter);
    void pauseWallpaperWhenIngameChanged(bool pauseWallpaperWhenIngame);
    void offlineModeChanged(bool offlineMode);

    void activeWidgetsCounterChanged(int activeWidgetsCounter);

public slots:
    void writeSingleSettingConfig(QString name, QVariant value);
    void saveWallpaperToConfig(const int monitorIndex, const QUrl& absoluteStoragePath, const QString& type);
    void removeWallpaperFromConfig(const int monitorIndex);
    void setqSetting(const QString& key, const QString& value);

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

    void setSendStatistics(bool sendStatistics)
    {
        if (m_sendStatistics == sendStatistics)
            return;

        m_sendStatistics = sendStatistics;

        writeSingleSettingConfig("sendStatistics", sendStatistics);
        emit sendStatisticsChanged(m_sendStatistics);
    }

    void setLocalStoragePath(QUrl localStoragePath)
    {
        if (m_localStoragePath == localStoragePath)
            return;

        //Remove: "file:///"
        QJsonValue cleanedPath = QJsonValue(localStoragePath.toString()); // QJsonValue(QString(localStoragePath.toString()).remove(0, 8));

        writeSingleSettingConfig("absoluteStoragePath", cleanedPath);

        m_installedListModel->setabsoluteStoragePath(cleanedPath.toString());
        m_localStoragePath = cleanedPath.toString();
        emit localStoragePathChanged(cleanedPath.toString());
        m_installedListModel->reset();
        m_installedListModel->loadInstalledContent();
    }

    void setDecoder(QString decoder)
    {
        if (m_decoder == decoder)
            return;

        m_decoder = decoder;

        emit decoderChanged(m_decoder);
    }

    void setActiveWallpaperCounter(int activeWallpaperCounter)
    {
        if (m_activeWallpaperCounter == activeWallpaperCounter)
            return;

        m_activeWallpaperCounter = activeWallpaperCounter;
        emit activeWallpaperCounterChanged(m_activeWallpaperCounter);
    }

    void increaseActiveWidgetsCounter()
    {
        m_activeWidgetsCounter++;
        emit activeWidgetsCounterChanged(m_activeWidgetsCounter);
    }

    void decreaseActivewidgetsCounter()
    {
        m_activeWidgetsCounter--;
        emit activeWidgetsCounterChanged(m_activeWidgetsCounter);
    }

    void increaseActiveWallpaperCounter()
    {
        m_activeWallpaperCounter++;
        emit activeWallpaperCounterChanged(m_activeWallpaperCounter);
    }

    void decreaseActiveWallpaperCounter()
    {
        if (m_activeWallpaperCounter <= 0) {
            return;
        }
        m_activeWallpaperCounter--;
        emit activeWallpaperCounterChanged(m_activeWallpaperCounter);
    }

    void setPauseWallpaperWhenIngame(bool pauseWallpaperWhenIngame)
    {
        if (m_pauseWallpaperWhenIngame == pauseWallpaperWhenIngame)
            return;

        m_pauseWallpaperWhenIngame = pauseWallpaperWhenIngame;
        emit pauseWallpaperWhenIngameChanged(m_pauseWallpaperWhenIngame);
    }

    void setOfflineMode(bool offlineMode)
    {
        if (m_offlineMode == offlineMode)
            return;

        m_offlineMode = offlineMode;
        emit offlineModeChanged(m_offlineMode);
    }

    void loadActiveProfiles();

    void setActiveWidgetsCounter(int activeWidgetsCounter)
    {
        if (m_activeWidgetsCounter == activeWidgetsCounter)
            return;

        m_activeWidgetsCounter = activeWidgetsCounter;
        emit activeWidgetsCounterChanged(m_activeWidgetsCounter);
    }

private:
    void createDefaultConfig();
    void setupWidgetAndWindowPaths();

    QVersionNumber m_version;
    QSettings m_qSettings;
    QTranslator m_translator;

    const shared_ptr<ProfileListModel> m_profileListModel;
    const shared_ptr<InstalledListModel> m_installedListModel;
    const shared_ptr<MonitorListModel> m_monitorListModel;
    const shared_ptr<SDKConnector> m_sdkconnector;

    QUrl m_localStoragePath;
    QUrl m_localSettingsPath;
    QUrl m_screenPlayWindowPath;
    QUrl m_screenPlayWidgetPath;
    QUrl m_screenPlayBasePath;

    bool m_pauseWallpaperWhenIngame { true };
    bool m_autostart { true };
    bool m_highPriorityStart { true };
    bool m_sendStatistics { false };
    bool m_offlineMode { true };

    QString m_decoder { "" };
    int m_activeWallpaperCounter { 0 };
    int m_activeWidgetsCounter { 0 };
};
}
