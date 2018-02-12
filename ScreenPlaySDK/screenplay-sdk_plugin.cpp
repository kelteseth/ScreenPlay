#include "screenplay-sdk_plugin.h"
#include "screenplaysdk.h"

#include <qqml.h>

void ScreenPlay_SDKPlugin::registerTypes(const char *uri)
{
    // @uri net.aimber.screenplaysdk
    qmlRegisterType<ScreenPlaySDK>(uri, 1, 0, "ScreenPlaySDK");
}

