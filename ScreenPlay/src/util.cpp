#include "util.h"

namespace ScreenPlay {

/*!
    \class ScreenPlay::Util
    \inmodule ScreenPlay
    \brief Easy to use global object to use when certain functionality is not available in QML.
*/

/*!
  Constructor
*/
Util::Util(QNetworkAccessManager* networkAccessManager, QObject* parent)
    : QObject(parent)
    , m_networkAccessManager { networkAccessManager }
{
    utilPointer = this;
    qRegisterMetaType<Util::AquireFFMPEGStatus>();
    qmlRegisterUncreatableType<Util>("ScreenPlay.QMLUtilities", 1, 0, "QMLUtilities", "Error only for enums");

    // In release mode redirect messages to logging otherwhise we break the nice clickable output :(
#ifdef QT_NO_DEBUG
    qInstallMessageHandler(Util::logToGui);
    qInfo() << "Starting ScreenPlay LOG!";
#endif

    // This gives us nice clickable output in QtCreator
    qSetMessagePattern("%{if-category}%{category}: %{endif}%{message}\n   Loc: [%{file}:%{line}]");

    QString path = QGuiApplication::instance()->applicationDirPath() + "/";
    QFile fileFFMPEG;
    QFile fileFFPROBE;

#ifdef Q_OS_WIN
    fileFFMPEG.setFileName(path + "ffmpeg.exe");
    fileFFPROBE.setFileName(path + "ffprobe.exe");
#else
    fileFFMPEG.setFileName(path + "ffmpeg");
    fileFFPROBE.setFileName(path + "ffprobe");
#endif

    if (fileFFMPEG.exists() && fileFFPROBE.exists())
        setFfmpegAvailable(true);
}

/*!
  Copies the given string to the clipboard.
*/
void Util::copyToClipboard(const QString& text) const
{
    auto* clipboard = QGuiApplication::clipboard();
    clipboard->setText(text);
}

/*!
  Opens a json file (absolute path) and tries to convert it to a QJsonObject.
  Return std::nullopt when not successful.
*/
std::optional<QJsonObject> Util::openJsonFileToObject(const QString& path)
{
    auto jsonString = openJsonFileToString(path);

    if (!jsonString.has_value()) {
        return std::nullopt;
    }

    QJsonDocument jsonDocument;
    QJsonParseError parseError {};
    jsonDocument = QJsonDocument::fromJson(jsonString.value().toUtf8(), &parseError);

    if (!(parseError.error == QJsonParseError::NoError)) {
        qWarning() << "Settings Json Parse Error: " << parseError.errorString();
        return std::nullopt;
    }

    return jsonDocument.object();
}

/*!
  Opens a json file (absolute path) and tries to convert it to a QString.
  Return std::nullopt when not successful.
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
  Generates a (non secure) random string with the default length of 32. Can contain:
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
  Parses a version from a given QString. The QString must be looke like this:
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
   Writes a given QJsonObject to a file. The path must be absolute. When truncate is set to
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
    out << QJsonDocument(object).toJson();

    configTmp.close();
    return true;
}

QString Util::toString(const QStringList& list)
{
    QString out;
    for (auto string : list) {
        out += " " + string;
    }
    return out;
}

QString Util::fixWindowsPath(QString url)
{
    return url.replace("/", "\\\\");
}

/*!
  Opens a native folder window on the given path. Windows and Mac only for now!
*/
std::optional<QJsonObject> Util::parseQByteArrayToQJsonObject(const QByteArray& byteArray)
{
    QJsonObject obj;
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(byteArray, &err);

    if (err.error == QJsonParseError::NoError) {
        obj = doc.object();
        return { obj };
    }

    return std::nullopt;
}

void Util::openFolderInExplorer(const QString& url) const
{
    QString fileString { "file:///" };
    QString parameter = url;
    if (url.contains(fileString)) {
        parameter.remove(fileString);
    }
    QProcess explorer;
#ifdef Q_OS_WIN
    explorer.setProgram("explorer.exe");
    // When we have space in the path like
    // C:\Program Files (x86)\Steam\...
    // we cannot set the path as an argument. But we can set the working it
    // to the wanted path and open the current path via the dot.
    explorer.setWorkingDirectory(QDir::toNativeSeparators(parameter));
    explorer.setArguments({ "." });
#elif defined(Q_OS_OSX)
    explorer.setProgram("open");
    explorer.setArguments({ QDir::toNativeSeparators(parameter) });
#endif

    explorer.startDetached();
}



/*!
  Loads all content of the legal folder in the qrc into a property string of this class.
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
  Loads all dataprotection of the legal folder in the qrc into a property string of this class.
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
  Downloads and extracts ffmpeg static version from https://ffmpeg.zeranoe.com
  The progress is tracked via setAquireFFMPEGStatus(AquireFFMPEGStatus);
*/
void Util::downloadFFMPEG()
{
    QNetworkRequest req;
    QString ffmpegVersion { "ffmpeg-4.2.1" };

#ifdef Q_OS_WIN
    req.setUrl(QUrl("https://ffmpeg.zeranoe.com/builds/win64/static/" + ffmpegVersion + "-win64-static.zip"));
#elif defined(Q_OS_OSX)
    req.setUrl(QUrl("https://ffmpeg.zeranoe.com/builds/macos64/static/" + ffmpegVersion + "-win64-static.zip"));
#endif
    setAquireFFMPEGStatus(AquireFFMPEGStatus::Download);

    QNetworkReply* reply = m_networkAccessManager->get(req);
    QObject::connect(reply, &QNetworkReply::finished, this, [this, reply, ffmpegVersion]() {
        using namespace libzippp;
        using namespace std;

        setAquireFFMPEGStatus(AquireFFMPEGStatus::DownloadSuccessful);

        QByteArray download = reply->readAll();

        auto* archive = ZipArchive::fromBuffer(download.data(), download.size(), ZipArchive::OpenMode::READ_ONLY, true);

        if (archive == nullptr) {
            setAquireFFMPEGStatus(AquireFFMPEGStatus::ExtractingFailedReadFromBuffer);
            return;
        }

        string path = QGuiApplication::instance()->applicationDirPath().toStdString() + "/";

#ifdef Q_OS_WIN
        ZipEntry entryFFMPEG = archive->getEntry(ffmpegVersion.toStdString() + "-win64-static/bin/ffmpeg.exe");
        std::string entryFFMPEGPath = path + "ffmpeg.exe";
#elif defined(Q_OS_OSX)
        ZipEntry entryFFMPEG = archive->getEntry(ffmpegVersion.toStdString() +"-macos64-static/bin/ffmpeg");
        std::string entryFFMPEGPath = path + "ffmpeg";
#endif

        if (entryFFMPEG.isNull()) {
            qDebug() << "entryFFMPEG is null. No more entry is available.";
            qDebug() << "Download size was: " << download.size() << "bytes";

            setAquireFFMPEGStatus(AquireFFMPEGStatus::ExtractingFailedFFMPEG);
            return;
        }

        if (!saveExtractedByteArray(entryFFMPEG, entryFFMPEGPath)) {
            qDebug() << "could not save ffmpeg";
            setAquireFFMPEGStatus(AquireFFMPEGStatus::ExtractingFailedFFMPEGSave);
            return;
        }

#ifdef Q_OS_WIN
        ZipEntry entryFFPROBE = archive->getEntry(ffmpegVersion.toStdString() + "-win64-static/bin/ffprobe.exe");
        std::string entryFFPROBEPath = path + "ffprobe.exe";
#elif defined(Q_OS_OSX)
        ZipEntry entryFFPROBE = archive->getEntry(ffmpegVersion.toStdString() +"-macos64-static/bin/ffprobe");
        std::string entryFFPROBEPath = path + "ffprobe";
#endif
        if (entryFFPROBE.isNull()) {
            qDebug() << "null";
            setAquireFFMPEGStatus(AquireFFMPEGStatus::ExtractingFailedFFPROBE);
            return;
        }

        if (!saveExtractedByteArray(entryFFPROBE, entryFFPROBEPath)) {
            qDebug() << "could not save ffprobe";
            setAquireFFMPEGStatus(AquireFFMPEGStatus::ExtractingFailedFFPROBESave);
            return;
        }

        setAquireFFMPEGStatus(AquireFFMPEGStatus::FinishedSuccessful);
    });

    QObject::connect(reply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
        [this](QNetworkReply::NetworkError code) {
            this->setAquireFFMPEGStatus(AquireFFMPEGStatus::DownloadFailed);
        });
}

/*!
  Basic logging to the GUI. No logging is done to a log file for now. This string can be copied
  in the settings tab in the UI.
*/
void Util::logToGui(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
    qDebug() << msg;
    QByteArray localMsg = msg.toLocal8Bit();
    QByteArray file = "File: " + QByteArray(context.file) + ", ";
    QByteArray line = "in line " + QByteArray::number(context.line) + ", ";
    QByteArray function = "function " + QByteArray(context.function) + ", Message: ";

    QString log = "&emsp; <font color=\"#03A9F4\"> " + QDateTime::currentDateTime().toString() + "</font> &emsp; " + localMsg + "<br><br>";

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

    if (utilPointer != nullptr)
        utilPointer->appendDebugMessages(log);
}

/*!
  Convenient function for the ffmpeg download extraction via libzippp. Extracts a given bytearray
  to a given absolute file path and file name. Returns false if extraction or saving wasn't successful.
*/
bool Util::saveExtractedByteArray(libzippp::ZipEntry& entry, std::string& absolutePathAndName)
{
    std::ofstream ofUnzippedFile(absolutePathAndName, std::ofstream::binary);

    if (ofUnzippedFile) {
        if (entry.readContent(ofUnzippedFile) != 0) {
            qDebug() << "Error cannot read ffmpeg zip content";
            return false;
        }
    } else {
        qDebug() << "Coud not open ofstream" << QString::fromStdString(absolutePathAndName);
        return false;
    }

    ofUnzippedFile.close();
    return true;
}

}
