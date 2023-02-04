// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#include "sysinfo.h"

/*!
    \module ScreenPlaySysInfo
    \title ScreenPlaySysInfo
    \brief Module for ScreenPlaySysInfo.
*/

/*!
    \class SysInfo
    \inmodule ScreenPlaySysInfo
    \brief  .

*/

/*!
 * \brief SysInfo::SysInfo
 */
SysInfo::SysInfo(QQuickItem* parent)
    : QQuickItem(parent)
    , m_ram(std::make_unique<RAM>())
    , m_cpu(std::make_unique<CPU>())
    , m_gpu(std::make_unique<GPU>())
    , m_storage(std::make_unique<Storage>())
    , m_uptime(std::make_unique<Uptime>())
{
}

#include "moc_sysinfo.cpp"
