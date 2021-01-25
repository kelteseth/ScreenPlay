#include "util.h"

#include <sentry.h>

namespace ScreenPlay {

/*!
    \class ScreenPlay::Util
    \inmodule ScreenPlay
    \brief Easy to use global object to use when certain functionality is not available in QML.
*/

/*!
  \brief Constructor.
*/
Util::Util(QNetworkAccessManager* networkAccessManager, QObject* parent)
    : QObject(parent)
    , m_networkAccessManager { networkAccessManager }
{
    utilPointer = this;
    // Fix log access vilation on quit
    QObject::connect(QGuiApplication::instance(), &QGuiApplication::aboutToQuit, this, []() { utilPointer = nullptr; });

    qmlRegisterUncreatableType<Util>("ScreenPlay.QMLUtilities", 1, 0, "QMLUtilities", "Error only for enums");

    // In release mode redirect messages to logging otherwhise we break the nice clickable output :(
#ifdef QT_NO_DEBUG
    qInstallMessageHandler(Util::logToGui);
    qInfo() << "Starting ScreenPlay LOG!";
#endif

    // This gives us nice clickable output in QtCreator
    qSetMessagePattern("%{if-category}%{category}: %{endif}%{message}\n   Loc: [%{file}:%{line}]");
}

/*!
  \brief Copies the given string to the clipboard.
*/
void Util::copyToClipboard(const QString& text) const
{
    auto* clipboard = QApplication::clipboard();
    clipboard->setText(text);
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
  \brief  Opens a json file (absolute path) and tries to convert it to a QString.
  Returns std::nullopt when not successful.
*/
std::optional<QString> Util::openJsonFileToString(const QString& path)
{
    QFile file;
    file.setFileName(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
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
  \brief Return .exe in windows otherwise empty string.
*/
QString Util::executableEnding()
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
   \brief Writes a given QJsonObject to a file. The path must be absolute. When truncate is set to
   true the exsisting json file will be overriten.
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
        return false;
    }

    QTextStream out(&configTmp);
    out.setCodec("UTF-8");
    out << QJsonDocument(object).toJson();

    configTmp.close();
    return true;
}

/*!
    \brief Helper function to append a QStringList into a QString with a space between the items.
*/
QString Util::toString(const QStringList& list)
{
    QString out;
    for (const auto& string : list) {
        out += " " + string;
    }
    return out;
}

void Util::appendToMetricsFile(const QString& key, const QVariant& value)
{
    if (!QGuiApplication::arguments().contains("--benchmark"))
        return;

    const QString appDir = QGuiApplication::applicationDirPath();
    QFile metricsFile { appDir + "/metrics.txt" };

    if (!metricsFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)) {
        qWarning() << "Cannot open metrix file:" << appDir << metricsFile.fileName();
        return;
    }

    QString text = key + "\t" + value.toString() + "\n";
    QTextStream out(&metricsFile);
    out << text;

    metricsFile.flush();
    metricsFile.close();
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
  \brief Opens a native folder window on the given path. Windows and Mac only for now!
*/
void Util::openFolderInExplorer(const QString& url) const
{

    QString path = QUrl::fromUserInput(url).toLocalFile();

    QProcess explorer;
#ifdef Q_OS_WIN
    explorer.setProgram("explorer.exe");
    // When we have space in the path like
    // C:\Program Files (x86)\Steam\...
    // we cannot set the path as an argument. But we can set the working it
    // to the wanted path and open the current path via the dot.
    explorer.setWorkingDirectory(QDir::toNativeSeparators(path));
    explorer.setArguments({ "." });
#elif defined(Q_OS_OSX)
    explorer.setProgram("open");
    explorer.setArguments({ QDir::toNativeSeparators(path) });
#endif

    explorer.startDetached();
}

/*!
  \brief Loads all content of the legal folder in the qrc into a property string of this class.
  allLicenseLoaded is emited when loading is finished.
*/
void Util::Util::requestAllLicenses()
{

    QtConcurrent::run([this]() {
        QString tmp;
        QFile file;
        QTextStream out(&file);

        file.setFileName(":/legal/Font Awesome Free License.txt");
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        tmp += out.readAll();
        file.close();

        file.setFileName(":/legal/gpl-3.0.txt");
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        tmp += out.readAll();
        file.close();

        file.setFileName(":/legal/gpl-3.0.txt");
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        tmp += out.readAll();
        file.close();

        file.setFileName(":/legal/OFL.txt");
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        tmp += out.readAll();
        file.close();

        file.setFileName(":/legal/OpenSSL.txt");
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        tmp += out.readAll();
        file.close();

        file.setFileName(":/legal/Qt LGPLv3.txt");
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
void Util::Util::requestDataProtection()
{
    QtConcurrent::run([this]() {
        QString tmp;
        QFile file;
        QTextStream out(&file);

        file.setFileName(":/legal/DataProtection.txt");
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        tmp += out.readAll();
        file.close();

        emit this->allDataProtectionLoaded(tmp);
    });
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
  \brief Maps the Search type to an installed type. Used for filtering the installed
         content.

*/
SearchType::SearchType Util::getSearchTypeFromInstalledType(const InstalledType::InstalledType type)
{
    using InstalledType::InstalledType;
    using SearchType::SearchType;

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
std::optional<InstalledType::InstalledType> Util::getInstalledTypeFromString(const QString& type)
{
    if (type.endsWith("Wallpaper")) {
        if (type.startsWith("video", Qt::CaseInsensitive)) {
            return InstalledType::InstalledType::VideoWallpaper;
        }
        if (type.startsWith("qml", Qt::CaseInsensitive)) {
            return InstalledType::InstalledType::QMLWallpaper;
        }
        if (type.startsWith("html", Qt::CaseInsensitive)) {
            return InstalledType::InstalledType::HTMLWallpaper;
        }
        if (type.startsWith("godot", Qt::CaseInsensitive)) {
            return InstalledType::InstalledType::GodotWallpaper;
        }
        if (type.startsWith("website", Qt::CaseInsensitive)) {
            return InstalledType::InstalledType::WebsiteWallpaper;
        }
        if (type.startsWith("gif", Qt::CaseInsensitive)) {
            return InstalledType::InstalledType::GifWallpaper;
        }
    }

    if (type.endsWith("Widget")) {
        if (type.startsWith("qml", Qt::CaseInsensitive)) {
            return InstalledType::InstalledType::QMLWidget;
        }
        if (type.startsWith("html", Qt::CaseInsensitive)) {
            return InstalledType::InstalledType::HTMLWidget;
        }
    }

    return std::nullopt;
}

static const char*
logLevelForMessageType(QtMsgType msgType)
{
    switch (msgType) {
    case QtDebugMsg:
        return "debug";
    case QtWarningMsg:
        return "warning";
    case QtCriticalMsg:
        return "error";
    case QtFatalMsg:
        return "fatal";
    case QtInfoMsg:
        Q_FALLTHROUGH();
    default:
        return "info";
    }
}

/*!
  \brief Basic logging to the GUI. No logging is done to a log file for now. This string can be copied
  in the settings tab in the UI.
*/
void Util::logToGui(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
    qDebug() << msg;
    QByteArray localMsg = msg.toLocal8Bit();
    QByteArray file = "File: " + QByteArray(context.file) + ", ";
    QByteArray line = "in line " + QByteArray::number(context.line) + ", ";
    QByteArray function = "function " + QByteArray(context.function) + ", Message: ";

    QString log = "&emsp; <font color=\"#03A9F4\"> " + QDateTime::currentDateTime().toString() + "</font> &emsp; " + localMsg + "<br>";

    switch (type) {
    case QtDebugMsg:
        log.prepend("<b><font color=\"##78909C\"> Debug</font>:</b>");
        break;
    case QtInfoMsg:
        log.prepend("<b><font color=\"#8BC34A\"> Info</font>:</b>");
        break;
    case QtWarningMsg:
        log.prepend("<b><font color=\"#FFC107\"> Warning</font>:</b>");
        break;
    case QtCriticalMsg:
        log.prepend("<b><font color=\"#FF5722\"> Critical</font>:</b>");
        break;
    case QtFatalMsg:
        log.prepend("<b><font color=\"#F44336\"> Fatal</font>:</b>");
        break;
    }
    log.append("\n");

    if (utilPointer != nullptr)
        utilPointer->appendDebugMessages(log);

    sentry_value_t crumb
        = sentry_value_new_breadcrumb("default", qUtf8Printable(msg));

    sentry_value_set_by_key(
        crumb, "category", sentry_value_new_string(context.category));

    sentry_value_set_by_key(
        crumb, "level", sentry_value_new_string(logLevelForMessageType(type)));

    sentry_value_t location = sentry_value_new_object();
    sentry_value_set_by_key(
        location, "file", sentry_value_new_string(context.file));
    sentry_value_set_by_key(
        location, "line", sentry_value_new_int32(context.line));
    sentry_value_set_by_key(crumb, "data", location);

    sentry_add_breadcrumb(crumb);
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
    out.setCodec("UTF-8");
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
    out.setCodec("UTF-8");

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
    out.setCodec("UTF-8");

    QFile qrc(qrcPath);
    qrc.open(QIODevice::ReadOnly);
    out << qrc.readAll();

    qrc.close();
    file.close();
    return true;
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

/*!
    \brief Converts the given \a url string to a local file path.
*/
QString Util::toLocal(const QString& url)
{
    return QUrl(url).toLocalFile();
}

}
