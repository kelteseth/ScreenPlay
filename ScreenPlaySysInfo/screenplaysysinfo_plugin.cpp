#include "screenplaysysinfo_plugin.h"
#include "sysinfo.h"
#include <qmetatype.h>
#include <qqml.h>

QObject* ScreenPlaySysInfoSingleton(QQmlEngine* engine, QJSEngine* scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    return new SysInfo();
}

void ScreenPlaySysInfoPlugin::registerTypes(const char* uri)
{
    // @uri net.aimber.sysinfo
    qmlRegisterSingletonType<SysInfo>(uri, 1, 0, "SysInfo", ScreenPlaySysInfoSingleton);

}
