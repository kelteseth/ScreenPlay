#include "screenplayshader_plugin.h"

#include <QQmlEngine>
#include <QUrl>
#include <qqml.h>

QObject* ScreenPlayShaderLibrarySingleton(QQmlEngine* engine, QJSEngine* scriptEngine)
{
    Q_UNUSED(scriptEngine)
    return new ShaderLibrary();
}

void ScreenPlayShaderPlugin::registerTypes(const char* uri)
{

    qmlRegisterType(QUrl("qrc:/ShaderWrapper/ShadertoyShader.qml"), "ScreenPlay.ShadertoyShader", 1,0, "ShadertoyShader");
    qmlRegisterSingletonType<ShaderLibrary>(uri, 1, 0, "ShaderLibrary", ScreenPlayShaderLibrarySingleton);
}
