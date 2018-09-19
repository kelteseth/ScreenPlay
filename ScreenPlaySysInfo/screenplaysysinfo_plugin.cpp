#include "screenplaysysinfo_plugin.h"
#include "sysinfo.h"

#include <qqml.h>

void ScreenPlaySysInfoPlugin::registerTypes(const char *uri)
{
    // @uri net.aimber.sysinfo
    qmlRegisterType<SysInfo>(uri, 1, 0, "SysInfo");
}

