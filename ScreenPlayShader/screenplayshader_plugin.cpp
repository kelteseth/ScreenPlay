#include "screenplayshader_plugin.h"

#include <qqml.h>

QObject* ScreenPlayShaderLibrarySingleton(QQmlEngine* engine, QJSEngine* scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    return new ShaderLibrary();
}

void ScreenPlayShaderPlugin::registerTypes(const char* uri)
{
    Q_UNUSED(uri)

    qmlRegisterSingletonType<ShaderLibrary>(uri, 1, 0, "ShaderLibrary", ScreenPlayShaderLibrarySingleton);
}
