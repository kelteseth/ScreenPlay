// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
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
    QFile lightningFragFile(":/shader/lightning.frag");
    lightningFragFile.open(QIODevice::ReadOnly);
    QFile lightningVertFile(":/shader/lightning.vert");
    lightningVertFile.open(QIODevice::ReadOnly);
    m_lightning = std::make_unique<Shader>(lightningVertFile.readAll(), lightningFragFile.readAll());

    QFile waterFragFile(":/shader/water.frag");
    waterFragFile.open(QIODevice::ReadOnly);
    QFile waterVertFile(":/shader/water.vert");
    waterVertFile.open(QIODevice::ReadOnly);
    m_water = std::make_unique<Shader>(waterVertFile.readAll(), waterFragFile.readAll());
}

ShaderLibrary::~ShaderLibrary()
{
}
