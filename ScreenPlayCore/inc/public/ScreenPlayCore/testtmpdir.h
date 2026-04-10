// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#pragma once

#include <QDateTime>
#include <QDir>
#include <QString>
#include <QStandardPaths>

namespace ScreenPlay {

/*!
 * \brief Creates a unique temporary directory under
 *        <system-tmp>/ScreenPlay/Tests/<datetime>/
 *
 * The directory is created immediately. The caller is responsible for
 * cleanup if desired (tests may want to keep output for inspection).
 *
 * \param suiteName  Optional sub-folder name appended after the datetime stamp (e.g. "VideoImport").
 * \return Absolute path of the created directory, or an empty string on failure.
 */
inline QString createTestTmpDir(const QString& suiteName = {})
{
    const QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd_HH-mm-ss");

    QString path = QDir::tempPath() + "/ScreenPlay/Tests/" + timestamp;
    if (!suiteName.isEmpty())
        path += "/" + suiteName;

    QDir dir;
    if (!dir.mkpath(path))
        return {};

    return QDir(path).absolutePath();
}

}
