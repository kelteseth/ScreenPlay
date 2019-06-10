#include "screenplay-sdk_plugin.h"
#include "screenplaysdk.h"

#include <qqml.h>

void ScreenPlay_SDKPlugin::registerTypes(const char *uri)
{
    // @uri ScreenPlay.screenplaysdk
    qmlRegisterType<ScreenPlaySDK>(uri, 1, 0, "ScreenPlaySDK");
}

