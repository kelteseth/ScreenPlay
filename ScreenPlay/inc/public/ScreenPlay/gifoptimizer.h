// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#pragma once

#include <QObject>
#include <QProcess>
#include <QString>
#include <QStringList>
#include <atomic>
#include <expected>
#include <memory>
#include <vector>

namespace ScreenPlay {

struct GifOptimizeResult {
    QString outputPath;
    qint64 fileSize;
    int fps;
    int width;
    int height;
    int maxColors;
    int passUsed; // 1-based tier index that succeeded
};

struct GifOptimizeError {
    QString message;
};

class GifOptimizer : public QObject {
    Q_OBJECT

public:
    struct Settings {
        int maxSizeBytes;
        double maxDurationSecs;
        Settings()
            : maxSizeBytes(1024 * 1024) // Steam's 1 MB limit
            , maxDurationSecs(0.0) // 0 = use full input duration
        {
        }
    };

    explicit GifOptimizer(const QString& ffmpegPath,
        std::atomic<bool>& interrupt,
        QObject* parent = nullptr);

    [[nodiscard]] std::expected<GifOptimizeResult, GifOptimizeError> optimize(
        const QString& inputPath,
        const QString& outputPath,
        Settings settings = {});

signals:
    void processOutput(QString text);

private:
    struct ResolutionTier {
        int width;
        int height;
        int maxColors;
        QString statsMode;
        int bayerScale;
    };

    static constexpr int MinFps = 12;
    static constexpr int MaxFps = 30;

    static std::vector<ResolutionTier> resolutionTiers();

    std::expected<qint64, QString> tryEncode(
        const QString& inputPath,
        const QString& outputPath,
        const ResolutionTier& tier,
        int fps,
        double maxDurationSecs);

    std::expected<GifOptimizeResult, GifOptimizeError> refineFps(
        const QString& inputPath,
        const QString& outputPath,
        const ResolutionTier& tier,
        int tierIndex,
        int loFps,
        int hiFps,
        const Settings& settings);

    QString runProcess(const QStringList& args);

    QString m_ffmpegPath;
    std::atomic<bool>& m_interrupt;
};

} // namespace ScreenPlay
