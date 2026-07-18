// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#pragma once

#include <QElapsedTimer>
#include <QObject>
#include <QTimer>

class QWindow;

namespace ScreenPlay {

// Caps how often a window's scene graph renders by holding back
// UpdateRequest events and re-requesting them when the frame budget
// allows. Requires the "basic" render loop - the threaded render loop
// paces itself without UpdateRequest and would bypass the filter.
class FrameRateLimiter : public QObject {
    Q_OBJECT

public:
    explicit FrameRateLimiter(QWindow* window, QObject* parent = nullptr);

    int maxFps() const { return m_maxFps; }
    void setMaxFps(int fps);

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    QWindow* const m_window;
    QTimer m_deliverTimer;
    QElapsedTimer m_clock;
    qint64 m_nextDueNs { 0 };
    int m_maxFps { 0 };
};
}
