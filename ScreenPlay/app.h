#pragma once

#include <QDir>
#include <QIcon>
#include <QObject>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickWindow>
#include <QStringList>
#include <QUrl>
#include <QtGlobal>
#include <qqml.h>

#include <QtWebEngine>

#include <memory>

#include "ganalytics.h"
#include "src/create.h"
#include "src/globalvariables.h"
#include "src/installedlistfilter.h"
#include "src/installedlistmodel.h"
#include "src/monitorlistmodel.h"
#include "src/profilelistmodel.h"
#include "src/screenplaymanager.h"
#include "src/sdkconnector.h"
#include "src/settings.h"
#include "src/util.h"

using std::make_unique,
    std::unique_ptr,
    std::make_shared,
    std::shared_ptr,
    ScreenPlay::Util,
    ScreenPlay::InstalledListModel,
    ScreenPlay::ScreenPlayManager,
    ScreenPlay::InstalledListFilter,
    ScreenPlay::MonitorListModel,
    ScreenPlay::ProfileListModel,
    ScreenPlay::SDKConnector,
    ScreenPlay::Settings,
    ScreenPlay::Create;

class App : public QObject {
    Q_OBJECT

    Q_PROPERTY(QQmlApplicationEngine* mainWindowEngine READ mainWindowEngine WRITE setMainWindowEngine NOTIFY mainWindowEngineChanged)
    Q_PROPERTY(GlobalVariables* globalVariables READ globalVariables WRITE setGlobalVariables NOTIFY globalVariablesChanged)
    Q_PROPERTY(ScreenPlayManager* screenPlayManager READ screenPlayManager WRITE setScreenPlayManager NOTIFY screenPlayManagerChanged)
    Q_PROPERTY(Create* create READ create WRITE setCreate NOTIFY createChanged)
    Q_PROPERTY(Util* util READ util WRITE setUtil NOTIFY utilChanged)
    Q_PROPERTY(Settings* settings READ settings WRITE setSettings NOTIFY settingsChanged)
    Q_PROPERTY(SDKConnector* sdkConnector READ sdkConnector WRITE setSdkConnector NOTIFY sdkConnectorChanged)

    Q_PROPERTY(InstalledListModel* installedListModel READ installedListModel WRITE setInstalledListModel NOTIFY installedListModelChanged)
    Q_PROPERTY(InstalledListFilter* installedListFilter READ installedListFilter WRITE setInstalledListFilter NOTIFY installedListFilterChanged)
    Q_PROPERTY(MonitorListModel* monitorListModel READ monitorListModel WRITE setMonitorListModel NOTIFY monitorListModelChanged)
    Q_PROPERTY(ProfileListModel* profileListModel READ profileListModel WRITE setProfileListModel NOTIFY profileListModelChanged)

public:
    explicit App();

#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    static App* instance()
    {
        static App app;
        return &app;
    }
#endif
    GlobalVariables* globalVariables() const
    {
        return m_globalVariables.get();
    }

    ScreenPlayManager* screenPlayManager() const
    {
        return m_screenPlayManager.get();
    }

    Create* create() const
    {
        return m_create.get();
    }

    Util* util() const
    {
        return m_util.get();
    }

    Settings* settings() const
    {
        return m_settings.get();
    }

    InstalledListModel* installedListModel() const
    {
        return m_installedListModel.get();
    }

    MonitorListModel* monitorListModel() const
    {
        return m_monitorListModel.get();
    }

    ProfileListModel* profileListModel() const
    {
        return m_profileListModel.get();
    }

    InstalledListFilter* installedListFilter() const
    {
        return m_installedListFilter.get();
    }

    SDKConnector* sdkConnector() const
    {
        return m_sdkConnector.get();
    }

    QQmlApplicationEngine* mainWindowEngine() const
    {
        return m_mainWindowEngine.get();
    }

signals:
    void globalVariablesChanged(GlobalVariables* globalVariables);
    void screenPlayManagerChanged(ScreenPlayManager* screenPlayManager);
    void createChanged(Create* create);
    void utilChanged(Util* util);
    void settingsChanged(Settings* settings);
    void installedListModelChanged(InstalledListModel* installedListModel);
    void monitorListModelChanged(MonitorListModel* monitorListModel);
    void profileListModelChanged(ProfileListModel* profileListModel);
    void installedListFilterChanged(InstalledListFilter* installedListFilter);
    void sdkConnectorChanged(SDKConnector* sdkConnector);
    void mainWindowEngineChanged(QQmlApplicationEngine* mainWindowEngine);

public slots:

    void exit();
    void setTrackerSendEvent(const QString& page){
        if(m_tracker){
            m_tracker->sendEvent("navigation",page);
        }
    }


    void setGlobalVariables(GlobalVariables* globalVariables)
    {
        if (m_globalVariables.get() == globalVariables)
            return;

        m_globalVariables.reset(globalVariables);
        emit globalVariablesChanged(m_globalVariables.get());
    }
    void setScreenPlayManager(ScreenPlayManager* screenPlayManager)
    {
        if (m_screenPlayManager.get() == screenPlayManager)
            return;

        m_screenPlayManager.reset(screenPlayManager);
        emit screenPlayManagerChanged(m_screenPlayManager.get());
    }

    void setCreate(Create* create)
    {
        if (m_create.get() == create)
            return;

        m_create.reset(create);
        emit createChanged(m_create.get());
    }

    void setUtil(Util* util)
    {
        if (m_util.get() == util)
            return;

        m_util.reset(util);
        emit utilChanged(m_util.get());
    }

    void setSettings(Settings* settings)
    {
        if (m_settings.get() == settings)
            return;

        m_settings.reset(settings);
        emit settingsChanged(m_settings.get());
    }

    void setInstalledListModel(InstalledListModel* installedListModel)
    {
        if (m_installedListModel.get() == installedListModel)
            return;

        m_installedListModel.reset(installedListModel);
        emit installedListModelChanged(m_installedListModel.get());
    }
    void setMonitorListModel(MonitorListModel* monitorListModel)
    {
        if (m_monitorListModel.get() == monitorListModel)
            return;

        m_monitorListModel.reset(monitorListModel);
        emit monitorListModelChanged(m_monitorListModel.get());
    }

    void setProfileListModel(ProfileListModel* profileListModel)
    {
        if (m_profileListModel.get() == profileListModel)
            return;

        m_profileListModel.reset(profileListModel);
        emit profileListModelChanged(m_profileListModel.get());
    }

    void setInstalledListFilter(InstalledListFilter* installedListFilter)
    {
        if (m_installedListFilter.get() == installedListFilter)
            return;

        m_installedListFilter.reset(installedListFilter);
        emit installedListFilterChanged(m_installedListFilter.get());
    }

    void setSdkConnector(SDKConnector* sdkConnector)
    {
        if (m_sdkConnector.get() == sdkConnector)
            return;

        m_sdkConnector.reset(sdkConnector);
        emit sdkConnectorChanged(m_sdkConnector.get());
    }

    void setMainWindowEngine(QQmlApplicationEngine* mainWindowEngine)
    {
        if (m_mainWindowEngine.get() == mainWindowEngine)
            return;

        m_mainWindowEngine.reset(mainWindowEngine);
        emit mainWindowEngineChanged(m_mainWindowEngine.get());
    }

private:
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    unique_ptr<QQmlApplicationEngine> m_mainWindowEngine;
#endif

    unique_ptr<GAnalytics> m_tracker;
    unique_ptr<Create> m_create;
    unique_ptr<ScreenPlayManager> m_screenPlayManager;
    unique_ptr<Util> m_util;

    shared_ptr<GlobalVariables> m_globalVariables;
    shared_ptr<Settings> m_settings;
    shared_ptr<SDKConnector> m_sdkConnector;

    shared_ptr<InstalledListModel> m_installedListModel;
    shared_ptr<MonitorListModel> m_monitorListModel;
    shared_ptr<ProfileListModel> m_profileListModel;
    shared_ptr<InstalledListFilter> m_installedListFilter;
};
