#pragma once

#include <QDir>
#include <QGuiApplication>
#include <QIcon>
#include <QObject>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQmlEngine>
#include <QStringList>
#include <QUrl>
#include <QtWebEngine>

#include <memory>

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

using std::make_shared,
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

    Q_PROPERTY(shared_ptr<GlobalVariables> globalVariables READ globalVariables WRITE setGlobalVariables NOTIFY globalVariablesChanged)
public:
    explicit App(int &argc, char **argv);

    shared_ptr<GlobalVariables> globalVariables() const
    {
        return m_globalVariables;
    }

signals:

    void globalVariablesChanged(shared_ptr<GlobalVariables> globalVariables);

public slots:
int run();
void setGlobalVariables(shared_ptr<GlobalVariables> globalVariables)
{
    if (m_globalVariables == globalVariables)
        return;

    m_globalVariables = globalVariables;
    emit globalVariablesChanged(m_globalVariables);
}

private:
    std::unique_ptr<QGuiApplication> app;
    std::unique_ptr<QQmlApplicationEngine> mainWindowEngine;
    shared_ptr<GlobalVariables> m_globalVariables;
};
