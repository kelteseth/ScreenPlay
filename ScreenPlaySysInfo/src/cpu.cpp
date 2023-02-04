// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#include "cpu.h"
#include "mathhelper.h"
#include <QtQml/qqml.h>

#define STATUS_SUCCESS 0
#define STATUS_INFO_LENGTH_MISMATCH 0xC0000004

/*!
    \class CPU
    \inmodule ScreenPlaySysInfo
    \brief  .

*/

CPU::CPU(QObject* parent)
    : QObject(parent)
{
    connect(&m_updateTimer, &QTimer::timeout, this, &CPU::update);
    m_updateTimer.start(m_tickRate);
}

/*!
 * \brief CPU::update
 */
void CPU::update()
{
    BOOL status;
    FILETIME ftIdleTime, ftKernelTime, ftUserTime;

    // get new CPU's idle/kernel/user time
    status = GetSystemTimes(&ftIdleTime, &ftKernelTime, &ftUserTime);
    if (status == 0)
        return;

    // convert times to uint by appending low and high bits
    uint64_t newIdleTime = FileTimeToInt64(ftIdleTime);
    uint64_t newKernelTime = FileTimeToInt64(ftKernelTime);
    uint64_t newUserTime = FileTimeToInt64(ftUserTime);

    // subtract old times
    uint64_t userTime = newUserTime - lastUserTime;
    uint64_t kernelTime = newKernelTime - lastKernelTime;
    double idleTime = newIdleTime - lastIdleTime;

    // calculate the usage
    double sysTime = kernelTime + userTime;
    float currentUsage = float((sysTime - idleTime) * 100 / sysTime);

    // save the new values
    lastIdleTime = newIdleTime;
    lastKernelTime = newKernelTime;
    lastUserTime = newUserTime;

    float cu = int(currentUsage * 100);
    currentUsage = float(cu / 100);

    // publish result
    setUsage(currentUsage);
}

#include "moc_cpu.cpp"
