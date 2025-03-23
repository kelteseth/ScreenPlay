// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#include "ScreenPlayCore/util.h"

#include <QDesktopServices>
#include <QFile>
#include <QGuiApplication>
#include <QJsonParseError>
#include <QRandomGenerator>

#include "core/qcoroprocess.h"
/*!
    \module ScreenPlayCore
    \title ScreenPlayCore
    \brief Module for ScreenPlayCore.
*/
/*!
    \namespace ScreenPlay
    \inmodule ScreenPlayCore
    \brief Namespace for ScreenPlayCore.
*/

namespace ScreenPlay {

Util::Util(
    QObject* parent)
    : QObject { parent }
{
}

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
    return QOperatingSystemVersion::currentType() == QOperatingSystemVersion::Windows ? ".exe" : "";
}

/*!
  \brief Return .exe on windows, .app on osx otherwise empty string.
*/
QString Util::executableAppEnding()
{

    auto osType = QOperatingSystemVersion::currentType();
    if (osType == QOperatingSystemVersion::Windows) {
        return ".exe";
    }
    if (osType == QOperatingSystemVersion::MacOS) {
        return ".app";
    }
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
    \brief Helper function to be used to print QJsonObjects. USe noquote!
     qDebug().noquote() << Util().toString(projectSettingsListModelProperties);
*/
QString Util::toString(const QJsonObject& obj, int indent)
{
    const auto formatValue = [](const QJsonValue& value) -> QString {
        switch (value.type()) {
        case QJsonValue::Bool:
            return value.toBool() ? "true" : "false";
        case QJsonValue::Double:
            return QString::number(value.toDouble());
        case QJsonValue::String:
            return "\"" + value.toString() + "\"";
        case QJsonValue::Null:
            return "null";
        default:
            return QString();
        }
    };

    const std::function<QString(const QJsonObject&, int)> formatObject =
        [&formatValue, &formatObject](const QJsonObject& obj, int indent) -> QString {
        QString indentStr(indent, ' ');
        QString output = "{\n";

        QJsonObject::const_iterator it;
        for (it = obj.begin(); it != obj.end(); ++it) {
            output += indentStr + "  \"" + it.key() + "\": ";

            switch (it.value().type()) {
            case QJsonValue::Object:
                output += formatObject(it.value().toObject(), indent + 2);
                break;

            case QJsonValue::Array: {
                QJsonArray arr = it.value().toArray();
                output += "[\n";
                for (int i = 0; i < arr.size(); ++i) {
                    output += indentStr + "    ";
                    if (arr[i].isObject()) {
                        output += formatObject(arr[i].toObject(), indent + 4);
                    } else {
                        output += formatValue(arr[i]);
                    }
                    if (i < arr.size() - 1)
                        output += ",";
                    output += "\n";
                }
                output += indentStr + "  ]";
                break;
            }

            default:
                output += formatValue(it.value());
            }

            if (std::next(it) != obj.end())
                output += ",";
            output += "\n";
        }

        output += indentStr + "}";
        return output;
    };

    return formatObject(obj, indent);
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
QString Util::toLocal(const QString& urlString) const
{
    return QUrl(urlString).toLocalFile();
}

QString Util::toLocal(const QUrl& url) const
{
    return url.toLocalFile();
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
    using enum ContentTypes::InstalledType;
    return (type == VideoWallpaper
        || type == QMLWallpaper
        || type == HTMLWallpaper
        || type == GifWallpaper
        || type == WebsiteWallpaper
        || type == GodotWallpaper);
}

/*!
    \brief Returns true of the given type is a widget.
*/
bool Util::isWidget(const ScreenPlay::ContentTypes::InstalledType type) const
{
    using namespace ScreenPlay;
    using enum ContentTypes::InstalledType;
    return (type == QMLWidget || type == HTMLWidget);
}
/*!
    \brief Returns true of the given type is a isScene.
*/
bool Util::isScene(const ScreenPlay::ContentTypes::InstalledType type) const
{
    using namespace ScreenPlay;
    using enum ContentTypes::InstalledType;
    return (type == HTMLWallpaper
        || type == QMLWallpaper
        || type == WebsiteWallpaper
        || type == GodotWallpaper);
}
/*!
    \brief Returns true of the given type is a isVideo.
*/
bool Util::isVideo(const ScreenPlay::ContentTypes::InstalledType type) const
{
    using namespace ScreenPlay;
    using enum ContentTypes::InstalledType;
    return (type == VideoWallpaper
        || type == GifWallpaper);
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
    \brief Checks if the wallpaper type is Qt-based (Video, QML, HTML, GIF, Website)
    \param type The InstalledType to check
    \return true if the wallpaper is Qt-based, false otherwise
*/
bool Util::isQtBasedWallpaper(const ScreenPlay::ContentTypes::InstalledType type) const
{
    using IT = ScreenPlay::ContentTypes::InstalledType;
    return type == IT::VideoWallpaper || type == IT::QMLWallpaper || type == IT::HTMLWallpaper || type == IT::GifWallpaper || type == IT::WebsiteWallpaper;
}

/*!
    \brief Checks if the wallpaper type is Godot-based
    \param type The InstalledType to check
    \return true if the wallpaper is Godot-based, false otherwise
*/
bool Util::isGodotWallpaper(const ScreenPlay::ContentTypes::InstalledType type) const
{
    return type == ScreenPlay::ContentTypes::InstalledType::GodotWallpaper;
}

/*!
    \brief Flattens a hierarchical property structure into a single-level object.

    Takes a JSON object containing categorized properties and returns a new JSON object
    where all properties are at the root level, removing the category hierarchy.

    For example, transforms:
    \code
    {
        "Emitter": {
            "emitRate": "25.00",
            "endSize": "18.00"
        },
        "ImageParticle": {
            "imgOpacity": "1.00"
        }
    }
    \endcode

    Into:
    \code
    {
        "emitRate": "25.00",
        "endSize": "18.00",
        "imgOpacity": "1.00"
    }
    \endcode

    \param properties The hierarchical JSON object containing categorized properties
    \return A flattened QJsonObject with all properties at the root level
*/
QJsonObject Util::flattenProperties(const QJsonObject& properties)
{
    QJsonObject flattened;

    // Iterate through each category
    for (auto categoryIt = properties.begin(); categoryIt != properties.end(); ++categoryIt) {
        const QJsonObject categoryObj = categoryIt.value().toObject();

        // Iterate through properties in this category
        for (auto propIt = categoryObj.begin(); propIt != categoryObj.end(); ++propIt) {
            // Add each property directly to the root level
            flattened.insert(propIt.key(), propIt.value());
        }
    }

    return flattened;
}

/*!
    \brief Checks if two wallpaper types share the same runtime environment
    \param type1 First InstalledType to compare
    \param type2 Second InstalledType to compare
    \return true if both wallpapers use the same runtime, false otherwise
*/
bool Util::isSameWallpaperRuntime(
    const ScreenPlay::ContentTypes::InstalledType type1,
    const ScreenPlay::ContentTypes::InstalledType type2) const
{
    if (!isWallpaper(type1) || !isWallpaper(type2)) {
        return false;
    }
    // If either type is Unknown, they're not compatible
    if (type1 == ScreenPlay::ContentTypes::InstalledType::Unknown || type2 == ScreenPlay::ContentTypes::InstalledType::Unknown) {
        return false;
    }

    // Check if both are Qt-based or both are Godot
    return (isQtBasedWallpaper(type1) && isQtBasedWallpaper(type2)) || (isGodotWallpaper(type1) && isGodotWallpaper(type2));
}

/*!
  \brief parseIntList parses a list of string separated with a comma
        "{1,2,3}". IMPORTANT: No trailing comma!
 */
std::optional<QVector<int>> Util::parseStringToIntegerList(const QString& string) const
{
    if (string.isEmpty())
        return {};

    if (!(string.startsWith("{") && string.endsWith("}")))
        return {};

    QVector<int> list;
    QString clean = string;
    clean = clean.replace("{", "");
    clean = clean.replace("}", "");
    QStringList stringList = clean.split(",");
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

        file.setFileName(":/qt/qml/ScreenPlay/legal/Font Awesome Free License.txt");
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        tmp += out.readAll();
        file.close();

        file.setFileName(":/qt/qml/ScreenPlay/legal/gpl-3.0.txt");
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        tmp += out.readAll();
        file.close();

        file.setFileName(":/qt/qml/ScreenPlay/legal/gpl-3.0.txt");
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        tmp += out.readAll();
        file.close();

        file.setFileName(":/qt/qml/ScreenPlay/legal/OFL.txt");
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        tmp += out.readAll();
        file.close();

        file.setFileName(":/qt/qml/ScreenPlay/legal/OpenSSL.txt");
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        tmp += out.readAll();
        file.close();

        file.setFileName(":/qt/qml/ScreenPlay/legal/Qt LGPLv3.txt");
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

    file.setFileName(":/qt/qml/ScreenPlay/legal/DataProtection.txt");
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

float Util::calculateRelativePosition(const QTime& endTime) const
{
    QTime startTime(0, 0, 0); // Start of the day
    QTime maxTime(23, 59, 59); // End of the day range

    // Total number of seconds from startTime to maxTime
    int totalSeconds = startTime.secsTo(maxTime);

    // Seconds from startTime to the given endTime
    int endTimeSeconds = startTime.secsTo(endTime);

    // Calculate the relative position
    float relativePosition = static_cast<float>(endTimeSeconds) / totalSeconds;

    // Round to four decimal places
    return qRound(relativePosition * 10000.0) / 10000.0;
}

/*!
  \brief Converts a range from 0.0f - 1.0f to 00:00:00 0 23:59:59
*/
QString Util::getTimeString(double relativeLinePosition)
{
    if (relativeLinePosition == 1.0) {
        // We overwrite the endTime here
        return "23:59:59";
    }
    const double totalHours = relativeLinePosition * 24;
    int hours = static_cast<int>(std::floor(totalHours)); // Gets the whole hour part
    double fractionalHours = totalHours - hours;
    int minutes = static_cast<int>(std::floor(fractionalHours * 60)); // Calculates the minutes
    double fractionalMinutes = fractionalHours * 60 - minutes;
    int seconds = static_cast<int>(std::round(fractionalMinutes * 60)); // Calculates the seconds

    // Adjust minutes and seconds if seconds rolled over to 60
    if (seconds == 60) {
        seconds = 0;
        minutes += 1;
    }

    // Adjust hours and minutes if minutes rolled over to 60
    if (minutes == 60) {
        minutes = 0;
        hours += 1;
    }

    // Ensure hours wrap correctly at 24
    if (hours == 24) {
        hours = 0;
    }

    // Format the output to "HH:MM:SS"
    return QString("%1:%2:%3").arg(hours, 2, 10, QChar('0')).arg(minutes, 2, 10, QChar('0')).arg(seconds, 2, 10, QChar('0'));
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
            co_return Result { false, {}, "Unable to open project.json" };
        }
        projectJson = projectOpt.value();
        if (!projectJson.contains("version"))
            co_return Result { false, {}, "Unable to read version in project.json" };

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
