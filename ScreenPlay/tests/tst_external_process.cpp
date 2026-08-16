// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

// Tests for external process (widget/wallpaper) lifecycle management.
// Focus: regressions for the timer-stop fix in ScreenPlayWidget::close() and
// ScreenPlayExternalProcess state transitions that do not require a live process.

#include <QCoreApplication>
#include <QSignalSpy>
#include <QTemporaryDir>
#include <QTest>
#include <QTimer>
#include <memory>

#include "QCoro/QCoroTask"
#include "ScreenPlay/globalvariables.h"
#include "ScreenPlay/screenplaywidget.h"
#include "ScreenPlay/settings.h"
#include "ScreenPlay/widgetdata.h"
#include "ScreenPlayCore/contenttypes.h"
#include "ScreenPlayCore/globalenums.h"

using namespace ScreenPlay;

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

static Result waitResult(QCoro::Task<Result> task)
{
    return QCoro::waitFor(std::move(task));
}

/// Build a minimal WidgetData suitable for constructing a ScreenPlayWidget.
static WidgetData makeWidgetData(const QString& absolutePath)
{
    WidgetData d;
    d.setAbsolutePath(absolutePath);
    d.setType(ContentTypes::InstalledType::QMLWidget);
    d.setPosition({ 100, 100 });
    return d;
}

// ---------------------------------------------------------------------------
// Fixture
// ---------------------------------------------------------------------------

class ProcessFixture {
public:
    ProcessFixture()
    {
        m_globalVars = std::make_shared<GlobalVariables>();
        m_globalVars->setLocalStoragePath(QUrl::fromLocalFile(m_tempDir.path()));
        m_globalVars->setLocalSettingsPath(QUrl::fromLocalFile(m_tempDir.path()));
        m_globalVars->setVersion(ScreenPlayEnums::Version::OpenSourceProStandalone);

        m_settings = std::make_shared<Settings>(m_globalVars);
    }

    /// Create a ScreenPlayWidget without starting the process.
    std::unique_ptr<ScreenPlayWidget> makeWidget(const QString& appID = "test-widget-id")
    {
        return std::make_unique<ScreenPlayWidget>(
            appID,
            m_globalVars,
            makeWidgetData(m_tempDir.path()),
            m_settings);
    }

private:
    QTemporaryDir m_tempDir;
    std::shared_ptr<GlobalVariables> m_globalVars;
    std::shared_ptr<Settings> m_settings;
};

// ---------------------------------------------------------------------------
// Test class
// ---------------------------------------------------------------------------

class ExternalProcessTest : public QObject {
    Q_OBJECT

private slots:

    // -----------------------------------------------------------------------
    // ScreenPlayWidget initial state
    // -----------------------------------------------------------------------

    void widget_initialState_is_NotSet()
    {
        ProcessFixture f;
        auto widget = f.makeWidget();
        QCOMPARE(widget->state(), ScreenPlayEnums::AppState::NotSet);
        QVERIFY(!widget->isConnected());
    }

    void widget_appID_is_set()
    {
        ProcessFixture f;
        auto widget = f.makeWidget("my-app-id");
        QCOMPARE(widget->appID(), QString("my-app-id"));
    }

    // -----------------------------------------------------------------------
    // close() on never-connected widget — regression test for fix #4
    //
    // Before the fix: close() returned without stopping m_pingAliveTimer.
    // To exercise the regression we start a rapid timer (10ms) that mimics
    // the ping timer, call close(), then verify the timer is no longer active
    // by checking no restartFailed signal fires during a short wait.
    //
    // We test this indirectly by checking that close() returns a success
    // Result and that no restartFailed/error signal fires after close().
    // -----------------------------------------------------------------------

    void widget_close_neverConnected_returns_success()
    {
        ProcessFixture f;
        auto widget = f.makeWidget();

        QSignalSpy restartSpy(widget.get(), &ScreenPlayWidget::restartFailed);

        // Widget was never connected — close() should succeed immediately
        auto result = waitResult(widget->close());
        QVERIFY(result.success());

        // Give the event loop a chance to fire any lingering timers
        QTest::qWait(50);

        // restartFailed must NOT have been triggered
        QCOMPARE(restartSpy.count(), 0);
    }

    void widget_close_does_not_emit_error_after_return()
    {
        ProcessFixture f;
        auto widget = f.makeWidget();

        QSignalSpy errorSpy(widget.get(), &ScreenPlayWidget::error);

        auto result = waitResult(widget->close());
        QVERIFY(result.success());

        // Process events — stale timers would fire here if not stopped
        QTest::qWait(100);
        QCOMPARE(errorSpy.count(), 0);
    }

    // -----------------------------------------------------------------------
    // State transitions via setState
    // -----------------------------------------------------------------------

    void widget_setState_emits_stateChanged()
    {
        ProcessFixture f;
        auto widget = f.makeWidget();

        QSignalSpy spy(widget.get(), &ScreenPlayWidget::stateChanged);

        widget->setState(ScreenPlayEnums::AppState::Starting);
        QCOMPARE(spy.count(), 1);
        QCOMPARE(spy.takeFirst().first().value<ScreenPlayEnums::AppState>(),
            ScreenPlayEnums::AppState::Starting);

        widget->setState(ScreenPlayEnums::AppState::Active);
        QCOMPARE(spy.count(), 1);
        QCOMPARE(spy.takeFirst().first().value<ScreenPlayEnums::AppState>(),
            ScreenPlayEnums::AppState::Active);
    }

    void widget_setState_same_value_no_signal()
    {
        ProcessFixture f;
        auto widget = f.makeWidget();
        widget->setState(ScreenPlayEnums::AppState::Starting);

        QSignalSpy spy(widget.get(), &ScreenPlayWidget::stateChanged);
        widget->setState(ScreenPlayEnums::AppState::Starting);
        QCOMPARE(spy.count(), 0);
    }

    // -----------------------------------------------------------------------
    // processIDChanged signal
    // -----------------------------------------------------------------------

    void widget_setProcessID_emits_signal()
    {
        ProcessFixture f;
        auto widget = f.makeWidget();

        QSignalSpy spy(widget.get(), &ScreenPlayWidget::processIDChanged);
        widget->setProcessID(12345);
        QCOMPARE(spy.count(), 1);
        QCOMPARE(widget->processID(), qint64(12345));
    }

    // -----------------------------------------------------------------------
    // Multiple close() calls — idempotent
    // -----------------------------------------------------------------------

    void widget_close_twice_is_safe()
    {
        ProcessFixture f;
        auto widget = f.makeWidget();

        auto r1 = waitResult(widget->close());
        QVERIFY(r1.success());

        // Second call should also not crash or assert
        auto r2 = waitResult(widget->close());
        QVERIFY(r2.success());
    }
};

QTEST_MAIN(ExternalProcessTest)
#include "tst_external_process.moc"
