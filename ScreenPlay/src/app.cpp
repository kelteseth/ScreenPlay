// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#include "ScreenPlay/app.h"

#include <QDir>
#include <QElapsedTimer>
#include <QGuiApplication>
#include <QIcon>
#include <QObject>
#include <QProcessEnvironment>
#include <QQuickStyle>
#include <QStringList>
#include <QUrl>
#include <QVersionNumber>
#include <QtGlobal>
#include <QtSvg>

#if defined(Q_OS_MACOS)
#include "ScreenPlayUtil/macutils.h"
#endif

namespace ScreenPlay {

/*!
    \module ScreenPlay
    \title ScreenPlay
    \brief Module for ScreenPlay.
*/
/*!
    \namespace ScreenPlay
    \inmodule ScreenPlay
    \brief Namespace for ScreenPlay.
*/

/*!
    \class ScreenPlay::App
    \inmodule ScreenPlay
    \brief The App class contains all members for ScreenPlay.
*/

/*!
    \brief Used for initialization after the constructor. The sole purpose is to check if
    another ScreenPlay instance is running and then quit early. This is also because we cannot
    call QGuiApplication::quit(); in the SDKConnector before the app.exec(); ( the Qt main event
    loop ) has started.
*/
void App::init()
{
    qInfo() << "Init ScreenPlay";

    QString fontsPath = QGuiApplication::instance()->applicationDirPath() + "/assets/fonts/";
#if defined(Q_OS_MACOS)
    fontsPath = QGuiApplication::instance()->applicationDirPath() + "/../Resources/fonts/";
#endif
    const QDir fontsDir(fontsPath);
    if (!fontsDir.isEmpty() && fontsDir.exists()) {
        QDirIterator it(fontsPath, { "*.ttf", "*.otf" }, QDir::Files);
        while (it.hasNext()) {
            QFontDatabase::addApplicationFont(it.next());
        }
    } else {
        qWarning() << "Unable to load font from: " << fontsPath;
    }

    using std::make_shared, std::make_unique;

    m_screenPlayManager = make_unique<ScreenPlayManager>();
    m_globalVariables = make_shared<GlobalVariables>();
    m_monitorListModel = make_shared<MonitorListModel>();
    m_util = make_unique<Util>();
    m_profileListModel = make_shared<ProfileListModel>(m_globalVariables);
    m_settings = make_shared<Settings>(m_globalVariables);
    m_installedListModel = make_shared<InstalledListModel>(m_globalVariables, m_settings);
    m_installedListFilter = make_shared<InstalledListFilter>(m_installedListModel);

    // Only create anonymousTelemetry if user did not disallow!
    if (m_settings->anonymousTelemetry()) {
#if defined(Q_OS_WIN)

        sentry_options_t* options = sentry_options_new();
        sentry_options_set_dsn(options, "https://425ea0b77def4f91a5a9decc01b36ff4@o428218.ingest.sentry.io/5373419");
        QString environment = QGuiApplication::applicationVersion() + "";
        sentry_options_set_environment(options, QString(environment).toStdString().c_str());

        const QString appPath = QGuiApplication::applicationDirPath();
        sentry_options_set_handler_path(options, QString(appPath + "/crashpad_handler.exe").toStdString().c_str());
        sentry_options_set_database_path(options, appPath.toStdString().c_str());
        sentry_options_set_handler_path(options, QString(QGuiApplication::applicationDirPath() + "/crashpad_handler" + Util().executableBinEnding()).toStdString().c_str());
        sentry_options_set_database_path(options, QGuiApplication::applicationDirPath().toStdString().c_str());
        const int sentryInitStatus = sentry_init(options);
        if (sentryInitStatus != 0) {
            qWarning() << "Unable to inti sentry crashhandler with statuscode: " << sentryInitStatus;
        }
#endif
    }

    m_create = make_unique<Create>(m_globalVariables);
    QObject::connect(m_create.get(), &Create::finished, m_installedListModel.get(), &InstalledListModel::reset);

    m_wizards = make_unique<Wizards>(m_globalVariables);

    // When the installed storage path changed
    QObject::connect(m_settings.get(), &Settings::resetInstalledListmodel, m_installedListModel.get(), &InstalledListModel::reset);
    QObject::connect(m_settings.get(), &Settings::requestRetranslation, this, &App::requestRetranslation);
    m_settings->setupLanguage();

    QObject::connect(m_globalVariables.get(), &GlobalVariables::localStoragePathChanged, this, [this](QUrl localStoragePath) {
        emit m_settings->resetInstalledListmodel();
        m_settings->setqSetting("ScreenPlayContentPath", localStoragePath.toString());
    });

    auto* guiAppInst = dynamic_cast<QGuiApplication*>(QGuiApplication::instance());

    // Set visible if the -silent parameter was not set
    if (guiAppInst->arguments().contains("-silent")) {
        qInfo() << "Starting in silent mode.";
        settings()->setSilentStart(true);
    }

    // Must be called last to display a error message on startup by the qml engine
    m_screenPlayManager->init(m_globalVariables, m_monitorListModel, m_settings);

    // TODO
    // QObject::connect(
    //     m_monitorListModel.get(),
    //     &MonitorListModel::monitorConfigurationChanged,
    //     m_screenPlayManager.get(), [this]() {
    //         m_screenPlayManager->removeAllWallpapers(true);
    //     });
}

QString App::version() const
{
    return QGuiApplication::applicationVersion();
}

/*!
    \brief Calls QGuiApplication quit() and can be used to do additional
           tasks before exiting.
*/
void App::exit()
{
    m_screenPlayManager->removeAllWallpapers(false);
    m_screenPlayManager->removeAllWidgets(false);
    // Must be called inside a separate event loop otherwise we
    // would kill the qml engine while it is calling this function.
    // A single shot timer is a handy woraround for this.
    QTimer::singleShot(0, this, [this]() {
        emit requestExit();
    });
}

void App::showDockIcon(const bool show)
{
#if defined(Q_OS_MACOS)
    MacUtils::instance()->showDockIcon(show);
#endif
}

/*!
    \property App::globalVariables
    \brief .

   .
*/
void App::setGlobalVariables(GlobalVariables* globalVariables)
{
    if (m_globalVariables.get() == globalVariables)
        return;

    m_globalVariables.reset(globalVariables);
    emit globalVariablesChanged(m_globalVariables.get());
}

/*!
    \property App::screenPlayManager
    \brief Sets the screen play manager.
*/
void App::setScreenPlayManager(ScreenPlayManager* screenPlayManager)
{
    if (m_screenPlayManager.get() == screenPlayManager)
        return;

    m_screenPlayManager.reset(screenPlayManager);
    emit screenPlayManagerChanged(m_screenPlayManager.get());
}
/*!
    \property App::create
    \brief .

   .
*/
void App::setCreate(Create* create)
{
    if (m_create.get() == create)
        return;

    m_create.reset(create);
    emit createChanged(m_create.get());
}
/*!
    \property App::util
    \brief .

   .
*/
void App::setUtil(Util* util)
{
    if (m_util.get() == util)
        return;

    m_util.reset(util);
    emit utilChanged(m_util.get());
}
/*!
    \property App::settings
    \brief .

   .
*/
void App::setSettings(Settings* settings)
{
    if (m_settings.get() == settings)
        return;

    m_settings.reset(settings);
    emit settingsChanged(m_settings.get());
}
/*!
    \property App::installedListModel
    \brief .

   .
*/
void App::setInstalledListModel(InstalledListModel* installedListModel)
{
    if (m_installedListModel.get() == installedListModel)
        return;

    m_installedListModel.reset(installedListModel);
    emit installedListModelChanged(m_installedListModel.get());
}
/*!
    \property App::monitorListModel
    \brief .

   .
*/
void App::setMonitorListModel(MonitorListModel* monitorListModel)
{
    if (m_monitorListModel.get() == monitorListModel)
        return;

    m_monitorListModel.reset(monitorListModel);
    emit monitorListModelChanged(m_monitorListModel.get());
}
/*!
    \property App::profileListModel
    \brief .

   .
*/
void App::setProfileListModel(ProfileListModel* profileListModel)
{
    if (m_profileListModel.get() == profileListModel)
        return;

    m_profileListModel.reset(profileListModel);
    emit profileListModelChanged(m_profileListModel.get());
}
/*!
    \property App::installedListFilter
    \brief .

   .
*/
void App::setInstalledListFilter(InstalledListFilter* installedListFilter)
{
    if (m_installedListFilter.get() == installedListFilter)
        return;

    m_installedListFilter.reset(installedListFilter);
    emit installedListFilterChanged(m_installedListFilter.get());
}
/*!
    \property App::wizards
    \brief .

   .
*/
void App::setWizards(Wizards* wizards)
{
    if (m_wizards.get() == wizards)
        return;

    m_wizards.reset(wizards);
    emit wizardsChanged(m_wizards.get());
}

QQmlEngine* App::engine() const
{
    return m_engine;
}

void App::setEngine(QQmlEngine* engine)
{
    m_engine = engine;
    if (!m_engine) {
        qFatal("QQmlEngine not set");
    }
    QObject::connect(
        m_settings.get(),
        &Settings::requestRetranslation,
        m_engine,
        &QQmlEngine::retranslate);
}
}

#include "moc_app.cpp"
