// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#include "ScreenPlay/gifoptimizer.h"

#include <QCoreApplication>
#include <QFile>
#include <QFileInfo>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(gifOptimizer, "screenplay.gifoptimizer")

namespace ScreenPlay {

/*!
 * \class GifOptimizer
 * \inmodule ScreenPlay
 * \brief Converts a video to an optimised animated GIF that fits under a byte-size limit.
 *
 * Designed to take the 5-second preview.webm (854x480, 16:9) as input and produce
 * a GIF suitable for Steam's 1 MB AddItemPreviewFile limit.
 *
 * Uses a two-phase best-fit strategy:
 * \list
 *   \li \b{Phase 1 — Tier scan}: finds the highest-resolution tier that can fit
 *       the budget at minimum fps.
 *   \li \b{Phase 2 — FPS maximise}: binary-searches fps upward within the winning
 *       tier so the output uses as much of the budget as possible.
 * \endlist
 *
 * Every tier applies all available GIF optimizations:
 *
 * \list
 *   \li Two-pass palette generation (palettegen + paletteuse)
 *   \li Bayer dithering — regular patterns that LZW compresses far better than
 *       error-diffusion (Floyd-Steinberg)
 *   \li stats_mode=diff — palette tuned to inter-frame differences (animation-aware)
 *   \li diff_mode=rectangle — only encode changed rectangular regions per frame
 *   \li Per-frame palette recomputation (new=1) for colour accuracy
 *   \li max_colors reduction at lower tiers
 *   \li Lanczos downscaling — smooth gradients that compress well
 * \endlist
 */
GifOptimizer::GifOptimizer(const QString& ffmpegPath,
    std::atomic<bool>& interrupt,
    QObject* parent)
    : QObject(parent)
    , m_ffmpegPath(ffmpegPath)
    , m_interrupt(interrupt)
{
}

/*!
 * \brief Returns resolution tiers from highest to lowest quality.
 *
 * Each tier defines the spatial quality (resolution, palette, dithering).
 * FPS is \e not part of the tier — it is binary-searched separately in
 * Phase 2 to maximise temporal quality within the byte budget.
 *
 * All dimensions are exact 16:9 so there is no aspect-ratio distortion
 * when the input is a 16:9 preview video (854x480).
 */
std::vector<GifOptimizer::ResolutionTier> GifOptimizer::resolutionTiers()
{
    return {
        // Tier 0 — Highest: close to source resolution, fine dither
        { .width = 640, .height = 360, .maxColors = 256, .statsMode = "diff", .bayerScale = 1 },

        // Tier 1 — High: moderate downscale
        { .width = 480, .height = 270, .maxColors = 256, .statsMode = "diff", .bayerScale = 2 },

        // Tier 2 — Medium
        { .width = 400, .height = 225, .maxColors = 256, .statsMode = "diff", .bayerScale = 2 },

        // Tier 3 — Reduced palette
        { .width = 320, .height = 180, .maxColors = 192, .statsMode = "diff", .bayerScale = 3 },

        // Tier 4 — Low
        { .width = 256, .height = 144, .maxColors = 128, .statsMode = "single", .bayerScale = 4 },

        // Tier 5 — Minimum: guaranteed small
        { .width = 192, .height = 108, .maxColors = 96, .statsMode = "single", .bayerScale = 5 },
    };
}

/*!
 * \brief Two-phase best-fit optimiser — fps-first strategy.
 *
 * Smooth animation matters more than resolution for a small preview GIF,
 * so the algorithm guarantees at least \c MinFps (12) before considering
 * resolution.
 *
 * \b{Phase 1 — Tier scan:} walks tiers from highest to lowest resolution
 * and probes each at \c MinFps (12fps).  The first tier whose encode fits
 * the budget becomes the winner.
 *
 * \b{Phase 2 — FPS maximise:} binary-searches fps from \c MinFps to
 * \c MaxFps (30) within the winning tier to use the remaining budget.
 *
 * \returns the best-quality \c GifOptimizeResult, or a \c GifOptimizeError
 *          if no tier at \c MinFps fits.
 */
std::expected<GifOptimizeResult, GifOptimizeError> GifOptimizer::optimize(
    const QString& inputPath,
    const QString& outputPath,
    Settings settings)
{
    if (!QFile::exists(inputPath)) {
        return std::unexpected(GifOptimizeError {
            QString("Input file does not exist: %1").arg(inputPath) });
    }

    const auto tiers = resolutionTiers();

    // ── Phase 1: find the best resolution tier that fits at MinFps ──
    for (int i = 0; i < static_cast<int>(tiers.size()); ++i) {
        const auto& tier = tiers[i];

        if (m_interrupt)
            return std::unexpected(GifOptimizeError { "Interrupted" });

        emit processOutput(
            QString("Tier %1/%2 probe: %3x%4 @ %5fps, %6 colors")
                .arg(i + 1)
                .arg(tiers.size())
                .arg(tier.width)
                .arg(tier.height)
                .arg(MinFps)
                .arg(tier.maxColors));

        auto probeSize = tryEncode(inputPath, outputPath, tier,
            MinFps, settings.maxDurationSecs);

        if (!probeSize) {
            emit processOutput(
                QString("Tier %1 encode error: %2").arg(i + 1).arg(probeSize.error()));
            continue;
        }

        if (*probeSize > settings.maxSizeBytes) {
            emit processOutput(
                QString("Tier %1: %2 bytes at %3fps already exceeds %4 limit, stepping down")
                    .arg(i + 1)
                    .arg(*probeSize)
                    .arg(MinFps)
                    .arg(settings.maxSizeBytes));
            continue;
        }

        emit processOutput(
            QString("Tier %1 fits at %2fps (%3 bytes). Maximising fps...")
                .arg(i + 1)
                .arg(MinFps)
                .arg(*probeSize));

        // ── Phase 2: binary-search fps upward within this tier ──
        return refineFps(inputPath, outputPath, tier, i,
            MinFps, MaxFps, settings);
    }

    return std::unexpected(GifOptimizeError {
        QString("All %1 resolution tiers exceeded the %2 byte limit even at %3fps")
            .arg(tiers.size())
            .arg(settings.maxSizeBytes)
            .arg(MinFps) });
}

/*!
 * \brief Binary-searches fps in [\a loFps, \a hiFps] to find the highest
 *        frame-rate whose output fits the budget.
 *
 * Invariant: \a loFps is known to fit (or is \c MinFps which was just probed).
 */
std::expected<GifOptimizeResult, GifOptimizeError> GifOptimizer::refineFps(
    const QString& inputPath,
    const QString& outputPath,
    const ResolutionTier& tier,
    int tierIndex,
    int loFps,
    int hiFps,
    const Settings& settings)
{
    int bestFps = loFps;
    qint64 bestSize = 0;

    // Quick check: does MaxFps already fit? Then skip the search entirely.
    {
        auto hiResult = tryEncode(inputPath, outputPath, tier,
            hiFps, settings.maxDurationSecs);
        if (hiResult && *hiResult <= settings.maxSizeBytes) {
            emit processOutput(
                QString("Max fps %1 fits (%2 bytes) — using maximum quality")
                    .arg(hiFps)
                    .arg(*hiResult));
            return GifOptimizeResult {
                .outputPath = outputPath,
                .fileSize = *hiResult,
                .fps = hiFps,
                .width = tier.width,
                .height = tier.height,
                .maxColors = tier.maxColors,
                .passUsed = tierIndex + 1
            };
        }
    }

    // Binary search between loFps and hiFps
    int lo = loFps;
    int hi = hiFps;

    while (lo <= hi) {
        if (m_interrupt)
            return std::unexpected(GifOptimizeError { "Interrupted" });

        const int mid = lo + (hi - lo) / 2;

        emit processOutput(
            QString("FPS search: trying %1fps [%2..%3]").arg(mid).arg(lo).arg(hi));

        auto result = tryEncode(inputPath, outputPath, tier,
            mid, settings.maxDurationSecs);

        if (!result) {
            // Encode failed at this fps — treat as too large
            hi = mid - 1;
            continue;
        }

        if (*result <= settings.maxSizeBytes) {
            bestFps = mid;
            bestSize = *result;
            lo = mid + 1; // try higher fps
        } else {
            hi = mid - 1; // too large, try lower fps
        }
    }

    // The last encode during the search may have been at a different fps.
    // Always re-encode at bestFps to guarantee the output file is correct.
    {
        auto finalResult = tryEncode(inputPath, outputPath, tier,
            bestFps, settings.maxDurationSecs);
        if (finalResult)
            bestSize = *finalResult;
    }

    if (bestSize <= 0) {
        return std::unexpected(GifOptimizeError {
            QString("FPS refinement failed at tier %1").arg(tierIndex + 1) });
    }

    emit processOutput(
        QString("Best: %1x%2 @ %3fps, %4 bytes (tier %5)")
            .arg(tier.width)
            .arg(tier.height)
            .arg(bestFps)
            .arg(bestSize)
            .arg(tierIndex + 1));

    return GifOptimizeResult {
        .outputPath = outputPath,
        .fileSize = bestSize,
        .fps = bestFps,
        .width = tier.width,
        .height = tier.height,
        .maxColors = tier.maxColors,
        .passUsed = tierIndex + 1
    };
}

/*!
 * \brief Runs a single two-pass ffmpeg GIF encode at the given \a tier settings.
 *
 * The filter_complex pipeline in detail:
 * \list
 *   \li \c{fps=N} — reduce temporal resolution (fewer frames = smaller file)
 *   \li \c{scale=W:H:flags=lanczos} — high-quality down-scale; smooth gradient
 *       artefacts compress better under LZW than sharp-alias artefacts
 *   \li \c{split} — fork the stream so one copy feeds \c palettegen, the other
 *       feeds \c paletteuse
 *   \li \c{palettegen}:
 *       \list
 *         \li \c{max_colors=N} — smaller palette → smaller LZW dictionary
 *         \li \c{stats_mode=diff} — palette tuned to \e{inter-frame} differences,
 *             ideal for animated content; \c{single} uses one global palette and
 *             can be smaller for visually busy scenes
 *       \endlist
 *   \li \c{paletteuse}:
 *       \list
 *         \li \c{dither=bayer} — regular Bayer-matrix pattern that LZW compresses
 *             dramatically better than error-diffusion (Floyd-Steinberg)
 *         \li \c{bayer_scale=N} — pattern granularity (lower = more dithering,
 *             smaller file but noisier)
 *         \li \c{diff_mode=rectangle} — only encodes the rectangular region that
 *             actually changed between frames; huge saving on partial-motion content
 *         \li \c{new=1} — recompute palette per frame for maximum colour accuracy
 *             in animations
 *       \endlist
 * \endlist
 *
 * \returns the output file size in bytes on success, or an error description.
 */
std::expected<qint64, QString> GifOptimizer::tryEncode(
    const QString& inputPath,
    const QString& outputPath,
    const ResolutionTier& tier,
    int fps,
    double maxDurationSecs)
{
    const QString filterStr = QString(
        "[0:v] fps=%1,scale=%2:%3:flags=lanczos,split [a][b];"
        "[a] palettegen=max_colors=%4:stats_mode=%5 [p];"
        "[b][p] paletteuse=dither=bayer:bayer_scale=%6:diff_mode=rectangle:new=1")
                                  .arg(fps)
                                  .arg(tier.width)
                                  .arg(tier.height)
                                  .arg(tier.maxColors)
                                  .arg(tier.statsMode)
                                  .arg(tier.bayerScale);

    QStringList args;
    args << "-y" << "-stats";

    if (maxDurationSecs > 0.0)
        args << "-t" << QString::number(maxDurationSecs, 'f', 1);

    args << "-i" << inputPath
         << "-filter_complex" << filterStr
         << "-an"
         << "-loop" << "0"
         << outputPath;

    emit processOutput("ffmpeg " + args.join(' '));

    const QString ffmpegOut = runProcess(args);
    emit processOutput(ffmpegOut);

    const QFileInfo out(outputPath);
    if (!out.exists() || out.size() == 0)
        return std::unexpected(QString("ffmpeg produced no output or empty file"));

    return out.size();
}

/*!
 * \brief Runs an ffmpeg process with the given \a args and blocks until it
 *        finishes or \c m_interrupt is set.
 */
QString GifOptimizer::runProcess(const QStringList& args)
{
    auto process = std::make_unique<QProcess>();
    process->setProgram(m_ffmpegPath);
    process->setArguments(args);
    process->setProcessChannelMode(QProcess::MergedChannels);
    process->setWorkingDirectory(QCoreApplication::applicationDirPath());

#ifdef Q_OS_MACOS
    QProcess chmod;
    chmod.setProgram("chmod");
    chmod.setArguments({ "+x", m_ffmpegPath });
    chmod.start();
    chmod.waitForFinished();
#endif

    process->start();

    while (!process->waitForFinished(10)) {
        if (m_interrupt) {
            process->terminate();
            if (!process->waitForFinished(1000))
                process->kill();
            break;
        }
    }

    QString output = process->readAll();
    process->close();
    return output;
}

} // namespace ScreenPlay
