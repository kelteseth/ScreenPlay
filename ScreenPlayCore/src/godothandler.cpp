// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#include "ScreenPlayCore/godothandler.h"

#include <QEventLoop>
#include <QFile>
#include <QJsonParseError>
#include <QSettings>
#include <QSysInfo>
#include <QTemporaryDir>
#include <QTextStream>
#include <QUrl>
#include <memory>

#include "CMakeVariables.h"
#include "ScreenPlayCore/archivereader.h"
#include "core/qcoroprocess.h"
#include "qarchivediskextractor.hpp"

namespace ScreenPlay {

GodotHandler::GodotHandler(QObject* parent)
    : QObject { parent }
{
}

/*!
  \brief Opens the Godot editor for a given project path.
*/
bool GodotHandler::openGodotEditor(const QString& contentPath, const QString& godotEditorExecutablePath) const
{
    const QList<QString> godotCmd = { "--editor", "--path", m_util.toLocal(contentPath) };
    QProcess process;
    process.setProgram(godotEditorExecutablePath);
    process.setArguments(godotCmd);
    return process.startDetached();
}

/*!
  \brief Checks if the wallpaper type is Godot-based
  \param type The InstalledType to check
  \return true if the wallpaper is Godot-based, false otherwise
*/
bool GodotHandler::isGodotWallpaper(const ScreenPlay::ContentTypes::InstalledType type) const
{
    return type == ScreenPlay::ContentTypes::InstalledType::GodotWallpaper;
}

/*!
  \brief Checks if a Godot project export file exists for the given project and if the Godot version matches.
  \param absolutePath The absolute path to the project directory
  \return true if the export file exists and the Godot version matches, false otherwise
*/
bool GodotHandler::godotProjectExportExists(const QString& absolutePath) const
{
    std::optional<QFileInfo> godotPackageFileOpt = getGodotProjectExportFile(absolutePath);
    if (!godotPackageFileOpt.has_value()) {
        return false;
    }

    QFileInfo godotPackageFile = godotPackageFileOpt.value();
    if (!godotPackageFile.exists()) {
        return false;
    }

    // Check if the Godot version in the zip matches the current version
    bool versionMatches = checkGodotVersionInZip(godotPackageFile.absoluteFilePath());

    if (!versionMatches) {
        qInfo() << "Godot export exists but version mismatch detected for:" << absolutePath;
    }

    return versionMatches;
}

/*!
  \brief Exports a Godot project to a package file with version validation.
*/
QCoro::QmlTask GodotHandler::exportGodotProject(const QString& absolutePath, const QString& godotEditorExecutablePath, const bool overwrite)
{
    return QCoro::QmlTask([this, absolutePath, godotEditorExecutablePath, overwrite]() -> QCoro::Task<Result> {
        QString projectPath = m_util.toLocal(absolutePath);

        std::optional<QFileInfo> godotPackageFileOpt = getGodotProjectExportFile(absolutePath);
        if (!godotPackageFileOpt.has_value()) {
            co_return Result { false, {}, "Unable to read project.json or missing version field" };
        }

        QFileInfo godotPackageFile = godotPackageFileOpt.value();
        QString packageFileName = godotPackageFile.fileName();

        if (godotPackageFile.exists()) {
            if (overwrite) {
                if (!QFile::moveToTrash(godotPackageFile.absoluteFilePath())) {
                    co_return Result { false, {}, QString("Unable to delete old export: %1").arg(godotPackageFile.absoluteFilePath()) };
                }
            } else {
                // Skip reexport
                co_return Result { true };
            }
        }

        // Update the project.json file with current Godot version before exporting
        QString projectJsonPath = projectPath + "/project.json";
        std::optional<QJsonObject> projectJsonOpt = m_util.openJsonFileToObject(projectJsonPath);
        if (projectJsonOpt.has_value()) {
            QJsonObject projectJson = projectJsonOpt.value();

            // Update Godot version fields
            const int currentGodotMajor = SCREENPLAY_GODOT_VERSION_MAJOR;
            const int currentGodotMinor = SCREENPLAY_GODOT_VERSION_MINOR;

            projectJson["godotVersionMajor"] = QString::number(currentGodotMajor);
            projectJson["godotVersionMinor"] = QString::number(currentGodotMinor);

            // Write the updated project.json back
            if (!m_util.writeJsonObjectToFile(projectJsonPath, projectJson, true)) {
                qWarning() << "Failed to update project.json with current Godot version";
                // Continue anyway, this is not a fatal error
            } else {
                qInfo() << "Updated project.json with Godot version:" << currentGodotMajor << "." << currentGodotMinor;
            }
        } else {
            qWarning() << "Failed to read project.json for version update, continuing with export";
            // Continue anyway, this is not a fatal error
        }

        // Determine the desired Godot export preset based on the detected OS
        const QString kernelType = QSysInfo::kernelType().toLower();
        QStringList presetCandidates;

        if (kernelType.startsWith(QStringLiteral("win"))) {
            presetCandidates << QStringLiteral("Windows Desktop") << QStringLiteral("Windows");
        } else if (kernelType == QStringLiteral("darwin")) {
            presetCandidates << QStringLiteral("macOS") << QStringLiteral("Mac OSX");
        } else if (kernelType == QStringLiteral("linux")) {
            presetCandidates << QStringLiteral("Linux") << QStringLiteral("Linux/X11");
        } else {
            qWarning() << "Unsupported kernel type for Godot export" << kernelType;
            co_return Result { false, {}, tr("Unsupported platform for Godot export") };
        }

        // Inspect available presets to ensure the requested one exists
        QStringList availablePresets;
        const QString exportPresetsPath = QDir(projectPath).filePath(QStringLiteral("export_presets.cfg"));
        if (QFile::exists(exportPresetsPath)) {
            QSettings presetSettings(exportPresetsPath, QSettings::IniFormat);
            const QStringList presetGroups = presetSettings.childGroups();
            for (const QString& group : presetGroups) {
                if (!group.startsWith(QStringLiteral("preset.")))
                    continue;
                presetSettings.beginGroup(group);
                const QString presetName = presetSettings.value(QStringLiteral("name")).toString();
                if (!presetName.isEmpty())
                    availablePresets << presetName;
                presetSettings.endGroup();
            }
        } else {
            qWarning() << "Missing export_presets.cfg at" << exportPresetsPath;
        }

        auto resolvePreset = [&availablePresets](const QStringList& candidates) -> QString {
            for (const QString& candidate : candidates) {
                for (const QString& preset : availablePresets) {
                    if (preset.compare(candidate, Qt::CaseInsensitive) == 0)
                        return preset;
                }
            }
            if (!availablePresets.isEmpty()) {
                qWarning() << "Requested Godot export preset not found; using"
                           << availablePresets.first();
                return availablePresets.first();
            }
            return candidates.isEmpty() ? QString() : candidates.first();
        };

        const QString exportPresetName = resolvePreset(presetCandidates);
        if (exportPresetName.isEmpty()) {
            qWarning() << "No suitable Godot export preset resolved" << presetCandidates;
            co_return Result { false, {}, tr("No usable Godot export preset found") };
        }

        // Choose the Godot export preset based on the detected OS or fallback
        const QList<QString> godotCmd
            = { "--export-pack", "--headless", exportPresetName, packageFileName };

        QProcess process;
        process.setWorkingDirectory(projectPath);
        process.setProgram(godotEditorExecutablePath);
        process.setArguments(godotCmd);
        using namespace QCoro;
        auto coro_process = qCoro(process);
        qInfo() << "Start" << process.program() << " " << process.arguments() << process.workingDirectory();
        co_await coro_process.start();
        co_await coro_process.waitForFinished();

        // Capture the standard output and error
        QString stdoutString = process.readAllStandardOutput();
        QString stderrString = process.readAllStandardError();

        // If you want to print the output to the console:
        if (!stdoutString.isEmpty())
            qDebug() << "Output:" << stdoutString;
        if (!stderrString.isEmpty())
            qDebug() << "Error:" << stderrString;

        // Check for errors
        if (process.exitStatus() != QProcess::NormalExit || process.exitCode() != 0) {
            QString errorMessage = tr("Failed to export Godot project. Error: %1").arg(process.errorString());
            qCritical() << errorMessage;
            co_return Result { false, {}, errorMessage };
        }

        // Check if the project.zip file was created
        QString zipPath = QDir(projectPath).filePath(packageFileName);
        if (!QFile::exists(zipPath)) {
            qCritical() << "Expected export file (" << packageFileName << ") was not created.";
            co_return Result { false };
        }

        // Optional: Verify if the .zip file is valid
        //     (A complete verification would involve extracting the file and checking its contents,
        //     but for simplicity, we're just checking its size here)
        QFileInfo zipInfo(zipPath);
        if (zipInfo.size() <= 0) {
            qCritical() << "The exported " << packageFileName << " file seems to be invalid.";
            co_return Result { false };
        }
        qInfo() << "exportGodotProject END";
        co_return Result { true };
    }());
}

/*!
  \brief Helper function that parses the project.json and returns a QFileInfo for the Godot export package.
  Returns std::nullopt if the project.json cannot be read or parsed.
*/
std::optional<QFileInfo> GodotHandler::getGodotProjectExportFile(const QString& absolutePath) const
{
    QString projectPath = m_util.toLocal(absolutePath);
    std::optional<QJsonObject> projectOpt = m_util.openJsonFileToObject(projectPath + "/project.json");
    if (!projectOpt.has_value()) {
        return std::nullopt;
    }

    QJsonObject projectJson = projectOpt.value();
    if (!projectJson.contains("version")) {
        return std::nullopt;
    }

    const quint64 version = projectJson.value("version").toInt();
    const QString packageFileName = QString("project-v%1.zip").arg(version);
    return QFileInfo(projectPath + "/" + packageFileName);
}

/*!
  \brief Reads the project.json file from inside a zip archive and returns it as a QJsonObject.
  Returns std::nullopt if the project.json cannot be read or parsed from the zip.
*/
std::optional<QJsonObject> GodotHandler::readProjectJsonFromZip(const QString& zipFilePath) const
{
    ArchiveReader reader;

    auto openResult = reader.openArchive(zipFilePath);
    if (!openResult.has_value()) {
        qWarning() << "Failed to open archive:" << zipFilePath << "-" << openResult.error();
        return std::nullopt;
    }

    auto jsonResult = reader.readJsonFromArchive("project.json");
    if (!jsonResult.has_value()) {
        qWarning() << "Failed to read/parse project.json from archive:" << zipFilePath << "-" << jsonResult.error();
        return std::nullopt;
    }

    return jsonResult.value();
}

/*!
  \brief Checks if the Godot version in the exported zip matches the current build's Godot version.
  Returns true if versions match, false if they don't match or if there's an error reading the zip.
*/
bool GodotHandler::checkGodotVersionInZip(const QString& zipFilePath) const
{
    auto projectJson = readProjectJsonFromZip(zipFilePath);
    if (!projectJson.has_value()) {
        return false;
    }

    const QJsonObject& obj = projectJson.value();

    // Check if the required version fields exist
    if (!obj.contains("godotVersionMajor") || !obj.contains("godotVersionMinor")) {
        qWarning() << "Missing Godot version fields in project.json from zip:" << zipFilePath;
        return false;
    }

    // Get versions from the zip
    const int zipGodotMajor = obj.value("godotVersionMajor").toString().toInt();
    const int zipGodotMinor = obj.value("godotVersionMinor").toString().toInt();

    // Compare with current build's Godot version
    const int currentGodotMajor = SCREENPLAY_GODOT_VERSION_MAJOR;
    const int currentGodotMinor = SCREENPLAY_GODOT_VERSION_MINOR;

    bool versionsMatch = (zipGodotMajor == currentGodotMajor) && (zipGodotMinor == currentGodotMinor);

    if (!versionsMatch) {
        qInfo() << "Godot version mismatch - Zip:" << zipGodotMajor << "." << zipGodotMinor
                << "Current:" << currentGodotMajor << "." << currentGodotMinor;
    }

    return versionsMatch;
}

}

#include "moc_godothandler.cpp"
