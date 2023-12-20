// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#include "ScreenPlayUtil/util.h"
#include "qguiapplication.h"

#include <QDesktopServices>
#include <QFile>
#include <QJsonParseError>
#include <QRandomGenerator>

#include "core/qcoroprocess.h"
/*!
    \module ScreenPlayUtil
    \title ScreenPlayUtil
    \brief Module for ScreenPlayUtil.
*/
/*!
    \namespace ScreenPlay
    \inmodule ScreenPlayUtil
    \brief Namespace for ScreenPlayUtil.
*/

namespace ScreenPlay {

/*!
  \brief Opens a json file (absolute path) and tries to convert it to a QJsonObject.
  Returns std::nullopt when not successful.
*/
std::optional<QJsonObject> Util::openJsonFileToObject(const QString& path)
{
    auto jsonString = openJsonFileToString(path);

    if (!jsonString.has_value()) {
        return std::nullopt;
    }

    QJsonDocument jsonDocument;
    QJsonParseError parseError {};
    jsonDocument = QJsonDocument::fromJson(jsonString->toUtf8(), &parseError);

    if (!(parseError.error == QJsonParseError::NoError)) {
        qWarning() << "Settings Json Parse Error: " << parseError.errorString();
        return std::nullopt;
    }

    return jsonDocument.object();
}

/*!
    \brief Writes a QJsonObject into a given file. It defaults to truncate the file
           or you can set it to append to an existing file.
*/
bool Util::writeJsonObjectToFile(const QString& absoluteFilePath, const QJsonObject& object, bool truncate)
{
    QFile configTmp;
    configTmp.setFileName(absoluteFilePath);
    QIODevice::OpenMode openMode;
    if (truncate) {
        openMode = QIODevice::ReadWrite | QIODevice::Truncate;
    } else {
        openMode = QIODevice::ReadWrite | QIODevice::Append;
    }

    if (!configTmp.open(openMode)) {
        qWarning() << "Could not open out file!" << configTmp.errorString();
        return false;
    }

    QTextStream out(&configTmp);
    out.setEncoding(QStringConverter::Utf8);
    out << QJsonDocument(object).toJson();

    configTmp.close();
    return true;
}

/*!
  \brief Takes ownership of \a obj and \a name. Tries to save into a text file
    with of name.
*/
bool Util::writeSettings(const QJsonObject& obj, const QString& absolutePath)
{
    QFile file { absolutePath };
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Could not open" << absolutePath;
        return false;
    }

    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    QJsonDocument doc(obj);

    out << doc.toJson();

    file.close();
    return true;
}
/*!
  \brief Tries to save into a text file with absolute path.
*/
bool Util::writeFile(const QString& text, const QString& absolutePath)
{
    QFile file { absolutePath };
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Could not open" << absolutePath;
        return false;
    }

    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    out << text;

    file.close();
    return true;
}
/*!
  \brief Tries to save into a text file with absolute path.
*/
bool Util::writeFileFromQrc(const QString& qrcPath, const QString& absolutePath)
{

    QFile file { absolutePath };
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Could not open" << absolutePath;
        return false;
    }

    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);

    QFile qrc(qrcPath);
    qrc.open(QIODevice::ReadOnly);

    QTextStream in(&qrc);
    // Read line by line to avoid CLRF/LF issues
    while (!in.atEnd()) {
        out << in.readLine() << "\n";
    }

    qrc.close();
    file.close();
    return true;
}
/*!
  \brief  Opens a json file (absolute path) and tries to convert it to a QString.
  Returns std::nullopt when not successful.
*/
std::optional<QString> Util::openJsonFileToString(const QString& path)
{
    QFile file;
    file.setFileName(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Unable to open file: " << path << file.errorString();
        return std::nullopt;
    }
    QString fileContent = file.readAll();
    file.flush();
    file.close();

    return { fileContent };
}

/*!
  \brief Generates a (non secure) random string with the default length of 32. Can contain:
  \list
    \li A-Z
    \li a-z
    \li 0-9
   \endlist
*/
QString Util::generateRandomString(quint32 length)
{
    const QString possibleCharacters {
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"
    };
    const auto radomGen = QRandomGenerator::system();

    QString randomString;
    for (quint32 i = 0; i < length; ++i) {
        const int index = radomGen->bounded(possibleCharacters.length());
        const QChar nextChar = possibleCharacters.at(index);
        randomString.append(nextChar);
    }
    return randomString;
}

/*!
  \brief Return .exe on windows otherwise empty string.
*/
QString Util::executableBinEnding()
{
#ifdef Q_OS_WIN
    return ".exe";
#endif
    return "";
}

/*!
  \brief Return .exe on windows, .app on osx otherwise empty string.
*/
QString Util::executableAppEnding()
{
#ifdef Q_OS_WIN
    return ".exe";
#endif
#ifdef Q_OS_MACOS
    return ".app";
#endif
    return "";
}

/*!
  \brief Parses a version from a given QString. The QString must be looke like this:
  1.0.0 - Major.Minor.Patch. A fixed position is used for parsing (at 0,2,4).
  Return std::nullopt when not successful.
*/
std::optional<QVersionNumber> Util::getVersionNumberFromString(const QString& str)
{
    // Must be: Major.Minor.Patch
    bool okMajor { false };
    bool okMinor { false };
    bool okPatch { false };

    int major = QString(str.at(0)).toInt(&okMajor);
    int minor = QString(str.at(2)).toInt(&okMinor);
    int patch = QString(str.at(4)).toInt(&okPatch);

    if (okMajor && okMinor && okPatch) {
        return std::nullopt;
    }

    return QVersionNumber(major, minor, patch);
}

/*!
  \brief Parses a QByteArray to a QJsonObject. If returns and std::nullopt on failure.
*/
std::optional<QJsonObject> Util::parseQByteArrayToQJsonObject(const QByteArray& byteArray)
{
    QJsonObject obj;
    QJsonParseError err {};
    QJsonDocument doc = QJsonDocument::fromJson(byteArray, &err);

    if (err.error == QJsonParseError::NoError) {
        obj = doc.object();
        return { obj };
    }

    return std::nullopt;
}

/*!
    \brief Helper function to append a QStringList into a QString with a space between the items.
*/
QString Util::toString(const QStringList& list) const
{
    QString out;
    for (const auto& string : list) {
        out += " " + string;
    }
    return out;
}

/*!
  \brief Util function that converts a QVector of Strings into a QJsonArray.
*/
QJsonArray Util::fillArray(const QVector<QString>& items)
{
    QJsonArray array;
    for (const QString& item : items) {
        array.append(item);
    }
    return array;
}

/*!
  \brief Maps the Search type to an installed type. Used for filtering the installed content.
*/
ScreenPlay::ContentTypes::SearchType Util::getSearchTypeFromInstalledType(const ScreenPlay::ContentTypes::InstalledType type)
{
    using namespace ScreenPlay;
    switch (type) {
    case ContentTypes::InstalledType::GodotWallpaper:
    case ContentTypes::InstalledType::HTMLWallpaper:
    case ContentTypes::InstalledType::QMLWallpaper:
    case ContentTypes::InstalledType::GifWallpaper:
    case ContentTypes::InstalledType::WebsiteWallpaper:
        return ContentTypes::SearchType::Scene;
    case ContentTypes::InstalledType::VideoWallpaper:
        return ContentTypes::SearchType::Wallpaper;
    case ContentTypes::InstalledType::HTMLWidget:
    case ContentTypes::InstalledType::QMLWidget:
        return ContentTypes::SearchType::Widget;
    case ContentTypes::InstalledType::Unknown:
    default:
        return ContentTypes::SearchType::All;
    }
}

/*!
    \brief Maps the installed type from a QString to an enum. Used for parsing the project.json.
*/
std::optional<ScreenPlay::ContentTypes::InstalledType> Util::getInstalledTypeFromString(const QString& type)
{
    using namespace ScreenPlay;
    if (type.endsWith("Wallpaper", Qt::CaseInsensitive)) {
        if (type.startsWith("video", Qt::CaseInsensitive)) {
            return ContentTypes::InstalledType::VideoWallpaper;
        }
        if (type.startsWith("qml", Qt::CaseInsensitive)) {
            return ContentTypes::InstalledType::QMLWallpaper;
        }
        if (type.startsWith("html", Qt::CaseInsensitive)) {
            return ContentTypes::InstalledType::HTMLWallpaper;
        }
        if (type.startsWith("godot", Qt::CaseInsensitive)) {
            return ContentTypes::InstalledType::GodotWallpaper;
        }
        if (type.startsWith("website", Qt::CaseInsensitive)) {
            return ContentTypes::InstalledType::WebsiteWallpaper;
        }
        if (type.startsWith("gif", Qt::CaseInsensitive)) {
            return ContentTypes::InstalledType::GifWallpaper;
        }
    }

    if (type.endsWith("Widget", Qt::CaseInsensitive)) {
        if (type.startsWith("qml", Qt::CaseInsensitive)) {
            return ContentTypes::InstalledType::QMLWidget;
        }
        if (type.startsWith("html", Qt::CaseInsensitive)) {
            return ContentTypes::InstalledType::HTMLWidget;
        }
    }

    return std::nullopt;
}

/*!
    \brief Maps the video codec type from a QString to an enum. Used for parsing the project.json.
*/
std::optional<ScreenPlay::Video::VideoCodec> Util::getVideoCodecFromString(const QString& type)
{
    if (type.isEmpty())
        return std::nullopt;

    if (type.contains("vp8", Qt::CaseInsensitive))
        return ScreenPlay::Video::VideoCodec::VP8;

    if (type.contains("vp9", Qt::CaseInsensitive))
        return ScreenPlay::Video::VideoCodec::VP9;

    if (type.contains("av1", Qt::CaseInsensitive))
        return ScreenPlay::Video::VideoCodec::AV1;

    if (type.contains("h264", Qt::CaseInsensitive))
        return ScreenPlay::Video::VideoCodec::H264;

    if (type.contains("h265", Qt::CaseInsensitive))
        return ScreenPlay::Video::VideoCodec::H264;

    return std::nullopt;
}

/*!
    \brief Converts the given \a url string to a local file path.
*/
QString Util::toLocal(const QString& url) const
{
    return QUrl(url).toLocalFile();
}

/*!
    \brief Returns a list of available wallpaper types like videoWallpaper.
*/
QStringList Util::getAvailableWallpaper() const
{
    return {
        "qmlWallpaper",
        "htmlWallpaper",
        "videoWallpaper",
        "godotWallpaper",
        "gifWallpaper",
        "websiteWallpaper"
    };
}

/*!
    \brief Returns a list of available widget types like qmlWidget.
*/
QStringList Util::getAvailableWidgets() const
{
    return {
        "qmlWidget",
        "htmlWidget",
    };
}

/*!
    \brief Returns a combined list of available widgets and wallpaper types.
*/
QStringList Util::getAvailableTypes() const
{
    return { getAvailableWallpaper() + getAvailableWidgets() };
}

/*!
    \brief Returns true of the given type is a wallpaper.
*/
bool Util::isWallpaper(const ScreenPlay::ContentTypes::InstalledType type) const
{

    using namespace ScreenPlay;
    return (type == ContentTypes::InstalledType::VideoWallpaper
        || type == ContentTypes::InstalledType::QMLWallpaper
        || type == ContentTypes::InstalledType::HTMLWallpaper
        || type == ContentTypes::InstalledType::GifWallpaper
        || type == ContentTypes::InstalledType::WebsiteWallpaper
        || type == ContentTypes::InstalledType::GodotWallpaper);
}

/*!
    \brief Returns true of the given type is a widget.
*/
bool Util::isWidget(const ScreenPlay::ContentTypes::InstalledType type) const
{
    using namespace ScreenPlay;

    return (type == ContentTypes::InstalledType::QMLWidget || type == ContentTypes::InstalledType::HTMLWidget);
}
/*!
    \brief Returns true of the given type is a isScene.
*/
bool Util::isScene(const ScreenPlay::ContentTypes::InstalledType type) const
{
    using namespace ScreenPlay;

    return (type == ContentTypes::InstalledType::HTMLWallpaper
        || type == ContentTypes::InstalledType::QMLWallpaper
        || type == ContentTypes::InstalledType::WebsiteWallpaper
        || type == ContentTypes::InstalledType::GodotWallpaper);
}
/*!
    \brief Returns true of the given type is a isVideo.
*/
bool Util::isVideo(const ScreenPlay::ContentTypes::InstalledType type) const
{
    using namespace ScreenPlay;

    return (type == ContentTypes::InstalledType::VideoWallpaper
        || type == ContentTypes::InstalledType::GifWallpaper);
}

/*!
    \brief HTML video fillModes to be used in the QWebEngine video player.
           See https://developer.mozilla.org/en-US/docs/Web/CSS/object-fit
*/
QStringList Util::getAvailableFillModes() const
{
    return { "stretch", "fill", "contain", "cover", "scale-down" };
}

/*!
  \brief parseIntList parses a list of string separated with a comma
        "1,2,3". IMPORTANT: No trailing comma!
 */
std::optional<QVector<int>> Util::parseStringToIntegerList(const QString string) const
{
    if (string.isEmpty())
        return {};

    QVector<int> list;
    QStringList stringList = string.split(",");
    for (const auto& item : stringList) {
        bool ok = false;
        int val = item.toInt(&ok);
        if (!ok) {
            qFatal("Could not parse item to Integer!");
        } else {
            list.append(val);
        }
    }

    return list;
}

float Util::roundDecimalPlaces(const float number) const
{
    float big = number * 100.0;
    return std::ceil(big * 0.01);
}
/*!
  \brief Copies the given string to the clipboard.
*/
void Util::copyToClipboard(const QString& text) const
{
    auto* clipboard = QGuiApplication::clipboard();
    clipboard->setText(text);
}

/*!
  \brief Opens a native folder window on the given path. Windows and Mac only for now!
*/
void Util::openFolderInExplorer(const QString& url) const
{
    const QString path = QUrl::fromUserInput(url).toLocalFile();

    // QDesktopServices can hang on Windows
    if (QSysInfo::productType() == "windows") {
        QProcess explorer;
        explorer.setProgram("explorer.exe");
        // When we have space in the path like
        // C:\Program Files (x86)\Steam\...
        // we cannot set the path as an argument. But we can set the working it
        // to the wanted path and open the current path via the dot.
        explorer.setWorkingDirectory(QDir::toNativeSeparators(path));
        explorer.setArguments({ "." });
        explorer.startDetached();
        return;
    }

    QDesktopServices::openUrl(QUrl::fromLocalFile(path));
}

bool Util::openGodotEditor(QString contentPath, QString godotEditorExecutablePath) const
{
    const QList<QString> godotCmd = { "--editor", "--path", toLocal(contentPath) };
    QProcess process;
    process.setProgram(godotEditorExecutablePath);
    process.setArguments(godotCmd);
    return process.startDetached();
}

/*!
  \brief Loads all content of the legal folder in the qrc into a property string of this class.
  allLicenseLoaded is emited when loading is finished.
*/
void Util::requestAllLicenses()
{
    if (m_requestAllLicensesFuture.isRunning())
        return;

    m_requestAllLicensesFuture = QtConcurrent::run([this]() {
        QString tmp;
        QFile file;
        QTextStream out(&file);

        file.setFileName(":/qml/ScreenPlayApp/legal/Font Awesome Free License.txt");
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        tmp += out.readAll();
        file.close();

        file.setFileName(":/qml/ScreenPlayApp/legal/gpl-3.0.txt");
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        tmp += out.readAll();
        file.close();

        file.setFileName(":/qml/ScreenPlayApp/legal/gpl-3.0.txt");
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        tmp += out.readAll();
        file.close();

        file.setFileName(":/qml/ScreenPlayApp/legal/OFL.txt");
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        tmp += out.readAll();
        file.close();

        file.setFileName(":/qml/ScreenPlayApp/legal/OpenSSL.txt");
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        tmp += out.readAll();
        file.close();

        file.setFileName(":/qml/ScreenPlayApp/legal/Qt LGPLv3.txt");
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        tmp += out.readAll();
        file.close();

        emit this->allLicenseLoaded(tmp);
    });
}

/*!
  \brief Loads all dataprotection of the legal folder in the qrc into a property string of this class.
  allDataProtectionLoaded is emited when loading is finished.
*/
void Util::requestDataProtection()
{
    QString tmp;
    QFile file;
    QTextStream out(&file);

    file.setFileName(":/qml/ScreenPlayApp/legal/DataProtection.txt");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    tmp += out.readAll();
    file.close();

    emit this->allDataProtectionLoaded(tmp);
}

bool Util::fileExists(const QString& filePath) const
{
    const QFileInfo file(toLocal(filePath));
    return file.isFile();
}

/*!
  \brief Takes reference to \a obj. If the copy of the thumbnail is successful,
  it adds the corresponding settings entry to the json object reference.
*/
bool Util::copyPreviewThumbnail(QJsonObject& obj, const QString& previewThumbnail, const QString& destination)
{
    const QUrl previewThumbnailUrl { previewThumbnail };
    const QFileInfo previewImageFile(previewThumbnailUrl.toString());
    const QString destinationFilePath = destination + "/" + previewImageFile.fileName();

    if (!previewThumbnail.isEmpty()) {
        if (!QFile::copy(previewThumbnailUrl.toLocalFile(), destinationFilePath)) {
            qDebug() << "Could not copy" << previewThumbnailUrl.toLocalFile() << " to " << destinationFilePath;
            return false;
        }
    }

    obj.insert("previewThumbnail", previewImageFile.fileName());
    obj.insert("preview", previewImageFile.fileName());

    return true;
}

QCoro::QmlTask Util::exportGodotProject(const QString& absolutePath, const QString& godotEditorExecutablePath)
{
    return QCoro::QmlTask([this, absolutePath, godotEditorExecutablePath]() -> QCoro::Task<Result> {
        QString projectPath = toLocal(absolutePath);
        std::optional<QJsonObject> projectOpt = openJsonFileToObject(projectPath + "/project.json");
        QJsonObject projectJson;
        if (!projectOpt.has_value()) {
            co_return Result { false };
        }
        projectJson = projectOpt.value();
        if (!projectJson.contains("version"))
            co_return Result { false };

        const quint64 version = projectJson.value("version").toInt();
        const QString packageFileName = QString("project-v%1.zip").arg(version);
        QFileInfo godotPackageFile(projectPath + "/" + packageFileName);
        // Skip reexport
        if (godotPackageFile.exists())
            co_return Result { true };

        qInfo() << "No suitable version found for Godot package" << packageFileName << " at" << godotPackageFile.absoluteFilePath() << " exporting a new pck as zip.";

        // Prepare the Godot export command
        const QList<QString>
            godotCmd
            = { "--export-pack", "--headless", "Windows Desktop", packageFileName };

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
}

#include "moc_util.cpp"
