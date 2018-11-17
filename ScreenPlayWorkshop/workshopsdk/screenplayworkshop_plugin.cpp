#include "screenplayworkshop_plugin.h"
#include "workshop.h"

#include <qqml.h>

void ScreenPlayWorkshopPlugin::registerTypes(const char *uri)
{
    // @uri net.aimber.workshop
    qmlRegisterType<Workshop>(uri, 1, 0, "Workshop");
}

