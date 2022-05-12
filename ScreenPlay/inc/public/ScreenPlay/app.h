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
    void globalVariablesChanged(GlobalVariables* globalVariables);
    void screenPlayManagerChanged(ScreenPlayManager* screenPlayManager);
    void createChanged(Create* create);
    void utilChanged(Util* util);
    void settingsChanged(Settings* settings);
    void installedListModelChanged(InstalledListModel* installedListModel);
    void monitorListModelChanged(MonitorListModel* monitorListModel);
    void profileListModelChanged(ProfileListModel* profileListModel);
    void installedListFilterChanged(InstalledListFilter* installedListFilter);
    void mainWindowEngineChanged(QQmlApplicationEngine* mainWindowEngine);
    void wizardsChanged(Wizards* wizards);

public slots:
    QString version() const;
    void exit();

    QPointF cursorPos() { return QCursor::pos(); }

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

    void setMainWindowEngine(QQmlApplicationEngine* mainWindowEngine)
    {
        if (m_mainWindowEngine.get() == mainWindowEngine)
            return;

        m_mainWindowEngine.reset(mainWindowEngine);
        emit mainWindowEngineChanged(m_mainWindowEngine.get());
    }

    void setWizards(Wizards* wizards)
    {
        if (m_wizards.get() == wizards)
            return;

        m_wizards.reset(wizards);
        emit wizardsChanged(m_wizards.get());
    }

private:
    bool setupKDE();
    bool isKDEInstalled();
    void installKDEWallpaper();
    void upgradeKDEWallpaper();
    void restartKDE();
    std::optional<bool> isNewestKDEWallpaperInstalled();

private:
    QNetworkAccessManager m_networkAccessManager;
    QElapsedTimer m_continuousIntegrationMetricsTimer;
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

    QString m_kdeWallpaperPath;
    QString m_appKdeWallapperPath;
};
}
