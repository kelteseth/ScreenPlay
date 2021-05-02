#include "ScreenPlayUtil/util.h"
#include <QFile>
#include <QJsonParseError>
#include <QRandomGenerator>

namespace ScreenPlayUtil {

/*!
  \brief Opens a json file (absolute path) and tries to convert it to a QJsonObject.
  Returns std::nullopt when not successful.
*/
std::optional<QJsonObject> openJsonFileToObject(const QString& path)
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
    \brief .
*/
bool writeJsonObjectToFile(const QString& absoluteFilePath, const QJsonObject& object, bool truncate)
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
    out.setCodec("UTF-8");
    out << QJsonDocument(object).toJson();

    configTmp.close();
    return true;
}

/*!
  \brief  Opens a json file (absolute path) and tries to convert it to a QString.
  Returns std::nullopt when not successful.
*/
std::optional<QString> openJsonFileToString(const QString& path)
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
QString generateRandomString(quint32 length)
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
  \brief Return .exe in windows otherwise empty string.
*/
QString executableEnding()
{
#ifdef Q_OS_WIN
    return ".exe";
#else
    return "";
#endif
}

/*!
  \brief Parses a version from a given QString. The QString must be looke like this:
  1.0.0 - Major.Minor.Patch. A fixed position is used for parsing (at 0,2,4).
  Return std::nullopt when not successful.
*/
std::optional<QVersionNumber> getVersionNumberFromString(const QString& str)
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
std::optional<QJsonObject> parseQByteArrayToQJsonObject(const QByteArray& byteArray)
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
QString toString(const QStringList& list)
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
QJsonArray fillArray(const QVector<QString>& items)
{
    QJsonArray array;
    for (const QString& item : items) {
        array.append(item);
    }
    return array;
}

/*!
  \brief Maps the Search type to an installed type. Used for filtering the installed
         content.

*/
ScreenPlay::SearchType::SearchType getSearchTypeFromInstalledType(const ScreenPlay::InstalledType::InstalledType type)
{
    using ScreenPlay::InstalledType::InstalledType;
    using ScreenPlay::SearchType::SearchType;

    switch (type) {
    case InstalledType::GodotWallpaper:
    case InstalledType::HTMLWallpaper:
    case InstalledType::QMLWallpaper:
    case InstalledType::GifWallpaper:
    case InstalledType::WebsiteWallpaper:
        return SearchType::Scene;
    case InstalledType::VideoWallpaper:
        return SearchType::Wallpaper;
    case InstalledType::HTMLWidget:
    case InstalledType::QMLWidget:
        return SearchType::Widget;
    case InstalledType::Unknown:
    default:
        return SearchType::All;
    }
}

/*!
    \brief Maps the installed type from a QString to an enum. Used for parsing the project.json.
*/
std::optional<ScreenPlay::InstalledType::InstalledType> getInstalledTypeFromString(const QString& type)
{
    using ScreenPlay::InstalledType::InstalledType;

    if (type.endsWith("Wallpaper")) {
        if (type.startsWith("video", Qt::CaseInsensitive)) {
            return InstalledType::VideoWallpaper;
        }
        if (type.startsWith("qml", Qt::CaseInsensitive)) {
            return InstalledType::QMLWallpaper;
        }
        if (type.startsWith("html", Qt::CaseInsensitive)) {
            return InstalledType::HTMLWallpaper;
        }
        if (type.startsWith("godot", Qt::CaseInsensitive)) {
            return InstalledType::GodotWallpaper;
        }
        if (type.startsWith("website", Qt::CaseInsensitive)) {
            return InstalledType::WebsiteWallpaper;
        }
        if (type.startsWith("gif", Qt::CaseInsensitive)) {
            return InstalledType::GifWallpaper;
        }
    }

    if (type.endsWith("Widget")) {
        if (type.startsWith("qml", Qt::CaseInsensitive)) {
            return InstalledType::QMLWidget;
        }
        if (type.startsWith("html", Qt::CaseInsensitive)) {
            return InstalledType::HTMLWidget;
        }
    }

    return std::nullopt;
}

/*!
    \brief Converts the given \a url string to a local file path.
*/
QString toLocal(const QString& url)
{
    return QUrl(url).toLocalFile();
}

/*!
    \brief .
*/
QStringList getAvailableWallpaper()
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
    \brief .
*/
QStringList getAvailableWidgets()
{
    return {
        "qmlWidget",
        "htmlWidget",
    };
}

/*!
    \brief .
*/
QStringList getAvailableTypes()
{
    return { getAvailableWallpaper() + getAvailableWidgets() };
}

/*!
    \brief .
*/
bool isWallpaper(const ScreenPlay::InstalledType::InstalledType type)
{

    using ScreenPlay::InstalledType::InstalledType;

    return (type == InstalledType::VideoWallpaper
        || type == InstalledType::QMLWallpaper
        || type == InstalledType::HTMLWallpaper
        || type == InstalledType::GifWallpaper
        || type == InstalledType::WebsiteWallpaper
        || type == InstalledType::GodotWallpaper);
}

/*!
    \brief .
*/
bool isWidget(const ScreenPlay::InstalledType::InstalledType type)
{

    using ScreenPlay::InstalledType::InstalledType;

    return (type == InstalledType::QMLWidget || type == InstalledType::HTMLWidget);
}

/*!
    \brief See https://developer.mozilla.org/en-US/docs/Web/CSS/object-fit
*/
QStringList getAvailableFillModes()
{
    return { "stretch", "fill", "contain", "cover", "scale-down" };
}

/*!
  \brief parseIntList parses a list of string separated with a comma
        "1,2,3". IMPORTANT: No trailing comma!
 */
std::optional<QVector<int>> parseStringToIntegerList(const QString string)
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

}
