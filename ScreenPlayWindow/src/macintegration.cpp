#include "macintegration.h"
#include "macbridge.h"

MacIntegration::MacIntegration(QObject* parent)
    : QObject(parent)
{
    MacBridge::instance();
}

void MacIntegration::SetBackgroundLevel(QWindow* window)
{
    MacBridge::SetBackgroundLevel(window);
}
