#include "shaderlibrary.h"

/*!
    \module ScreenPlayShader
    \title ScreenPlayShader
    \brief Module for ScreenPlayShader.
*/

/*!
    \class ShaderLibrary
    \inmodule ScreenPlayShader
    \brief  .
*/

ShaderLibrary::ShaderLibrary(QQuickItem* parent)
    : QQuickItem(parent)
{
    QFile lightningFragFile(":/lightning.frag");
    lightningFragFile.open(QIODevice::ReadOnly);
    QFile lightningVertFile(":/lightning.vert");
    lightningVertFile.open(QIODevice::ReadOnly);
    m_lightning = std::make_unique<Shader>(lightningVertFile.readAll(), lightningFragFile.readAll());

    QFile waterFragFile(":/water.frag");
    waterFragFile.open(QIODevice::ReadOnly);
    QFile waterVertFile(":/water.vert");
    waterVertFile.open(QIODevice::ReadOnly);
    m_water = std::make_unique<Shader>(waterVertFile.readAll(), waterFragFile.readAll());
}

ShaderLibrary::~ShaderLibrary()
{
}
