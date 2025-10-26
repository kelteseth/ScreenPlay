// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QGuiApplication>
#include <QtTest>
#include <memory>

#include "ScreenPlay/create.h"

#include "CMakeVariables.h"

class ScreenPlayTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase()
    {
        qInfo() << "Initializing unit test case...";
        
        // For unit tests, we just need a simple Create instance
        // No QML engine or GUI needed
        m_create = std::make_shared<ScreenPlay::Create>();
        QVERIFY(m_create);
        
        qInfo() << "Unit test case initialized successfully";
    }

    void cleanupTestCase()
    {
        m_create.reset();
        qInfo() << "Unit test case cleaned up";
    }

    void detect_video_codec_h264();
    void detect_video_codec_vp9();

private:
    std::shared_ptr<ScreenPlay::Create> m_create;
};

void ScreenPlayTest::detect_video_codec_h264()
{
    using namespace ScreenPlay;
    
    const QString h264VideoPath = QString(SCREENPLAY_SOURCE_DIR) + "/Content/wallpaper_video_nebula_h264/mantissa.xyz_loop_072.mp4";
    QVERIFY(QFile::exists(h264VideoPath));
    
    qInfo() << "Testing H.264 codec detection with:" << h264VideoPath;
    
    auto detectedCodec = m_create->detectVideoCodec(h264VideoPath);
    
    // Should detect H264 codec
    QCOMPARE(detectedCodec, Video::VideoCodec::H264);
    
    // H.264 is playable, conversion can be skipped
    QVERIFY(m_create->canSkipConversion(detectedCodec));
    
    qInfo() << "✓ H.264 video correctly detected and marked as playable";
}

void ScreenPlayTest::detect_video_codec_vp9()
{
    using namespace ScreenPlay;
    
    const QString vp9VideoPath = QString(SCREENPLAY_SOURCE_DIR) + "/Content/wallpaper_video_astronaut_vp9/mantissa.xyz_loop_075.webm";
    QVERIFY(QFile::exists(vp9VideoPath));
    
    qInfo() << "Testing VP9 codec detection with:" << vp9VideoPath;
    
    auto detectedCodec = m_create->detectVideoCodec(vp9VideoPath);
    
    // Should detect VP9 codec
    QCOMPARE(detectedCodec, Video::VideoCodec::VP9);
    
    // VP9 is playable, conversion can be skipped
    QVERIFY(m_create->canSkipConversion(detectedCodec));
    
    qInfo() << "✓ VP9 video correctly detected and marked as playable";
}

QTEST_MAIN(ScreenPlayTest)

#include "tst_ScreenPlay.moc"
