// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#include "shaderlibrary.h"

#include <QFile>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(shaderLibrary, "screenplay.shader.library")

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
    if (!lightningFragFile.open(QIODevice::ReadOnly)) {
        qCWarning(shaderLibrary) << "Could not open lightning.frag";
        return;
    }
    QFile lightningVertFile(":/shader/lightning.vert");
    if (!lightningVertFile.open(QIODevice::ReadOnly)) {
        qCWarning(shaderLibrary) << "Could not open lightning.vert";
        return;
    }
    m_lightning = std::make_unique<Shader>(lightningVertFile.readAll(), lightningFragFile.readAll());

    QFile waterFragFile(":/shader/water.frag");
    if (!waterFragFile.open(QIODevice::ReadOnly)) {
        qCWarning(shaderLibrary) << "Could not open water.frag";
        return;
    }
    QFile waterVertFile(":/shader/water.vert");
    if (!waterVertFile.open(QIODevice::ReadOnly)) {
        qCWarning(shaderLibrary) << "Could not open water.vert";
        return;
    }
    m_water = std::make_unique<Shader>(waterVertFile.readAll(), waterFragFile.readAll());
}

ShaderLibrary::~ShaderLibrary()
{
}

#include "moc_shaderlibrary.cpp"
