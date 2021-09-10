#pragma once

#include <QQmlContext>
#include <QQmlEngine>
#include <QQmlExtensionPlugin>
#include <QSettings>
#include <QtQml>

#include "steam/steam_qt_enums_generated.h"
#include "workshop.h"

#include "steamqmlimageprovider.h"

class ScreenPlayWorkshopPlugin : public QQmlExtensionPlugin {
    Q_OBJECT

    Q_PLUGIN_METADATA(IID QQmlExtensionInterface_iid)

public:
    void registerTypes(const char* uri) override;
};
