// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#pragma once

#include <QDir>
#include <QElapsedTimer>
#include <QGuiApplication>
#include <QIcon>
#include <QObject>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickWindow>
#include <QStringList>
#include <QUrl>
#include <QtGlobal>
#include <QtQml>
#include <QtSvg>

#include "ScreenPlay/create.h"
#include "ScreenPlay/globalvariables.h"
#include "ScreenPlay/installedlistfilter.h"
#include "ScreenPlay/installedlistmodel.h"
#include "ScreenPlay/monitorlistmodel.h"
#include "ScreenPlay/profilelistmodel.h"
#include "ScreenPlay/screenplaymanager.h"
#include "ScreenPlay/settings.h"
#include "ScreenPlay/util.h"
#include "ScreenPlay/wizards.h"

#include <memory>

#if defined(Q_OS_WIN)
#include <sentry.h>
#endif

namespace ScreenPlay {

class App : public QObject {
    Q_OBJECT

    Q_PROPERTY(QQmlApplicationEngine* mainWindowEngine READ mainWindowEngine WRITE setMainWindowEngine NOTIFY mainWindowEngineChanged)
    Q_PROPERTY(GlobalVariables* globalVariables READ globalVariables WRITE setGlobalVariables NOTIFY globalVariablesChanged)
    Q_PROPERTY(ScreenPlayManager* screenPlayManager READ screenPlayManager WRITE setScreenPlayManager NOTIFY screenPlayManagerChanged)
    Q_PROPERTY(Create* create READ create WRITE setCreate NOTIFY createChanged)
    Q_PROPERTY(Wizards* wizards READ wizards WRITE setWizards NOTIFY wizardsChanged)
    Q_PROPERTY(Util* util READ util WRITE setUtil NOTIFY utilChanged)
    Q_PROPERTY(Settings* settings READ settings WRITE setSettings NOTIFY settingsChanged)

    Q_PROPERTY(InstalledListModel* installedListModel READ installedListModel WRITE setInstalledListModel NOTIFY installedListModelChanged)
    Q_PROPERTY(InstalledListFilter* installedListFilter READ installedListFilter WRITE setInstalledListFilter NOTIFY installedListFilterChanged)
    Q_PROPERTY(MonitorListModel* monitorListModel READ monitorListModel WRITE setMonitorListModel NOTIFY monitorListModelChanged)
    Q_PROPERTY(ProfileListModel* profileListModel READ profileListModel WRITE setProfileListModel NOTIFY profileListModelChanged)

public:
    explicit App();

    void init();
    bool m_isAnotherScreenPlayInstanceRunning { false };

    GlobalVariables* globalVariables() const { return m_globalVariables.get(); }
    ScreenPlayManager* screenPlayManager() const { return m_screenPlayManager.get(); }
    Create* create() const { return m_create.get(); }
    Util* util() const { return m_util.get(); }
    Settings* settings() const { return m_settings.get(); }
    InstalledListModel* installedListModel() const { return m_installedListModel.get(); }
    MonitorListModel* monitorListModel() const { return m_monitorListModel.get(); }
    ProfileListModel* profileListModel() const { return m_profileListModel.get(); }
    InstalledListFilter* installedListFilter() const { return m_installedListFilter.get(); }
    QQmlApplicationEngine* mainWindowEngine() const { return m_mainWindowEngine.get(); }
    Wizards* wizards() const { return m_wizards.get(); }

signals:
    void globalVariablesChanged(ScreenPlay::GlobalVariables* globalVariables);
    void screenPlayManagerChanged(ScreenPlay::ScreenPlayManager* screenPlayManager);
    void createChanged(ScreenPlay::Create* create);
    void utilChanged(ScreenPlay::Util* util);
    void settingsChanged(ScreenPlay::Settings* settings);
    void installedListModelChanged(ScreenPlay::InstalledListModel* installedListModel);
    void monitorListModelChanged(ScreenPlay::MonitorListModel* monitorListModel);
    void profileListModelChanged(ScreenPlay::ProfileListModel* profileListModel);
    void installedListFilterChanged(ScreenPlay::InstalledListFilter* installedListFilter);
    void mainWindowEngineChanged(QQmlApplicationEngine* mainWindowEngine);
    void wizardsChanged(ScreenPlay::Wizards* wizards);

public slots:
    QString version() const;
    void showDockIcon(const bool show);
    void exit();
    QPointF cursorPos() { return QCursor::pos(); }

    void setGlobalVariables(GlobalVariables* globalVariables);
    void setScreenPlayManager(ScreenPlayManager* screenPlayManager);
    void setCreate(Create* create);
    void setUtil(Util* util);
    void setSettings(Settings* settings);
    void setInstalledListModel(InstalledListModel* installedListModel);
    void setMonitorListModel(MonitorListModel* monitorListModel);
    void setProfileListModel(ProfileListModel* profileListModel);
    void setInstalledListFilter(InstalledListFilter* installedListFilter);
    void setMainWindowEngine(QQmlApplicationEngine* mainWindowEngine);
    void setWizards(Wizards* wizards);

private:
    QNetworkAccessManager m_networkAccessManager;
    std::unique_ptr<QQmlApplicationEngine> m_mainWindowEngine;

    std::unique_ptr<Create> m_create;
    std::unique_ptr<Wizards> m_wizards;
    std::unique_ptr<ScreenPlayManager> m_screenPlayManager;
    std::unique_ptr<Util> m_util;

    std::shared_ptr<GlobalVariables> m_globalVariables;

    std::shared_ptr<Settings> m_settings;
    std::shared_ptr<InstalledListModel> m_installedListModel;
    std::shared_ptr<MonitorListModel> m_monitorListModel;
    std::shared_ptr<ProfileListModel> m_profileListModel;
    std::shared_ptr<InstalledListFilter> m_installedListFilter;
};
}
