// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#pragma once
#include "ScreenPlay/create.h"
#include "ScreenPlay/globalvariables.h"
#include "ScreenPlay/installedlistfilter.h"
#include "ScreenPlay/installedlistmodel.h"
#include "ScreenPlay/monitorlistmodel.h"
#include "ScreenPlay/profilelistmodel.h"
#include "ScreenPlay/screenplaymanager.h"
#include "ScreenPlay/settings.h"
#include "ScreenPlay/wizards.h"
#include "ScreenPlayCore/util.h"

#include <QQmlEngine>
#include <QString>
#include <memory>

#if defined(Q_OS_WIN)
#include <sentry.h>
#endif

namespace ScreenPlay {

class App : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("CPP ONLY")
    QML_SINGLETON

    // We must add the namespace here to make
    // it work with QtC autocompletion, see QTCREATORBUG-30197
    Q_PROPERTY(ScreenPlay::GlobalVariables* globalVariables READ globalVariables WRITE setGlobalVariables NOTIFY globalVariablesChanged FINAL)
    Q_PROPERTY(ScreenPlay::ScreenPlayManager* screenPlayManager READ screenPlayManager WRITE setScreenPlayManager NOTIFY screenPlayManagerChanged FINAL)
    Q_PROPERTY(ScreenPlay::Create* create READ create WRITE setCreate NOTIFY createChanged FINAL)
    Q_PROPERTY(ScreenPlay::Wizards* wizards READ wizards WRITE setWizards NOTIFY wizardsChanged FINAL)
    Q_PROPERTY(ScreenPlay::Util* util READ util WRITE setUtil NOTIFY utilChanged FINAL)
    Q_PROPERTY(ScreenPlay::Settings* settings READ settings WRITE setSettings NOTIFY settingsChanged FINAL)
    Q_PROPERTY(ScreenPlay::InstalledListModel* installedListModel READ installedListModel WRITE setInstalledListModel NOTIFY installedListModelChanged FINAL)
    Q_PROPERTY(ScreenPlay::InstalledListFilter* installedListFilter READ installedListFilter WRITE setInstalledListFilter NOTIFY installedListFilterChanged FINAL)
    Q_PROPERTY(ScreenPlay::MonitorListModel* monitorListModel READ monitorListModel WRITE setMonitorListModel NOTIFY monitorListModelChanged FINAL)
    Q_PROPERTY(ScreenPlay::ProfileListModel* profileListModel READ profileListModel WRITE setProfileListModel NOTIFY profileListModelChanged FINAL)

public:
    explicit App(QObject* parent = nullptr);
    Q_INVOKABLE QString version() const;
    Q_INVOKABLE void showDockIcon(const bool show);
    Q_INVOKABLE void exit();

    GlobalVariables* globalVariables() const { return m_globalVariables.get(); }
    ScreenPlayManager* screenPlayManager() const { return m_screenPlayManager.get(); }
    Create* create() const { return m_create.get(); }
    Util* util() const { return m_util.get(); }
    Settings* settings() const { return m_settings.get(); }
    InstalledListModel* installedListModel() const { return m_installedListModel.get(); }
    MonitorListModel* monitorListModel() const { return m_monitorListModel.get(); }
    ProfileListModel* profileListModel() const { return m_profileListModel.get(); }
    InstalledListFilter* installedListFilter() const { return m_installedListFilter.get(); }
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
    void wizardsChanged(ScreenPlay::Wizards* wizards);
    void requestExit();
    void requestRetranslation();

public slots:
    void setGlobalVariables(GlobalVariables* globalVariables);
    void setScreenPlayManager(ScreenPlayManager* screenPlayManager);
    void setCreate(Create* create);
    void setUtil(Util* util);
    void setSettings(Settings* settings);
    void setInstalledListModel(InstalledListModel* installedListModel);
    void setMonitorListModel(MonitorListModel* monitorListModel);
    void setProfileListModel(ProfileListModel* profileListModel);
    void setInstalledListFilter(InstalledListFilter* installedListFilter);
    void setWizards(Wizards* wizards);

private:
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
