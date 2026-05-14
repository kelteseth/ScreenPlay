// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QGuiApplication>
#include <QImage>
#include <QSignalSpy>
#include <QtTest>

#include "ScreenPlay/createimportvideo.h"
#include "ScreenPlay/gifoptimizer.h"
#include "ScreenPlayCore/testtmpdir.h"
#include "ScreenPlayCore/util.h"

#include "CMakeVariables.h"

// Test videos:
// H264 (nebula):    mantissa.xyz_loop_072.mp4  — 2048x1152, 60fps, 900 frames, 15s
// WebM (astronaut): mantissa.xyz_loop_075.webm — 2048x1152, 30fps, 420 frames, 14s
// AV1  (shapes):    Mantissa.Xyz Loop 091.mkv  — 3840x2160, 60fps, 600 frames, 10s

class GifOptimizerTest : public QObject {
    Q_OBJECT

private:
    QString m_tmpDir;
    QString m_nebulaH264;
    QString m_astronautWebm;
    QString m_shapesAv1;

    static constexpr int DefaultQuality = 22;
    static constexpr int OneMB = 1024 * 1024;

    std::unique_ptr<ScreenPlay::CreateImportVideo> makeImporter(
        const QString& videoPath,
        const QString& exportPath,
        ScreenPlay::Video::VideoCodec targetCodec,
        std::atomic<bool>& interrupt)
    {
        return std::make_unique<ScreenPlay::CreateImportVideo>(
            videoPath, exportPath, targetCodec, DefaultQuality, interrupt);
    }

    QString makeSubDir(const QString& name)
    {
        const QString dir = m_tmpDir + "/" + name;
        QDir().mkpath(dir);
        return dir;
    }

    bool fileExistsAndNonEmpty(const QString& path)
    {
        QFileInfo fi(path);
        return fi.exists() && fi.size() > 0;
    }

    void printDirSummary(const QString& label, const QString& dir)
    {
        if (!QDir(dir).exists())
            return;
        qint64 totalSize = 0;
        int fileCount = 0;
        QDirIterator it(dir, QDir::Files, QDirIterator::Subdirectories);
        while (it.hasNext()) {
            it.next();
            totalSize += it.fileInfo().size();
            ++fileCount;
            qInfo().noquote() << QString("  %1 (%2 KB)")
                                     .arg(it.fileInfo().fileName())
                                     .arg(it.fileInfo().size() / 1024);
        }
        qInfo().noquote() << QString("%1: %2 files, %3 KB total")
                                 .arg(label)
                                 .arg(fileCount)
                                 .arg(totalSize / 1024);
    }

private slots:
    void initTestCase()
    {
        m_nebulaH264 = QString(SCREENPLAY_SOURCE_DIR)
            + "/Content/wallpaper_video_nebula_h264/mantissa.xyz_loop_072.mp4";
        m_astronautWebm = QString(SCREENPLAY_SOURCE_DIR)
            + "/Content/wallpaper_video_astronaut_vp9/mantissa.xyz_loop_075.webm";
        m_shapesAv1 = QString(SCREENPLAY_SOURCE_DIR)
            + "/Content/wallpaper_video_shapes_av1/Mantissa.Xyz Loop 091.mkv";

        m_tmpDir = ScreenPlay::createTestTmpDir("GifOptimizer");
        QVERIFY2(!m_tmpDir.isEmpty(), "Failed to create test tmp dir");

        qInfo() << "Test output:" << m_tmpDir;
        QVERIFY2(QFile::exists(m_nebulaH264),
            qPrintable("Nebula H264 video not found: " + m_nebulaH264));
        QVERIFY2(QFile::exists(m_astronautWebm),
            qPrintable("Astronaut WebM video not found: " + m_astronautWebm));
        QVERIFY2(QFile::exists(m_shapesAv1),
            qPrintable("Shapes AV1 video not found: " + m_shapesAv1));
    }

    void cleanupTestCase()
    {
        qInfo() << "\n════════════════════════ GifOptimizer Test Summary ════════════════════════";
        qInfo().noquote() << "Output dir (preserved):" << m_tmpDir;
        for (const auto& sub : QDir(m_tmpDir).entryList(QDir::Dirs | QDir::NoDotAndDotDot))
            printDirSummary(sub, m_tmpDir + "/" + sub);
        qInfo() << "══════════════════════════════════════════════════════════════════════════";
    }

    // ═══════════════════════════════════════════════════════════════════════
    // Nebula (H264) → preview.webm → preview.gif
    // ═══════════════════════════════════════════════════════════════════════

    void nebulaToGif()
    {
        const QString dir = makeSubDir("nebula");
        std::atomic<bool> interrupt { false };

        auto importer = makeImporter(m_nebulaH264, dir,
            ScreenPlay::Video::VideoCodec::VP9, interrupt);
        QVERIFY(importer->createWallpaperInfo());
        QVERIFY(importer->createWallpaperVideoPreview());
        QVERIFY(fileExistsAndNonEmpty(dir + "/preview.webm"));

        ScreenPlay::GifOptimizer optimizer(ScreenPlay::Util::ffmpegExecutable(), interrupt);
        auto result = optimizer.optimize(dir + "/preview.webm", dir + "/preview.gif");

        if (!result.has_value()) {
            QFAIL(qPrintable("Nebula GIF failed: " + result.error().message));
        }
        QVERIFY(result->fileSize > 0);
        QVERIFY(result->fileSize <= OneMB);
        QVERIFY(fileExistsAndNonEmpty(dir + "/preview.gif"));

        qInfo().noquote()
            << QString("Nebula: %1 bytes, %2x%3 @ %4fps, %5 colors, tier %6")
                   .arg(result->fileSize)
                   .arg(result->width)
                   .arg(result->height)
                   .arg(result->fps)
                   .arg(result->maxColors)
                   .arg(result->passUsed);
    }

    // ═══════════════════════════════════════════════════════════════════════
    // Astronaut (WebM/VP9) → preview.webm → preview.gif
    // ═══════════════════════════════════════════════════════════════════════

    void astronautToGif()
    {
        const QString dir = makeSubDir("astronaut");
        std::atomic<bool> interrupt { false };

        auto importer = makeImporter(m_astronautWebm, dir,
            ScreenPlay::Video::VideoCodec::VP9, interrupt);
        QVERIFY(importer->createWallpaperInfo());
        QVERIFY(importer->createWallpaperVideoPreview());
        QVERIFY(fileExistsAndNonEmpty(dir + "/preview.webm"));

        ScreenPlay::GifOptimizer optimizer(ScreenPlay::Util::ffmpegExecutable(), interrupt);
        auto result = optimizer.optimize(dir + "/preview.webm", dir + "/preview.gif");

        if (!result.has_value()) {
            QFAIL(qPrintable("Astronaut GIF failed: " + result.error().message));
        }
        QVERIFY(result->fileSize > 0);
        QVERIFY(result->fileSize <= OneMB);
        QVERIFY(fileExistsAndNonEmpty(dir + "/preview.gif"));

        qInfo().noquote()
            << QString("Astronaut: %1 bytes, %2x%3 @ %4fps, %5 colors, tier %6")
                   .arg(result->fileSize)
                   .arg(result->width)
                   .arg(result->height)
                   .arg(result->fps)
                   .arg(result->maxColors)
                   .arg(result->passUsed);
    }

    // ═══════════════════════════════════════════════════════════════════════
    // Shapes (AV1/4K) → preview.webm → preview.gif
    // ═══════════════════════════════════════════════════════════════════════

    void shapesToGif()
    {
        const QString dir = makeSubDir("shapes");
        std::atomic<bool> interrupt { false };

        auto importer = makeImporter(m_shapesAv1, dir,
            ScreenPlay::Video::VideoCodec::VP9, interrupt);
        QVERIFY(importer->createWallpaperInfo());
        QVERIFY(importer->createWallpaperVideoPreview());
        QVERIFY(fileExistsAndNonEmpty(dir + "/preview.webm"));

        ScreenPlay::GifOptimizer optimizer(ScreenPlay::Util::ffmpegExecutable(), interrupt);
        auto result = optimizer.optimize(dir + "/preview.webm", dir + "/preview.gif");

        if (!result.has_value()) {
            QFAIL(qPrintable("Shapes GIF failed: " + result.error().message));
        }
        QVERIFY(result->fileSize > 0);
        QVERIFY(result->fileSize <= OneMB);
        QVERIFY(fileExistsAndNonEmpty(dir + "/preview.gif"));

        qInfo().noquote()
            << QString("Shapes: %1 bytes, %2x%3 @ %4fps, %5 colors, tier %6")
                   .arg(result->fileSize)
                   .arg(result->width)
                   .arg(result->height)
                   .arg(result->fps)
                   .arg(result->maxColors)
                   .arg(result->passUsed);
    }

    // ═══════════════════════════════════════════════════════════════════════
    // Error case: missing input file
    // ═══════════════════════════════════════════════════════════════════════

    void missingInputReturnsError()
    {
        const QString dir = makeSubDir("missing_input");
        std::atomic<bool> interrupt { false };

        ScreenPlay::GifOptimizer optimizer(ScreenPlay::Util::ffmpegExecutable(), interrupt);

        auto result = optimizer.optimize(
            dir + "/does_not_exist.webm", dir + "/preview.gif");

        QVERIFY(!result.has_value());
        QVERIFY(result.error().message.contains("does not exist"));
    }
};

QTEST_MAIN(GifOptimizerTest)

#include "tst_GifOptimizer.moc"
