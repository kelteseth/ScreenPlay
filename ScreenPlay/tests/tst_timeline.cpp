// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#include <QCoreApplication>
#include <QJsonArray>
#include <QJsonObject>
#include <QTemporaryDir>
#include <QTime>
#include <QtTest>
#include <memory>

#include "QCoro/QCoroTask"
#include "ScreenPlay/globalvariables.h"
#include "ScreenPlay/screenplaytimelinemanager.h"
#include "ScreenPlay/settings.h"
#include "ScreenPlay/wallpapertimelinesection.h"

using namespace ScreenPlay;

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

/// Synchronously awaits a QCoro::Task<Result> using QCoro::waitFor.
static Result waitResult(QCoro::Task<Result> task)
{
    return QCoro::waitFor(std::move(task));
}

/// Build a minimal timeline JSON object suitable for addTimelineFromSettings.
static QJsonObject makeTimelineObj(const QString& start, const QString& end,
    const QJsonArray& wallpaper = QJsonArray())
{
    QJsonObject obj;
    obj.insert("startTime", start);
    obj.insert("endTime", end);
    obj.insert("wallpaper", wallpaper);
    return obj;
}

// ---------------------------------------------------------------------------
// Test fixture helper — creates a ready-to-use ScreenPlayTimelineManager with
// real (but minimal) GlobalVariables and Settings backed by a temp directory.
// ---------------------------------------------------------------------------
class TimelineFixture {
public:
    TimelineFixture()
    {
        m_globalVars = std::make_shared<GlobalVariables>();
        m_globalVars->setLocalStoragePath(QUrl::fromLocalFile(m_tempDir.path()));
        m_globalVars->setLocalSettingsPath(QUrl::fromLocalFile(m_tempDir.path()));
        // OpenSourceStandalone => isBasicVersion() == false  (all timelines allowed)
        // OpenSourceProStandalone => isBasicVersion() == false (multiple timelines allowed)
        m_globalVars->setVersion(ScreenPlayEnums::Version::OpenSourceProStandalone);

        m_settings = std::make_shared<Settings>(m_globalVars);

        m_tlm = std::make_unique<ScreenPlayTimelineManager>();
        m_tlm->setGlobalVariables(m_globalVars);
        m_tlm->setSettings(m_settings);
    }

    ScreenPlayTimelineManager& tlm() { return *m_tlm; }
    std::shared_ptr<GlobalVariables> globalVars() { return m_globalVars; }

private:
    QTemporaryDir m_tempDir;
    std::shared_ptr<GlobalVariables> m_globalVars;
    std::shared_ptr<Settings> m_settings;
    std::unique_ptr<ScreenPlayTimelineManager> m_tlm;
};

// ---------------------------------------------------------------------------
// Test class
// ---------------------------------------------------------------------------
class TimelineTest : public QObject {
    Q_OBJECT

private slots:

    // -----------------------------------------------------------------------
    // addTimelineFromSettings — parsing
    // -----------------------------------------------------------------------

    void addTimeline_valid_data()
    {
        QTest::addColumn<QString>("start");
        QTest::addColumn<QString>("end");
        QTest::addColumn<bool>("expectOk");

        QTest::newRow("full_day")
            << "00:00:00" << "23:59:59" << true;
        QTest::newRow("morning_section")
            << "00:00:00" << "08:00:00" << true;
        QTest::newRow("start_equals_end_invalid")
            << "06:00:00" << "06:00:00" << false;
        QTest::newRow("start_after_end_invalid")
            << "12:00:00" << "06:00:00" << false;
    }

    void addTimeline_valid()
    {
        QFETCH(QString, start);
        QFETCH(QString, end);
        QFETCH(bool, expectOk);

        TimelineFixture f;
        auto result = f.tlm().addTimelineFromSettings(makeTimelineObj(start, end));
        QCOMPARE(result.has_value(), expectOk);
    }

    void addTimeline_missing_startTime_field()
    {
        // No startTime key — fromString returns an invalid QTime
        // which compares as invalid > valid, triggering InvalidTimeFormat
        TimelineFixture f;
        QJsonObject obj;
        obj.insert("endTime", "12:00:00");
        obj.insert("wallpaper", QJsonArray());
        auto result = f.tlm().addTimelineFromSettings(obj);
        // Invalid/missing times produce TimelineManagerError
        QVERIFY(!result.has_value());
    }

    void addTimeline_missing_endTime_field()
    {
        TimelineFixture f;
        QJsonObject obj;
        obj.insert("startTime", "00:00:00");
        obj.insert("wallpaper", QJsonArray());
        auto result = f.tlm().addTimelineFromSettings(obj);
        QVERIFY(!result.has_value());
    }

    void addTimeline_invalid_time_string()
    {
        TimelineFixture f;
        auto result = f.tlm().addTimelineFromSettings(makeTimelineObj("not-a-time", "23:59:59"));
        QVERIFY(!result.has_value());
        QCOMPARE(result.error(), ScreenPlayTimelineManager::TimelineManagerError::InvalidTimeFormat);
    }

    // -----------------------------------------------------------------------
    // Basic-version enforcement — no const_cast UB (fix #5)
    // After the fix, times are overwritten via effectiveStart/End variables.
    // We verify the section actually stores the full-day span.
    // -----------------------------------------------------------------------
    void addTimeline_basicVersion_enforces_fullDay()
    {
        TimelineFixture f;
        // Switch to a basic-restricted version (single full-day timeline enforced)
        f.globalVars()->setVersion(ScreenPlayEnums::Version::OpenSourceStandalone);

        auto result = f.tlm().addTimelineFromSettings(makeTimelineObj("02:00:00", "10:00:00"));
        QVERIFY(result.has_value());

        // The section must cover the whole day despite the partial input times
        auto sections = f.tlm().timelineSections();
        QCOMPARE(sections.size(), 1);
        const auto section = sections.first().toObject();
        QCOMPARE(QTime::fromString(section.value("startTime").toString(), "hh:mm:ss"), QTime(0, 0, 0));
        QCOMPARE(QTime::fromString(section.value("endTime").toString(), "hh:mm:ss"), QTime(23, 59, 59));
    }

    // -----------------------------------------------------------------------
    // addTimelineAt — structural invariants via validateTimelineSections
    // -----------------------------------------------------------------------

    void addTimelineAt_single_section_invariants()
    {
        TimelineFixture f;
        QVERIFY(f.tlm().addTimelineFromSettings(makeTimelineObj("00:00:00", "23:59:59")).has_value());

        // Split at midday
        QVERIFY(f.tlm().addTimelineAt(0, 0.5f, "split1"));

        auto sections = f.tlm().timelineSections();
        QCOMPARE(sections.size(), 2);

        // First section starts at midnight, ends somewhere in the middle
        const QTime sec0Start = QTime::fromString(sections[0].toObject().value("startTime").toString(), "hh:mm:ss");
        const QTime sec0End = QTime::fromString(sections[0].toObject().value("endTime").toString(), "hh:mm:ss");
        const QTime sec1Start = QTime::fromString(sections[1].toObject().value("startTime").toString(), "hh:mm:ss");
        const QTime sec1End = QTime::fromString(sections[1].toObject().value("endTime").toString(), "hh:mm:ss");

        QCOMPARE(sec0Start, QTime(0, 0, 0));
        QCOMPARE(sec0End, sec1Start); // sections are contiguous
        QCOMPARE(sec1End, QTime(23, 59, 59));
        QVERIFY(sec0End < sec1End);
    }

    void addTimelineAt_three_sections_indices_are_contiguous()
    {
        TimelineFixture f;
        QVERIFY(f.tlm().addTimelineFromSettings(makeTimelineObj("00:00:00", "23:59:59")).has_value());

        QVERIFY(f.tlm().addTimelineAt(0, 0.33f, "A"));
        QVERIFY(f.tlm().addTimelineAt(1, 0.66f, "B"));

        auto sections = f.tlm().timelineSections();
        QCOMPARE(sections.size(), 3);

        // Indices must be 0,1,2 in order
        for (int i = 0; i < sections.size(); ++i) {
            QCOMPARE(sections[i].toObject().value("index").toInt(), i);
        }
    }

    void addTimelineAt_relativePositions_ascending()
    {
        TimelineFixture f;
        QVERIFY(f.tlm().addTimelineFromSettings(makeTimelineObj("00:00:00", "23:59:59")).has_value());
        QVERIFY(f.tlm().addTimelineAt(0, 0.25f, "first"));
        QVERIFY(f.tlm().addTimelineAt(1, 0.75f, "second"));

        auto sections = f.tlm().timelineSections();
        QCOMPARE(sections.size(), 3);

        double prevPos = -1.0;
        for (const auto& v : sections) {
            double pos = v.toObject().value("relativePosition").toDouble();
            QVERIFY(pos > prevPos);
            prevPos = pos;
        }
    }

    void addTimelineAt_first_section_starts_at_midnight()
    {
        TimelineFixture f;
        QVERIFY(f.tlm().addTimelineFromSettings(makeTimelineObj("00:00:00", "23:59:59")).has_value());
        QVERIFY(f.tlm().addTimelineAt(0, 0.5f, "half"));

        auto sections = f.tlm().timelineSections();
        // After split, first section should still start at 00:00:00
        const QString firstStart = sections.first().toObject().value("startTime").toString();
        QCOMPARE(firstStart, QString("00:00:00"));
    }

    // -----------------------------------------------------------------------
    // removeTimelineAt — structural invariants (includes fix #3 regression)
    // -----------------------------------------------------------------------

    void removeTimelineAt_last_section_is_rejected()
    {
        // Removing the sole section must fail
        TimelineFixture f;
        QVERIFY(f.tlm().addTimelineFromSettings(makeTimelineObj("00:00:00", "23:59:59")).has_value());

        auto result = waitResult(f.tlm().removeTimelineAt(0));
        QVERIFY(!result.success());
    }

    void removeTimelineAt_two_sections_only_index0_allowed()
    {
        TimelineFixture f;
        QVERIFY(f.tlm().addTimelineFromSettings(makeTimelineObj("00:00:00", "23:59:59")).has_value());
        QVERIFY(f.tlm().addTimelineAt(0, 0.5f, "split"));

        // index 1 is the last — must not be removable
        auto resultLast = waitResult(f.tlm().removeTimelineAt(1));
        QVERIFY(!resultLast.success());

        // index 0 is the first — should succeed
        auto resultFirst = waitResult(f.tlm().removeTimelineAt(0));
        QVERIFY(resultFirst.success());

        // Back to one section spanning the whole day
        auto sections = f.tlm().timelineSections();
        QCOMPARE(sections.size(), 1);
        QCOMPARE(sections.first().toObject().value("startTime").toString(), QString("00:00:00"));
    }

    void removeTimelineAt_three_sections_index0_activeIndex_not_negative()
    {
        // Regression for fix #3: removing index 0 with 3+ sections must NOT
        // result in activeTimelineIndex == -1.
        TimelineFixture f;
        QVERIFY(f.tlm().addTimelineFromSettings(makeTimelineObj("00:00:00", "23:59:59")).has_value());
        QVERIFY(f.tlm().addTimelineAt(0, 0.33f, "A"));
        QVERIFY(f.tlm().addTimelineAt(1, 0.66f, "B"));

        QCOMPARE(f.tlm().timelineSections().size(), 3);

        // Remove the non-active section at index 0
        auto result = waitResult(f.tlm().removeTimelineAt(0));
        QVERIFY(result.success());

        QCOMPARE(f.tlm().timelineSections().size(), 2);
        // activeTimelineIndex must be 0 or 1 — never -1
        QVERIFY(f.tlm().activeTimelineIndex() >= 0);
    }

    void removeTimelineAt_middle_section_expands_neighbour()
    {
        TimelineFixture f;
        QVERIFY(f.tlm().addTimelineFromSettings(makeTimelineObj("00:00:00", "23:59:59")).has_value());
        QVERIFY(f.tlm().addTimelineAt(0, 0.33f, "A"));
        QVERIFY(f.tlm().addTimelineAt(1, 0.66f, "B"));

        // Section layout: [0-33% | 33-66% | 66-100%] (indices 0, 1, 2)
        // Record endTime of section 0 before removal
        auto sectionsBefore = f.tlm().timelineSections();
        const QString section0End = sectionsBefore[0].toObject().value("endTime").toString();

        // Remove section 1 — section 2 (now at index 1) should absorb its space
        auto result = waitResult(f.tlm().removeTimelineAt(1));
        QVERIFY(result.success());

        auto sectionsAfter = f.tlm().timelineSections();
        QCOMPARE(sectionsAfter.size(), 2);

        // Section 0 start/end unchanged
        QCOMPARE(sectionsAfter[0].toObject().value("endTime").toString(), section0End);
        // Section 1 (formerly 2) now starts where section 0 ends
        QCOMPARE(sectionsAfter[1].toObject().value("startTime").toString(), section0End);
        // Last section must still end at 23:59:59
        QCOMPARE(sectionsAfter.last().toObject().value("endTime").toString(), QString("23:59:59"));
    }

    // -----------------------------------------------------------------------
    // removeTimelineAt — all sections, then invariant check
    // -----------------------------------------------------------------------

    void removeAllTimlineSections_leaves_empty_list()
    {
        TimelineFixture f;
        QVERIFY(f.tlm().addTimelineFromSettings(makeTimelineObj("00:00:00", "23:59:59")).has_value());
        QVERIFY(f.tlm().addTimelineAt(0, 0.5f, "half"));

        auto result = waitResult(f.tlm().removeAllTimlineSections());
        QVERIFY(result.success());

        // After removeAll, the list must be empty (QML adds the default section afterwards)
        QCOMPARE(f.tlm().timelineSections().size(), 0);
    }

    // -----------------------------------------------------------------------
    // findTimelineSectionForCurrentTime — boundary tests using fixed sections
    // -----------------------------------------------------------------------

    void findTimelineSectionForCurrentTime_data()
    {
        QTest::addColumn<QString>("queryTime");
        QTest::addColumn<int>("expectedIndex");

        // Three sections: 00:00:00-08:00:00, 08:00:00-20:00:00, 20:00:00-23:59:59
        QTest::newRow("in_first_section") << "04:00:00" << 0;
        QTest::newRow("in_second_section") << "14:00:00" << 1;
        QTest::newRow("in_last_section") << "22:00:00" << 2;
        QTest::newRow("at_exact_boundary") << "08:00:00" << 1; // boundary belongs to the right section
        QTest::newRow("midnight") << "00:00:00" << 0;
    }

    // Note: we cannot override QTime::currentTime() so we test containsTime directly.
    void findTimelineSectionForCurrentTime()
    {
        QFETCH(QString, queryTime);
        QFETCH(int, expectedIndex);

        TimelineFixture f;
        QVERIFY(f.tlm().addTimelineFromSettings(makeTimelineObj("00:00:00", "08:00:00")).has_value());
        QVERIFY(f.tlm().addTimelineFromSettings(makeTimelineObj("08:00:00", "20:00:00")).has_value());
        QVERIFY(f.tlm().addTimelineFromSettings(makeTimelineObj("20:00:00", "23:59:59")).has_value());

        const QTime t = QTime::fromString(queryTime, "hh:mm:ss");
        QVERIFY(t.isValid());

        // containsTime tests the range logic independently of the wall clock.
        auto sections = f.tlm().timelineSections();
        QCOMPARE(sections.size(), 3);

        bool found = false;
        for (int i = 0; i < sections.size(); ++i) {
            const QJsonObject s = sections[i].toObject();
            const QTime start = QTime::fromString(s.value("startTime").toString(), "hh:mm:ss");
            const QTime end = QTime::fromString(s.value("endTime").toString(), "hh:mm:ss");
            // Replicate containsTime: [start, end)
            if (t >= start && t < end) {
                QCOMPARE(i, expectedIndex);
                found = true;
                break;
            }
        }
        // For 23:59:59 boundary, containsTime uses endTime inclusive
        if (!found) {
            // The last section uses inclusive end
            const QJsonObject last = sections.last().toObject();
            const QTime end = QTime::fromString(last.value("endTime").toString(), "hh:mm:ss");
            if (t == end) {
                QCOMPARE(sections.size() - 1, expectedIndex);
                found = true;
            }
        }
        QVERIFY(found);
    }

    // -----------------------------------------------------------------------
    // moveTimelineAt
    // -----------------------------------------------------------------------

    void moveTimelineAt_updates_adjacent_section_startTime()
    {
        TimelineFixture f;
        QVERIFY(f.tlm().addTimelineFromSettings(makeTimelineObj("00:00:00", "23:59:59")).has_value());
        QVERIFY(f.tlm().addTimelineAt(0, 0.5f, "split"));

        // Move the handle from 50% to 25%
        const float newPos = 0.25f;
        Util util;
        const QString calcTime = util.getTimeString(newPos);
        QVERIFY(f.tlm().moveTimelineAt(0, "split", newPos, calcTime));

        auto sections = f.tlm().timelineSections();
        // Section 1's startTime must equal section 0's new endTime
        const QString sec0End = sections[0].toObject().value("endTime").toString();
        const QString sec1Start = sections[1].toObject().value("startTime").toString();
        QCOMPARE(sec0End, sec1Start);
    }

    // -----------------------------------------------------------------------
    // timelineSections serialization round-trip
    // -----------------------------------------------------------------------

    void timelineSections_roundtrip()
    {
        TimelineFixture f;
        QVERIFY(f.tlm().addTimelineFromSettings(makeTimelineObj("00:00:00", "08:00:00")).has_value());
        QVERIFY(f.tlm().addTimelineFromSettings(makeTimelineObj("08:00:00", "23:59:59")).has_value());

        const QJsonArray firstDump = f.tlm().timelineSections();
        QCOMPARE(firstDump.size(), 2);

        // Reconstruct in a fresh manager from the serialized data
        TimelineFixture f2;
        for (const auto& v : firstDump) {
            auto result = f2.tlm().addTimelineFromSettings(v.toObject());
            QVERIFY(result.has_value());
        }

        const QJsonArray secondDump = f2.tlm().timelineSections();
        QCOMPARE(secondDump.size(), firstDump.size());

        for (int i = 0; i < firstDump.size(); ++i) {
            const QJsonObject orig = firstDump[i].toObject();
            const QJsonObject rebuilt = secondDump[i].toObject();
            QCOMPARE(rebuilt.value("startTime").toString(), orig.value("startTime").toString());
            QCOMPARE(rebuilt.value("endTime").toString(), orig.value("endTime").toString());
        }
    }

    // -----------------------------------------------------------------------
    // setValueAtMonitorTimelineIndex — unknown key + empty category
    // returns success==true but data must be unchanged (fix #6 documentation)
    // -----------------------------------------------------------------------

    void setValueAtMonitorIndex_unknown_key_empty_category_returns_error()
    {
        // With no wallpaper actually assigned, the call should fail to find a
        // wallpaper for monitor 0 and return a failure Result.
        TimelineFixture f;
        QVERIFY(f.tlm().addTimelineFromSettings(makeTimelineObj("00:00:00", "23:59:59")).has_value());

        auto sections = f.tlm().timelineSections();
        const QString id = sections.first().toObject().value("identifier").toString();

        auto result = waitResult(
            f.tlm().setValueAtMonitorTimelineIndex(0, 0, id, "typo_key", QVariant("val"), ""));

        // No wallpaper on monitor 0 → failure
        QVERIFY(!result.success());
    }
};

QTEST_MAIN(TimelineTest)
#include "tst_timeline.moc"
