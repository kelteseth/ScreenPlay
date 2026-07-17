// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#pragma once
#include "ScreenPlay/create.h"
#include "ScreenPlay/errormanager.h"
#include "ScreenPlay/globalvariables.h"
#include "ScreenPlay/installedlistfilter.h"
#include "ScreenPlay/installedlistmodel.h"
#include "ScreenPlay/monitorlistmodel.h"
#include "ScreenPlay/profilelistmodel.h"
#include "ScreenPlay/screenplaymanager.h"
#include "ScreenPlay/settings.h"
#include "ScreenPlay/uiappstatesignals.h"
#include "ScreenPlay/wizards.h"

#include "ScreenPlayCore/framestats.h"
#include "ScreenPlayCore/godothandler.h"
#include "ScreenPlayCore/util.h"

#include <QPointer>
#include <QQmlApplicationEngine>
#include <QQmlEngine>
#include <QString>
#include <memory>

#if defined(Q_OS_WIN)
#include <sentry.h>
#endif

namespace ScreenPlay {

class App : public QObject {
    Q_OBJECT
    QML_SINGLETON
    QML_ELEMENT

    // Sub-systems are constructor-owned and never replaced — properties are
    // read-only. We must add the namespace here to make it work with QtC
    // autocompletion, see QTCREATORBUG-30197.
    Q_PROPERTY(ScreenPlay::GlobalVariables* globalVariables READ globalVariables CONSTANT FINAL)
    Q_PROPERTY(ScreenPlay::ScreenPlayManager* screenPlayManager READ screenPlayManager CONSTANT FINAL)
    Q_PROPERTY(ScreenPlay::Create* create READ create CONSTANT FINAL)
    Q_PROPERTY(ScreenPlay::Wizards* wizards READ wizards CONSTANT FINAL)
    Q_PROPERTY(ScreenPlay::Util* util READ util CONSTANT FINAL)
    Q_PROPERTY(ScreenPlay::GodotHandler* godotHandler READ godotHandler CONSTANT FINAL)
    Q_PROPERTY(ScreenPlay::Settings* settings READ settings CONSTANT FINAL)
    Q_PROPERTY(ScreenPlay::InstalledListModel* installedListModel READ installedListModel CONSTANT FINAL)
    Q_PROPERTY(ScreenPlay::InstalledListFilter* installedListFilter READ installedListFilter CONSTANT FINAL)
    Q_PROPERTY(ScreenPlay::MonitorListModel* monitorListModel READ monitorListModel CONSTANT FINAL)
    Q_PROPERTY(ScreenPlay::ProfileListModel* profileListModel READ profileListModel CONSTANT FINAL)
    Q_PROPERTY(ScreenPlay::UiAppStateSignals* uiAppStateSignals READ uiAppStateSignals CONSTANT FINAL)
    Q_PROPERTY(ScreenPlay::FrameStats* frameStats READ frameStats CONSTANT FINAL)
    Q_PROPERTY(ScreenPlay::ErrorManager* errorManager READ errorManager CONSTANT FINAL)

public:
    explicit App(QObject* parent = nullptr);
    ~App();

    // Qt 6 declarative singleton factory: invoked by the QML engine on first
    // access. Owns the App instance lifetime; do not call directly.
    static App* create(QQmlEngine* engine, QJSEngine* jsEngine);

    Q_INVOKABLE QString version() const;
    Q_INVOKABLE void showDockIcon(const bool show);
    Q_INVOKABLE QCoro::QmlTask exit();

    GlobalVariables* globalVariables() const { return m_globalVariables.get(); }
    ScreenPlayManager* screenPlayManager() const { return m_screenPlayManager.get(); }
    Create* create() const { return m_create.get(); }
    Util* util() const { return m_util.get(); }
    GodotHandler* godotHandler() const { return m_godotHandler.get(); }
    Settings* settings() const { return m_settings.get(); }
    InstalledListModel* installedListModel() const { return m_installedListModel.get(); }
    MonitorListModel* monitorListModel() const { return m_monitorListModel.get(); }
    ProfileListModel* profileListModel() const { return m_profileListModel.get(); }
    InstalledListFilter* installedListFilter() const { return m_installedListFilter.get(); }
    Wizards* wizards() const { return m_wizards.get(); }
    UiAppStateSignals* uiAppStateSignals() const { return m_uiAppStateSignals.get(); }
    ErrorManager* errorManager() const { return m_errorManager.get(); }
    FrameStats* frameStats() const { return m_frameStats.get(); }

signals:
    void requestExit();
    void requestRetranslation();

private:
    void attachEngine(QQmlEngine* engine);
    // Member coroutine instead of a capturing-lambda coroutine: lambda
    // captures live in the closure object, which would die before the
    // coroutine resumes.
    QCoro::Task<void> exitTask();

    QPointer<QQmlEngine> m_engine;
    std::unique_ptr<Create> m_create;
    std::unique_ptr<FrameStats> m_frameStats;
    std::unique_ptr<Wizards> m_wizards;
    std::unique_ptr<ScreenPlayManager> m_screenPlayManager;
    std::unique_ptr<Util> m_util;
    std::unique_ptr<GodotHandler> m_godotHandler;

    std::shared_ptr<GlobalVariables> m_globalVariables;
    std::shared_ptr<Settings> m_settings;
    std::shared_ptr<InstalledListModel> m_installedListModel;
    std::shared_ptr<MonitorListModel> m_monitorListModel;
    std::shared_ptr<ProfileListModel> m_profileListModel;
    std::shared_ptr<InstalledListFilter> m_installedListFilter;
    std::shared_ptr<UiAppStateSignals> m_uiAppStateSignals;
    std::shared_ptr<ErrorManager> m_errorManager;
};
}
