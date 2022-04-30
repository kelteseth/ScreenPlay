#include "uptime.h"

#ifdef Q_OS_WINDOWS
#include <Windows.h>
#include <sysinfoapi.h>
#endif

/*!
    \class Uptime
    \inmodule ScreenPlaySysInfo
    \brief  .
*/
Uptime::Uptime(QObject* parent)
    : QObject(parent)
{
    QObject::connect(&m_updateTimer, &QTimer::timeout, this, &Uptime::update);
    m_updateTimer.start(m_tickRate);
}

/*!
 * \brief Uptime::update
 */
void Uptime::update()
{
#ifdef Q_OS_WINDOWS
    auto ticks = GetTickCount64();
    ticks /= 1000;
    auto seconds = ticks % 60;
    ticks /= 60;
    auto minutes = ticks % 60;
    ticks /= 60;
    auto hours = ticks;
    ticks /= 24;
    auto days = ticks;
    ticks /= 365;
    auto years = ticks;

    setSeconds(seconds);
    setMinutes(minutes);
    setHours(hours);
    setDays(days);
    setYears(years);
#endif
}
