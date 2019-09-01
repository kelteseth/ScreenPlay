#include "util.h"

namespace ScreenPlay {

/*!
    \class Global QML Utilities
    \brief Easy to use global object to use to:
    \list
        \i Navigate the main menu
        \i Open Explorer at a given path
    \endlist
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

std::optional<QJsonObject> Util::openJsonFileToObject(const QString& path)
{
    auto jsonString = openJsonFileToString(path);

    if (!jsonString.has_value()) {
        return {};
    }

    QJsonDocument jsonDocument;
    QJsonParseError parseError {};
    jsonDocument = QJsonDocument::fromJson(jsonString.value().toUtf8(), &parseError);

    if (!(parseError.error == QJsonParseError::NoError)) {
        qWarning() << "Settings Json Parse Error: " << parseError.errorString();
        return {};
    }

    return jsonDocument.object();
}

std::optional<QString> Util::openJsonFileToString(const QString& path)
{
    QFile file;
    file.setFileName(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return {};
    }
    QString fileContent = file.readAll();
    file.flush();
    file.close();

    return { fileContent };
}

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

void Util::setNavigation(QString nav)
{
    emit requestNavigation(nav);
}

void Util::setNavigationActive(bool isActive)
{
    emit requestNavigationActive(isActive);
}

void Util::setToggleWallpaperConfiguration()
{
    emit requestToggleWallpaperConfiguration();
}

QString Util::fixWindowsPath(QString url)
{
    return url.replace("/", "\\\\");
}

void Util::openFolderInExplorer(QString url)
{
    QProcess explorer;
#ifdef Q_OS_WIN
    explorer.setProgram("explorer.exe");
#elif defined(Q_OS_OSX)
    explorer.setProgram("open");
#endif
    QStringList args;
    args.append(QDir::toNativeSeparators(url));
    explorer.setArguments(args);
    explorer.startDetached();
}

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

void Util::Util::requestAllLDataProtection()
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

void Util::downloadFFMPEG()
{
    QNetworkRequest req;

#ifdef Q_OS_WIN
    req.setUrl(QUrl("https://ffmpeg.zeranoe.com/builds/win64/static/ffmpeg-4.1.4-win64-static.zip"));
#elif defined(Q_OS_OSX)
    req.setUrl(QUrl("https://ffmpeg.zeranoe.com/builds/macos64/static/ffmpeg-4.1.4-macos64-static.zip"));
#endif
    setAquireFFMPEGStatus(AquireFFMPEGStatus::Download);

    QNetworkReply* reply = m_networkAccessManager->get(req);
    QObject::connect(reply, &QNetworkReply::finished, this, [this, reply]() {
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
        ZipEntry entryFFMPEG = archive->getEntry("ffmpeg-4.1.4-win64-static/bin/ffmpeg.exe");
        std::string entryFFMPEGPath = path + "ffmpeg.exe";
#elif defined(Q_OS_OSX)
        ZipEntry entryFFMPEG = archive->getEntry("ffmpeg-4.1.4-macos64-static/bin/ffmpeg");
        std::string entryFFMPEGPath = path + "ffmpeg";
#endif

        if (entryFFMPEG.isNull()) {
            qDebug() << "null";

            setAquireFFMPEGStatus(AquireFFMPEGStatus::ExtractingFailedFFMPEG);
            return;
        }

        if (!saveExtractedByteArray(entryFFMPEG, entryFFMPEGPath)) {
            qDebug() << "could not save ffmpeg";
            setAquireFFMPEGStatus(AquireFFMPEGStatus::ExtractingFailedFFMPEGSave);
            return;
        }

#ifdef Q_OS_WIN
        ZipEntry entryFFPROBE = archive->getEntry("ffmpeg-4.1.4-win64-static/bin/ffprobe.exe");
        std::string entryFFPROBEPath = path + "ffprobe.exe";
#elif defined(Q_OS_OSX)
        ZipEntry entryFFPROBE = archive->getEntry("ffmpeg-4.1.4-macos64-static/bin/ffprobe");
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
