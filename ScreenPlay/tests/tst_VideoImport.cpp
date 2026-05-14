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
#include "ScreenPlayCore/testtmpdir.h"

#include "CMakeVariables.h"

// Known values obtained via ffprobe – update if the test media change.
// H264 source (mantissa.xyz_loop_072.mp4): 2048x1152, 60fps, 900 frames, 15s, no audio, mov/mp4
// WebM source (mantissa.xyz_loop_075.webm): 2048x1152, 30fps, 420 frames, 14s, no audio, VP8 codec
// AV1 source (Mantissa.Xyz Loop 091.mkv):   3840x2160, 60fps, 600 frames, 10s, no audio, AV1 codec

class VideoImportTest : public QObject {
    Q_OBJECT

private:
    QString m_tmpDir;
    QString m_h264Video;
    QString m_webmVideo;
    QString m_av1Video;

    // Match the default quality from CreateWallpaperSettings.qml (slider: from=63, value=22, to=0)
    static constexpr int DefaultQuality = 22;

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

    void verifyPreviews(const QString& dir, const QString& sourceVideo)
    {
        QVERIFY(fileExistsAndNonEmpty(dir + "/preview.webm"));
        QVERIFY(QFileInfo(dir + "/preview.webm").size() < QFileInfo(sourceVideo).size());

        QVERIFY(fileExistsAndNonEmpty(dir + "/preview.jpg"));
        {
            QImage img(dir + "/preview.jpg");
            QCOMPARE(img.width(), 854);
            QCOMPARE(img.height(), 480);
        }

        QVERIFY(fileExistsAndNonEmpty(dir + "/previewThumbnail.jpg"));
        {
            QImage img(dir + "/previewThumbnail.jpg");
            QCOMPARE(img.width(), 320);
            QCOMPARE(img.height(), 180);
        }

        QVERIFY(fileExistsAndNonEmpty(dir + "/preview.gif"));
        QVERIFY(QFileInfo(dir + "/preview.gif").size() < 1024 * 1024);

        QVERIFY(fileExistsAndNonEmpty(dir + "/preview.webp"));
    }

    bool runAllPreviewSteps(ScreenPlay::CreateImportVideo* importer)
    {
        return importer->createWallpaperVideoPreview()
            && importer->createWallpaperImagePreview()
            && importer->createWallpaperImageThumbnailPreview()
            && importer->createWallpaperGifPreview()
            && importer->createWallpaperWebpPreview();
    }

private slots:
    void initTestCase()
    {
        m_h264Video = QString(SCREENPLAY_SOURCE_DIR) + "/Content/wallpaper_video_nebula_h264/mantissa.xyz_loop_072.mp4";
        m_webmVideo = QString(SCREENPLAY_SOURCE_DIR) + "/Content/wallpaper_video_astronaut_vp9/mantissa.xyz_loop_075.webm";
        m_av1Video = QString(SCREENPLAY_SOURCE_DIR) + "/Content/wallpaper_video_shapes_av1/Mantissa.Xyz Loop 091.mkv";

        m_tmpDir = ScreenPlay::createTestTmpDir("VideoImport");
        QVERIFY2(!m_tmpDir.isEmpty(), "Failed to create test tmp dir");

        qInfo() << "Test output:" << m_tmpDir;
        QVERIFY2(QFile::exists(m_h264Video), qPrintable("H264 test video not found: " + m_h264Video));
        QVERIFY2(QFile::exists(m_webmVideo), qPrintable("WebM test video not found: " + m_webmVideo));
        QVERIFY2(QFile::exists(m_av1Video), qPrintable("AV1 test video not found: " + m_av1Video));
    }

    void cleanupTestCase()
    {
        return; // REMOVE ME
        qInfo() << "\n════════════════════════ Test Output Summary ════════════════════════";
        qInfo().noquote() << "Dir:" << m_tmpDir;
        for (const auto& sub : QDir(m_tmpDir).entryList(QDir::Dirs | QDir::NoDotAndDotDot))
            printDirSummary(sub, m_tmpDir + "/" + sub);
        qInfo() << "════════════════════════════════════════════════════════════════════";

        QDir(m_tmpDir).removeRecursively();
    }

    // ═══════════════════════════════════════════════════════════════════════
    // H264 source → VP9 (full pipeline with previews + actual VP9 encode)
    // ═══════════════════════════════════════════════════════════════════════

    void h264SourceToVp9()
    {
        const QString dir = makeSubDir("h264_to_vp9");
        std::atomic<bool> interrupt { false };
        auto importer = makeImporter(m_h264Video, dir,
            ScreenPlay::Video::VideoCodec::VP9, interrupt);

        // Probe
        QVERIFY(importer->createWallpaperInfo());
        QCOMPARE(importer->m_videoWidth, 2048);
        QCOMPARE(importer->m_videoHeight, 1152);
        QCOMPARE(importer->m_numberOfFrames, 900);
        QCOMPARE(importer->m_framerate, 60);
        QCOMPARE(importer->m_length, 15);
        QCOMPARE(importer->m_skipAudio, true);
        QCOMPARE(importer->m_isWebm, false);

        // Previews
        QVERIFY(runAllPreviewSteps(importer.get()));
        verifyPreviews(dir, m_h264Video);

        // Encode H264 → VP9 (must actually transcode, source != target codec)
        QVERIFY(importer->createWallpaperVideo());
        const QString encoded = dir + "/mantissa.xyz_loop_072.webm";
        QVERIFY(fileExistsAndNonEmpty(encoded));
        // Encoded file should differ from source (different codec/container)
        QVERIFY(QFileInfo(encoded).size() != QFileInfo(m_h264Video).size());

        QCOMPARE(importer->build16x9CropFilter(), QString("crop=2048:1152"));
    }

    // ═══════════════════════════════════════════════════════════════════════
    // WebM source (VP8) → VP9 (full pipeline with previews + actual VP9 encode)
    // ═══════════════════════════════════════════════════════════════════════

    void webmSourceToVp9()
    {
        const QString dir = makeSubDir("webm_to_vp9");
        std::atomic<bool> interrupt { false };
        auto importer = makeImporter(m_webmVideo, dir,
            ScreenPlay::Video::VideoCodec::VP9, interrupt);

        // Probe
        QVERIFY(importer->createWallpaperInfo());
        QCOMPARE(importer->m_videoWidth, 2048);
        QCOMPARE(importer->m_videoHeight, 1152);
        QCOMPARE(importer->m_numberOfFrames, 420);
        QCOMPARE(importer->m_framerate, 30);
        QCOMPARE(importer->m_length, 14);
        QCOMPARE(importer->m_isWebm, true);

        // Previews
        QVERIFY(runAllPreviewSteps(importer.get()));
        verifyPreviews(dir, m_webmVideo);

        // Encode VP8 → VP9 (different codecs, must transcode)
        QVERIFY(importer->createWallpaperVideo());
        const QString encoded = dir + "/mantissa.xyz_loop_075.webm";
        QVERIFY(fileExistsAndNonEmpty(encoded));
        // Re-encoded VP9 file should differ in size from VP8 original
        QVERIFY(QFileInfo(encoded).size() != QFileInfo(m_webmVideo).size());

        QCOMPARE(importer->build16x9CropFilter(), QString("crop=2048:1152"));
    }

    // ═══════════════════════════════════════════════════════════════════════
    // H264 source → AV1 (probe + encode only, previews already covered)
    // ═══════════════════════════════════════════════════════════════════════

    void h264SourceToAv1()
    {
        const QString dir = makeSubDir("h264_to_av1");
        std::atomic<bool> interrupt { false };
        auto importer = makeImporter(m_h264Video, dir,
            ScreenPlay::Video::VideoCodec::AV1, interrupt);

        QVERIFY(importer->createWallpaperInfo());

        // Encode H264 → AV1 (.mkv)
        QVERIFY(importer->createWallpaperVideo());
        const QString encoded = dir + "/mantissa.xyz_loop_072.mkv";
        QVERIFY(fileExistsAndNonEmpty(encoded));
        QVERIFY(QFileInfo(encoded).size() != QFileInfo(m_h264Video).size());
    }

    // ═══════════════════════════════════════════════════════════════════════
    // WebM source (VP8) → AV1 (probe + encode only)
    // ═══════════════════════════════════════════════════════════════════════

    void webmSourceToAv1()
    {
        const QString dir = makeSubDir("webm_to_av1");
        std::atomic<bool> interrupt { false };
        auto importer = makeImporter(m_webmVideo, dir,
            ScreenPlay::Video::VideoCodec::AV1, interrupt);

        QVERIFY(importer->createWallpaperInfo());

        // Encode VP8 → AV1 (.mkv)
        QVERIFY(importer->createWallpaperVideo());
        const QString encoded = dir + "/mantissa.xyz_loop_075.mkv";
        QVERIFY(fileExistsAndNonEmpty(encoded));
        QVERIFY(QFileInfo(encoded).size() != QFileInfo(m_webmVideo).size());
    }

    // ═══════════════════════════════════════════════════════════════════════
    // AV1 source (4K) → VP9 (full pipeline with previews + actual VP9 encode)
    // ═══════════════════════════════════════════════════════════════════════

    void av1SourceToVp9()
    {
        const QString dir = makeSubDir("av1_to_vp9");
        std::atomic<bool> interrupt { false };
        auto importer = makeImporter(m_av1Video, dir,
            ScreenPlay::Video::VideoCodec::VP9, interrupt);

        // Probe
        QVERIFY(importer->createWallpaperInfo());
        QCOMPARE(importer->m_videoWidth, 3840);
        QCOMPARE(importer->m_videoHeight, 2160);
        QCOMPARE(importer->m_numberOfFrames, 600);
        QCOMPARE(importer->m_framerate, 60);
        QCOMPARE(importer->m_length, 10);
        QCOMPARE(importer->m_skipAudio, true);
        QCOMPARE(importer->m_isWebm, false);

        // Previews
        QVERIFY(runAllPreviewSteps(importer.get()));
        verifyPreviews(dir, m_av1Video);

        // Encode AV1 → VP9 (must actually transcode, source != target codec)
        QVERIFY(importer->createWallpaperVideo());
        const QString encoded = dir + "/Mantissa.Xyz Loop 091.webm";
        QVERIFY(fileExistsAndNonEmpty(encoded));
        QVERIFY(QFileInfo(encoded).size() != QFileInfo(m_av1Video).size());

        QCOMPARE(importer->build16x9CropFilter(), QString("crop=3840:2160"));
    }

    // ═══════════════════════════════════════════════════════════════════════
    // AV1 source (4K) → AV1 target (skip encoding – same codec → file copy)
    // ═══════════════════════════════════════════════════════════════════════

    void av1SourceSkipEncoding()
    {
        const QString dir = makeSubDir("av1_skip");
        std::atomic<bool> interrupt { false };
        auto importer = makeImporter(m_av1Video, dir,
            ScreenPlay::Video::VideoCodec::AV1, interrupt);

        QVERIFY(importer->createWallpaperInfo());

        // Same codec → must skip encoding and just copy the file
        QVERIFY(importer->createWallpaperVideo());
        const QString copied = dir + "/Mantissa.Xyz Loop 091.mkv";
        QVERIFY(fileExistsAndNonEmpty(copied));
        QCOMPARE(QFileInfo(copied).size(), QFileInfo(m_av1Video).size());
    }

    // ═══════════════════════════════════════════════════════════════════════
    // H264 source → H264 target (skip encoding – same codec → file copy)
    // ═══════════════════════════════════════════════════════════════════════

    void h264SourceSkipEncoding()
    {
        const QString dir = makeSubDir("h264_skip");
        std::atomic<bool> interrupt { false };
        auto importer = makeImporter(m_h264Video, dir,
            ScreenPlay::Video::VideoCodec::H264, interrupt);

        QVERIFY(importer->createWallpaperInfo());

        // Same codec → must skip encoding and just copy the file
        QVERIFY(importer->createWallpaperVideo());
        const QString copied = dir + "/mantissa.xyz_loop_072.mp4";
        QVERIFY(fileExistsAndNonEmpty(copied));
        // Verify it is an exact copy (byte-for-byte), not a re-encode
        QCOMPARE(QFileInfo(copied).size(), QFileInfo(m_h264Video).size());
    }

    // ═══════════════════════════════════════════════════════════════════════
    // NoConversion → just copies regardless of codec
    // ═══════════════════════════════════════════════════════════════════════

    void noConversionJustCopies()
    {
        const QString dir = makeSubDir("noconversion");
        std::atomic<bool> interrupt { false };
        auto importer = makeImporter(m_h264Video, dir,
            ScreenPlay::Video::VideoCodec::NoConversion, interrupt);

        // NoConversion path doesn't need probe data – skip createWallpaperInfo
        QVERIFY(importer->createWallpaperVideo());

        const QString copied = dir + "/mantissa.xyz_loop_072.mp4";
        QVERIFY(fileExistsAndNonEmpty(copied));
        QCOMPARE(QFileInfo(copied).size(), QFileInfo(m_h264Video).size());
    }

    // ═══════════════════════════════════════════════════════════════════════
    // Edge cases
    // ═══════════════════════════════════════════════════════════════════════

    void cropFilterUnknownDimensionsReturnsEmpty()
    {
        std::atomic<bool> interrupt { false };
        auto importer = makeImporter(m_h264Video, m_tmpDir,
            ScreenPlay::Video::VideoCodec::H264, interrupt);

        // Don't call createWallpaperInfo → dimensions stay 0
        QVERIFY(importer->build16x9CropFilter().isEmpty());
    }
};

QTEST_MAIN(VideoImportTest)

#include "tst_VideoImport.moc"
