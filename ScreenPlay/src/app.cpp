// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#include "ScreenPlay/app.h"
#include "ScreenPlay/errormanager.h"
#include "ScreenPlayCore/godothandler.h"

#include <QDir>
#include <QElapsedTimer>
#include <QGuiApplication>
#include <QIcon>
#include <QLoggingCategory>
#include <QObject>
#include <QProcessEnvironment>
#include <QQuickStyle>
#include <QStringList>
#include <QUrl>
#include <QVersionNumber>
#include <QtGlobal>
#include <QtSvg>

Q_LOGGING_CATEGORY(app, "screenplay.app")

#if defined(Q_OS_MACOS)
#include "ScreenPlayCore/macutils.h"
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
    \brief Constructs the App and all owned sub-systems. Engine-dependent
    wiring (e.g. retranslation) is deferred to attachEngine() so the QML
    engine is available before any QML-side side effects run.
*/
App::App(QObject* parent)
    : QObject(parent)
{
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
        qCWarning(app) << "Unable to load font from: " << fontsPath;
    }
    using std::make_shared, std::make_unique;

    m_errorManager = make_shared<ErrorManager>();
    m_uiAppStateSignals = make_unique<UiAppStateSignals>();
    m_frameStats = make_unique<FrameStats>();
    m_screenPlayManager = make_unique<ScreenPlayManager>();
    m_globalVariables = make_shared<GlobalVariables>();
    m_monitorListModel = make_shared<MonitorListModel>();
    m_util = make_unique<Util>();
    m_godotHandler = make_unique<GodotHandler>();
    m_profileListModel = make_shared<ProfileListModel>(m_globalVariables);
    m_settings = make_shared<Settings>(m_globalVariables);
    m_installedListModel = make_shared<InstalledListModel>(m_globalVariables, m_settings);
    m_installedListFilter = make_shared<InstalledListFilter>(m_installedListModel);

    // Only create anonymousTelemetry if user did not disallow!
    if (m_settings->anonymousTelemetry()) {
#if defined(Q_OS_WIN)

        sentry_options_t* options = sentry_options_new();
        sentry_options_set_dsn(options, "https://e9481f935713b95a81c0c0a86806afe9@o428218.ingest.us.sentry.io/4510085149818880");
        QString environment = QGuiApplication::applicationVersion() + "";
        sentry_options_set_environment(options, QString(environment).toStdString().c_str());

        sentry_options_set_handler_path(options, QString(QGuiApplication::applicationDirPath() + "/crashpad_handler" + Util().executableBinEnding()).toStdString().c_str());
        sentry_options_set_database_path(options, QGuiApplication::applicationDirPath().toStdString().c_str());
        const int sentryInitStatus = sentry_init(options);
        if (sentryInitStatus != 0) {
            qCWarning(app) << "Unable to inti sentry crashhandler with statuscode: " << sentryInitStatus;
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
        qCInfo(app) << "Starting in silent mode.";
        settings()->setSilentStart(true);
    }

    // Must be called last to display an error message on startup by the qml engine
    m_screenPlayManager->init(m_globalVariables, m_monitorListModel, m_settings, m_errorManager);
}

App::~App()
{
#if defined(Q_OS_WIN)
    if (settings()->anonymousTelemetry()) {
        sentry_close();
    }
#endif
}

/*!
    \brief Qt 6 declarative singleton factory. The QML engine calls this on
    first access of \c App from QML and takes ownership of the returned
    instance. Wires the engine reference and any engine-dependent signals.
*/
App* App::create(QQmlEngine* engine, QJSEngine* /*jsEngine*/)
{
    auto* instance = new App;
    instance->attachEngine(engine);
    return instance;
}

void App::attachEngine(QQmlEngine* engine)
{
    m_engine = engine;

    if (m_errorManager) {
        m_errorManager->setQmlReady(true);
    }

    if (auto* appEngine = qobject_cast<QQmlApplicationEngine*>(engine)) {
        QObject::connect(
            m_settings.get(),
            &Settings::requestRetranslation,
            appEngine,
            &QQmlApplicationEngine::retranslate);
    }
}

QString App::version() const
{
    return QGuiApplication::applicationVersion();
}

/*!
    \brief Calls QGuiApplication quit() and can be used to do additional
           tasks before exiting.
*/
QCoro::QmlTask App::exit()
{
    return QCoro::QmlTask(exitTask());
}

QCoro::Task<void> App::exitTask()
{
    const Result result = co_await m_screenPlayManager->shutdown();
    if (!result.success())
        qCritical() << "Shutdown reported failure, exiting anyway:" << result.message();
    emit requestExit();
}

void App::showDockIcon(const bool show)
{
#if defined(Q_OS_MACOS)
    MacUtils::instance()->showDockIcon(show);
#endif
}

}

#include "moc_app.cpp"
