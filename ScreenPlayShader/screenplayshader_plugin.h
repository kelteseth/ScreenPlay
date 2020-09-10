#pragma once
#include <QQmlExtensionPlugin>

#include "shaderlibrary.h"

class ScreenPlayShaderPlugin : public QQmlExtensionPlugin {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QQmlExtensionInterface_iid)

public:
    void registerTypes(const char* uri) override;

private:
    ShaderLibrary m_shaderLibrary;
};
