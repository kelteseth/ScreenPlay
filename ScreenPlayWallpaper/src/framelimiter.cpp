// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#include "framelimiter.h"

#include <QEvent>
#include <QWindow>

namespace ScreenPlay {

FrameRateLimiter::FrameRateLimiter(QWindow* window, QObject* parent)
    : QObject(parent)
    , m_window(window)
{
    m_clock.start();
    m_deliverTimer.setSingleShot(true);
    m_deliverTimer.setTimerType(Qt::PreciseTimer);
    connect(&m_deliverTimer, &QTimer::timeout, this, [this] {
        m_window->requestUpdate();
    });
    m_window->installEventFilter(this);
}

void FrameRateLimiter::setMaxFps(int fps)
{
    m_maxFps = qMax(0, fps);
    if (m_maxFps == 0) {
        m_deliverTimer.stop();
        m_window->requestUpdate();
    }
}

bool FrameRateLimiter::eventFilter(QObject* watched, QEvent* event)
{
    if (m_maxFps <= 0 || watched != m_window || event->type() != QEvent::UpdateRequest)
        return QObject::eventFilter(watched, event);

    const qint64 intervalNs = 1'000'000'000ll / m_maxFps;
    const qint64 now = m_clock.nsecsElapsed();
    if (now >= m_nextDueNs) {
        // The due times form an absolute grid so delivery latency does not
        // accumulate; resync after idle phases instead of bursting.
        m_nextDueNs = (now - m_nextDueNs > intervalNs) ? now + intervalNs : m_nextDueNs + intervalNs;
        return false;
    }
    if (!m_deliverTimer.isActive())
        m_deliverTimer.start(int((m_nextDueNs - now) / 1'000'000) + 1);
    return true;
}
}
