// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#include "ScreenPlayCore/framestats.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QSGRendererInterface>
#include <QScreen>
#include <algorithm>
#include <cmath>

namespace ScreenPlay {

FrameStats::FrameStats(QObject* parent)
    : QObject(parent)
{
    m_clock.start();
    m_deltasNs.reserve(maxSamples);
    m_refreshTimer.setInterval(250);
    connect(&m_refreshTimer, &QTimer::timeout, this, &FrameStats::refresh);
}

void FrameStats::attach(QQuickWindow* window)
{
    if (!window || m_window == window)
        return;
    if (m_window)
        disconnect(m_window, &QQuickWindow::frameSwapped, this, nullptr);
    m_window = window;
    if (QScreen* s = window->screen(); s && s->refreshRate() > 1)
        m_expectedMs = 1000.0 / s->refreshRate();
    // Direct connection: fires on the render thread immediately after present.
    connect(window, &QQuickWindow::frameSwapped, this, &FrameStats::onFrameSwapped,
        Qt::DirectConnection);
}

void FrameStats::setEnabled(bool enabled)
{
    if (m_enabled.load(std::memory_order_relaxed) == enabled)
        return;
    m_enabled.store(enabled, std::memory_order_relaxed);
    if (enabled) {
        resetStats();
        m_refreshTimer.start();
        // Kick off the continuous render loop (see onFrameSwapped).
        if (m_window)
            m_window->update();
    } else {
        m_refreshTimer.stop();
    }
    emit enabledChanged(enabled);
}

void FrameStats::onFrameSwapped()
{
    if (!m_enabled.load(std::memory_order_relaxed))
        return;
    const qint64 now = m_clock.nsecsElapsed();
    {
        QMutexLocker lock(&m_mutex);
        if (m_lastSwapNs >= 0) {
            m_deltasNs.append(now - m_lastSwapNs);
            if (m_deltasNs.size() > maxSamples)
                m_deltasNs.remove(0, maxSamples / 2);
        }
        m_lastSwapNs = now;
    }
    // Keep the window presenting continuously while measuring - an idle
    // window renders no frames, so there would be nothing to time. Vsync
    // paces the loop to the refresh rate. QQuickWindow::update() is
    // documented callable from any thread.
    m_window->update();
}

void FrameStats::recordAnimSample(double x)
{
    // Called from QML on afterAnimating (GUI thread). Track per-frame movement
    // of a constant-velocity item; negative jumps are animation wrap-around.
    if (m_lastAnimX >= 0) {
        const double step = x - m_lastAnimX;
        if (step > 0 && step < 200)
            m_animSteps.append(step);
        if (m_animSteps.size() > maxSamples)
            m_animSteps.remove(0, maxSamples / 2);
    }
    m_lastAnimX = x;
}

void FrameStats::resetStats()
{
    QMutexLocker lock(&m_mutex);
    m_deltasNs.clear();
    m_lastSwapNs = -1;
    m_animSteps.clear();
    m_lastAnimX = -1;
    m_stutterCount = 0;
}

QVariantList FrameStats::recentDeltas(int n) const
{
    QMutexLocker lock(&m_mutex);
    QVariantList out;
    const int start = qMax(0, int(m_deltasNs.size()) - n);
    for (int i = start; i < m_deltasNs.size(); ++i)
        out.append(m_deltasNs[i] / 1e6);
    return out;
}

static double percentile(const QVector<qint64>& sorted, double p)
{
    if (sorted.isEmpty())
        return 0;
    const int idx = qMin(int(sorted.size()) - 1, int(std::ceil(p * sorted.size())) - 1);
    return sorted[qMax(0, idx)] / 1e6;
}

void FrameStats::refresh()
{
    QVector<qint64> deltas;
    {
        QMutexLocker lock(&m_mutex);
        deltas = m_deltasNs;
    }

    // Displayed statistics cover only the last few seconds - a long-lived
    // buffer made fps/mean take minutes to recover after a degraded phase.
    // The full buffer stays available via recentDeltas()/lastDeltasMs.
    constexpr qint64 statsWindowNs = 5ll * 1000 * 1000 * 1000;
    qint64 windowSum = 0;
    int firstIndex = deltas.size();
    while (firstIndex > 0 && windowSum < statsWindowNs)
        windowSum += deltas[--firstIndex];
    if (firstIndex > 0)
        deltas.remove(0, firstIndex);

    if (deltas.size() < 4) {
        emit statsChanged();
        return;
    }

    double sum = 0;
    for (const qint64 d : deltas)
        sum += d;
    const double meanNs = sum / deltas.size();
    double var = 0;
    int stutters = 0;
    const double stutterThresholdNs = m_expectedMs * 1e6 * 1.5;
    for (const qint64 d : deltas) {
        var += (d - meanNs) * (d - meanNs);
        if (d > stutterThresholdNs)
            ++stutters;
    }
    var /= deltas.size();

    QVector<qint64> sorted = deltas;
    std::sort(sorted.begin(), sorted.end());

    m_meanMs = meanNs / 1e6;
    m_fps = meanNs > 0 ? 1e9 / meanNs : 0;
    m_stddevMs = std::sqrt(var) / 1e6;
    m_p99Ms = percentile(sorted, 0.99);
    m_worstMs = sorted.last() / 1e6;
    m_stutterCount = stutters;

    if (m_animSteps.size() > 4) {
        double s = 0;
        for (const double v : m_animSteps)
            s += v;
        const double m = s / m_animSteps.size();
        double v2 = 0;
        for (const double v : m_animSteps)
            v2 += (v - m) * (v - m);
        m_animStepStddevPx = std::sqrt(v2 / m_animSteps.size());
    }

    emit statsChanged();
}

QString FrameStats::envSummary() const
{
    static const char* vars[] = {
        "QSG_RHI_BACKEND",
        "QSG_RENDER_LOOP",
        "QT_D3D_MAX_FRAME_LATENCY",
        "QT_D3D_NO_VBLANK_THREAD",
        "QSG_USE_SIMPLE_ANIMATION_DRIVER",
        "QSG_FIXED_ANIMATION_STEP",
        "QT_D3D_NO_FLIP",
    };
    QStringList parts;
    for (const char* v : vars) {
        if (qEnvironmentVariableIsSet(v))
            parts << QString("%1=%2").arg(v, qEnvironmentVariable(v));
    }
    return parts.isEmpty() ? QStringLiteral("defaults") : parts.join(QStringLiteral("  "));
}

QString FrameStats::apiName() const
{
    if (!m_window || !m_window->rendererInterface())
        return QStringLiteral("unknown");
    switch (m_window->rendererInterface()->graphicsApi()) {
    case QSGRendererInterface::Direct3D11:
        return QStringLiteral("d3d11");
    case QSGRendererInterface::Direct3D12:
        return QStringLiteral("d3d12");
    case QSGRendererInterface::Vulkan:
        return QStringLiteral("vulkan");
    case QSGRendererInterface::OpenGL:
        return QStringLiteral("opengl");
    case QSGRendererInterface::Metal:
        return QStringLiteral("metal");
    case QSGRendererInterface::Software:
        return QStringLiteral("software");
    default:
        return QStringLiteral("other");
    }
}

QString FrameStats::summaryJson() const
{
    QJsonObject o;
    o["qt"] = QString(qVersion());
    o["env"] = envSummary();
    o["api"] = apiName();
    o["expected_ms"] = m_expectedMs;
    o["fps"] = m_fps;
    o["mean_ms"] = m_meanMs;
    o["stddev_ms"] = m_stddevMs;
    o["p99_ms"] = m_p99Ms;
    o["worst_ms"] = m_worstMs;
    o["stutters"] = m_stutterCount;
    int frames = 0;
    {
        QMutexLocker lock(&m_mutex);
        frames = m_deltasNs.size();
    }
    o["frames"] = frames;
    o["anim_step_stddev_px"] = m_animStepStddevPx;
    return QString::fromUtf8(QJsonDocument(o).toJson(QJsonDocument::Compact));
}

}

#include "moc_framestats.cpp"
