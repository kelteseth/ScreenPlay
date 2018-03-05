#include "startuperror.h"

StartupError::StartupError(QQmlApplicationEngine* main, QQmlApplicationEngine* err, QObject *parent) : QObject(parent)
{
    m_errorWindow = err;
    m_mainWindow = main;
}

void StartupError::startScreenPlayAnyway()
{
    m_mainWindow->load(QUrl(QStringLiteral("qrc:/main.qml")));

}
