#include "shaderlibrary.h"

#include <QFile>

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

/*!
 * \brief ShaderLibrary::ShaderLibrary
 * \param parent
 */
ShaderLibrary::ShaderLibrary(QQuickItem* parent)
    : QQuickItem(parent)
{
    QFile lightningFragFile("qrc:/ScreenPlaySysInfo/src/lightning.frag");
    lightningFragFile.open(QIODevice::ReadOnly);
    QFile lightningVertFile("qrc:/ScreenPlaySysInfo/src/lightning.vert");
    lightningVertFile.open(QIODevice::ReadOnly);
    m_lightning = std::make_unique<Shader>(lightningVertFile.readAll(), lightningFragFile.readAll());

    QFile waterFragFile("qrc:/ScreenPlaySysInfo/src/water.frag");
    waterFragFile.open(QIODevice::ReadOnly);
    QFile waterVertFile("qrc:/ScreenPlaySysInfo/src/water.vert");
    waterVertFile.open(QIODevice::ReadOnly);
    m_water = std::make_unique<Shader>(waterVertFile.readAll(), waterFragFile.readAll());
}

ShaderLibrary::~ShaderLibrary()
{
}
