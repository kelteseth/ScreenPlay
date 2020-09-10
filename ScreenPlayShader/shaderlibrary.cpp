#include "shaderlibrary.h"

ShaderLibrary::ShaderLibrary(QQuickItem* parent)
    : QQuickItem(parent)
{
    QFile lightningFragFile(":/lightning.frag");
    lightningFragFile.open(QIODevice::ReadOnly);
    QFile lightningVertFile(":/lightning.vert");
    lightningVertFile.open(QIODevice::ReadOnly);
    m_lightning = std::make_unique<Shader>(lightningVertFile.readAll(),lightningFragFile.readAll());
}

ShaderLibrary::~ShaderLibrary()
{
}
