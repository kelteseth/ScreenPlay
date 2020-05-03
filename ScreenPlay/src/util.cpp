#include "util.h"

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
    qRegisterMetaType<Util::AquireFFMPEGStatus>();
    qmlRegisterUncreatableType<Util>("ScreenPlay.QMLUtilities", 1, 0, "QMLUtilities", "Error only for enums");

    // In release mode redirect messages to logging otherwhise we break the nice clickable output :(
#ifdef QT_NO_DEBUG
    qInstallMessageHandler(Util::logToGui);
    qInfo() << "Starting ScreenPlay LOG!";
#endif

    // This gives us nice clickable output in QtCreator
    qSetMessagePattern("%{if-category}%{category}: %{endif}%{message}\n   Loc: [%{file}:%{line}]");

    QString path = QApplication::instance()->applicationDirPath() + "/";
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
    for (const auto &string : list) {
        out += " " + string;
    }
    return out;
}

/*!
  \brief Parses a QByteArray to a QJsonObject. If returns and std::nullopt on failure.
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
  \brief Downloads and extracts ffmpeg static version from https://ffmpeg.zeranoe.com
  The progress is tracked via setAquireFFMPEGStatus(AquireFFMPEGStatus);
*/
void Util::downloadFFMPEG()
{
    QNetworkRequest req;
    QString ffmpegVersion { "ffmpeg-4.2.2" };

#ifdef Q_OS_WIN
    req.setUrl(QUrl("https://ffmpeg.zeranoe.com/builds/win64/static/" + ffmpegVersion + "-win64-static.zip"));
#elif defined(Q_OS_OSX)
    req.setUrl(QUrl("https://ffmpeg.zeranoe.com/builds/macos64/static/" + ffmpegVersion + "-macos64-static.zip"));
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

        string path = QApplication::instance()->applicationDirPath().toStdString() + "/";

        ZipEntry entryFFMPEG;
        std::string entryFFMPEGPath;
#ifdef Q_OS_WIN
         entryFFMPEG = archive->getEntry(ffmpegVersion.toStdString() + "-win64-static/bin/ffmpeg.exe");
         entryFFMPEGPath = path + "ffmpeg.exe";
#elif defined(Q_OS_OSX)
         entryFFMPEG = archive->getEntry(ffmpegVersion.toStdString() +"-macos64-static/bin/ffmpeg");
         entryFFMPEGPath = path + "ffmpeg";
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

         ZipEntry entryFFPROBE;
         std::string entryFFPROBEPath;
#ifdef Q_OS_WIN
        entryFFPROBE = archive->getEntry(ffmpegVersion.toStdString() + "-win64-static/bin/ffprobe.exe");
        entryFFPROBEPath = path + "ffprobe.exe";
#elif defined(Q_OS_OSX)
        entryFFPROBE = archive->getEntry(ffmpegVersion.toStdString() +"-macos64-static/bin/ffprobe");
        entryFFPROBEPath = path + "ffprobe";
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
}

/*!
  \brief Convenient function for the ffmpeg download extraction via libzippp. Extracts a given bytearray
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
