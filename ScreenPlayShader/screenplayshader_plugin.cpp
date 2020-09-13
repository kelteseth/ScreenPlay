#include "screenplayshader_plugin.h"

#include <qqml.h>
#include <QQmlEngine>

QObject* ScreenPlayShaderLibrarySingleton(QQmlEngine* engine, QJSEngine* scriptEngine)
{
    Q_UNUSED(scriptEngine)
    //Add QRC
   // engine->addImportPath("qrc:/ShaderWrapper/");
  // engine->addImportPath("..");
    return new ShaderLibrary();
}

void ScreenPlayShaderPlugin::registerTypes(const char* uri)
{

    qmlRegisterSingletonType<ShaderLibrary>(uri, 1, 0, "ShaderLibrary", ScreenPlayShaderLibrarySingleton);
}
