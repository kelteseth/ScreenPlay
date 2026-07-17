// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#pragma once

#include <QElapsedTimer>
#include <QMutex>
#include <QObject>
#include <QQuickWindow>
#include <QTimer>
#include <QVariantList>
#include <QVector>
#include <atomic>

namespace ScreenPlay {

/*!
    Frame pacing diagnostics for the main window (FPS overlay).

    Collects presentation timing (QQuickWindow::frameSwapped, emitted on the
    render thread right after present) and animation-step samples fed from QML
    on the GUI thread. The frameSwapped cadence is the best portable proxy for
    present pacing; the animation step variance is the "what the eye sees"
    metric: for a constant-velocity animation every displayed frame should
    move the item by exactly velocity/refreshRate pixels.

    Collection only runs while \c enabled is true (the overlay is visible) -
    the render-thread hook is a single atomic load otherwise. Sample storage
    is bounded so the overlay can stay open for hours.
*/
class FrameStats : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged FINAL)
    Q_PROPERTY(double fps READ fps NOTIFY statsChanged FINAL)
    Q_PROPERTY(double meanMs READ meanMs NOTIFY statsChanged FINAL)
    Q_PROPERTY(double stddevMs READ stddevMs NOTIFY statsChanged FINAL)
    Q_PROPERTY(double p99Ms READ p99Ms NOTIFY statsChanged FINAL)
    Q_PROPERTY(double worstMs READ worstMs NOTIFY statsChanged FINAL)
    Q_PROPERTY(int stutterCount READ stutterCount NOTIFY statsChanged FINAL)
    Q_PROPERTY(double expectedMs READ expectedMs NOTIFY statsChanged FINAL)
    Q_PROPERTY(double animStepStddevPx READ animStepStddevPx NOTIFY statsChanged FINAL)
    Q_PROPERTY(QString apiName READ apiName NOTIFY statsChanged FINAL)
    Q_PROPERTY(QString envSummary READ envSummary CONSTANT FINAL)

public:
    explicit FrameStats(QObject* parent = nullptr);

    Q_INVOKABLE void attach(QQuickWindow* window);
    Q_INVOKABLE void resetStats();
    Q_INVOKABLE QVariantList recentDeltas(int n);
    Q_INVOKABLE void recordAnimSample(double x);
    Q_INVOKABLE QString summaryJson() const;

    bool enabled() const { return m_enabled.load(std::memory_order_relaxed); }
    void setEnabled(bool enabled);

    double fps() const { return m_fps; }
    double meanMs() const { return m_meanMs; }
    double stddevMs() const { return m_stddevMs; }
    double p99Ms() const { return m_p99Ms; }
    double worstMs() const { return m_worstMs; }
    int stutterCount() const { return m_stutterCount; }
    double expectedMs() const { return m_expectedMs; }
    double animStepStddevPx() const { return m_animStepStddevPx; }
    QString apiName() const;
    QString envSummary() const;

signals:
    void enabledChanged(bool enabled);
    void statsChanged();

private:
    void onFrameSwapped(); // render thread!
    void refresh(); // GUI thread, timer driven

    // At 144 Hz this is a ~2.3 minute sliding window - plenty for pacing
    // statistics while keeping memory bounded for day-long sessions.
    static constexpr int maxSamples = 20000;

    std::atomic<bool> m_enabled { false };
    mutable QMutex m_mutex;
    QElapsedTimer m_clock;
    qint64 m_lastSwapNs = -1;
    QVector<qint64> m_deltasNs; // render thread appends under mutex

    // GUI thread animation samples
    double m_lastAnimX = -1;
    QVector<double> m_animSteps;

    QQuickWindow* m_window = nullptr;
    QTimer m_refreshTimer;

    double m_fps = 0, m_meanMs = 0, m_stddevMs = 0, m_p99Ms = 0, m_worstMs = 0;
    double m_expectedMs = 16.67, m_animStepStddevPx = 0;
    int m_stutterCount = 0;
};

}
