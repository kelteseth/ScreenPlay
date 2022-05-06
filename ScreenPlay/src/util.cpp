#include "ScreenPlay/util.h"

#if defined(Q_OS_WIN)
#include <sentry.h>
#endif

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
  \brief Removes file///: or file:// from the url/string
*/
QString Util::toLocal(const QString& url)
{
    return ScreenPlayUtil::toLocal(url);
}

/*!
  \brief Exports a given project into a .screenplay 7Zip file.
*/
bool Util::exportProject(QString& contentPath, QString& exportPath)
{
    contentPath = ScreenPlayUtil::toLocal(contentPath);
    exportPath = ScreenPlayUtil::toLocal(exportPath);

    QDir dir(contentPath);
    bool success = true;
    if (!dir.exists()) {
        qWarning() << "Directory does not exist!" << dir;
        return false;
    }
    QStringList files;
    for (auto& item : dir.entryInfoList(QDir::Files)) {
        files.append(item.absoluteFilePath());
    }
    m_compressor = std::make_unique<QArchive::DiskCompressor>(exportPath);
    m_compressor->setArchiveFormat(QArchive::SevenZipFormat);
    m_compressor->addFiles(files);
    /* Connect Signals with Slots (in this case lambda functions). */
    QObject::connect(m_compressor.get(), &QArchive::DiskCompressor::started, [&]() {
        qInfo() << "[+] Starting Compressor... ";
    });
    QObject::connect(m_compressor.get(), &QArchive::DiskCompressor::finished, [&]() {
        qInfo() << "[+] Compressed File(s) Successfully!";

        return;
    });
    QObject::connect(m_compressor.get(), &QArchive::DiskCompressor::error, [&](short code, QString file) {
        qInfo() << "[-] An error has occured :: " << QArchive::errorCodeToString(code) << " :: " << file;

        return;
    });

    QObject::connect(m_compressor.get(), &QArchive::DiskCompressor::progress, [&](QString file, int proc, int total, qint64 br, qint64 bt) {
        qInfo() << "Progress::" << file << ":: Done ( " << proc << " / " << total << ") " << (br * 100 / bt) << "%.";
        return;
    });

    m_compressor->start();
    return true;
}

/*!
  \brief Imports a given project from a .screenplay zip file.
*/
bool Util::importProject(QString& archivePath, QString& extractionPath)
{
    archivePath = ScreenPlayUtil::toLocal(archivePath);
    extractionPath = ScreenPlayUtil::toLocal(extractionPath);

    QFileInfo fileInfo(archivePath);
    if (!fileInfo.fileName().endsWith(".screenplay")) {
        qWarning() << "Unsupported file type: " << fileInfo.fileName() << ". We only support '.screenplay' files.";
        return false;
    }
    const QString name = fileInfo.fileName().remove(".screenplay");

    const auto timestamp = QDateTime::currentDateTime().toString("ddMMyyyyhhmmss-");
    extractionPath = extractionPath + "/" + timestamp + name + "/";
    QDir dir(extractionPath);

    if (dir.exists()) {
        qWarning() << "Directory does already exist!" << dir;
        return false;
    }

    if (!dir.mkdir(extractionPath)) {
        qWarning() << "Unable to create directory:" << dir;
        return false;
    }

    m_extractor = std::make_unique<QArchive::DiskExtractor>(archivePath, extractionPath);

    QObject::connect(m_extractor.get(), &QArchive::DiskExtractor::started, [&]() {
        qInfo() << "[+] Starting Extractor... ";
    });
    QObject::connect(m_extractor.get(), &QArchive::DiskExtractor::finished, [&]() {
        qInfo() << "[+] Extracted File(s) Successfully!";
        return;
    });
    QObject::connect(m_extractor.get(), &QArchive::DiskExtractor::error, [&](short code) {
        if (code == QArchive::ArchivePasswordNeeded || code == QArchive::ArchivePasswordIncorrect) {
            return;
        }
        qInfo() << "[-] An error has occured :: " << QArchive::errorCodeToString(code);
        return;
    });
    QObject::connect(m_extractor.get(), &QArchive::DiskExtractor::info, [&](QJsonObject info) {
        qInfo() << "ARCHIVE CONTENTS:: " << info;
        return;
    });

    QObject::connect(m_extractor.get(), &QArchive::DiskExtractor::progress,
        [&](QString file, int proc, int total, qint64 br, qint64 bt) {
            qInfo() << "Progress(" << proc << "/" << total << "): "
                    << file << " : " << (br * 100 / bt) << "% done.";
        });

    m_extractor->setCalculateProgress(true);
    m_extractor->getInfo();
    m_extractor->start();
    return true;
}

/*!
  \brief Loads all content of the legal folder in the qrc into a property string of this class.
  allLicenseLoaded is emited when loading is finished.
*/
void Util::Util::requestAllLicenses()
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
void Util::Util::requestDataProtection()
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

#if defined(Q_OS_WIN)
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
#endif
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

}
