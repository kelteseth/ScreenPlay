#include "qmlutilities.h"

namespace ScreenPlay {

/*!
    \class Global QML Utilities
    \brief Easy to use global object to use to:
    \list
        \i Navigate the main menu
        \i Open Explorer at a given path
    \endlist
*/

QMLUtilities::QMLUtilities(QNetworkAccessManager* networkAccessManager, QObject* parent)
    : QObject(parent)
    , m_networkAccessManager { networkAccessManager }
{
}
void QMLUtilities::setNavigation(QString nav)
{
    emit requestNavigation(nav);
}

void QMLUtilities::setNavigationActive(bool isActive)
{
    emit requestNavigationActive(isActive);
}

void QMLUtilities::setToggleWallpaperConfiguration()
{
    emit requestToggleWallpaperConfiguration();
}

QString QMLUtilities::fixWindowsPath(QString url)
{
    return url.replace("/", "\\\\");
}

void QMLUtilities::openFolderInExplorer(QString url)
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

void QMLUtilities::QMLUtilities::requestAllLicenses()
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

void QMLUtilities::QMLUtilities::requestAllLDataProtection()
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

void QMLUtilities::downloadFFMPEG()
{
    QNetworkRequest req;

#ifdef Q_OS_WIN
    req.setUrl(QUrl("https://ffmpeg.zeranoe.com/builds/win64/static/ffmpeg-4.1.4-win64-static.zip"));
#elif defined(Q_OS_OSX)
    req.setUrl(QUrl("https://ffmpeg.zeranoe.com/builds/macos64/static/ffmpeg-4.1.4-macos64-static.zip"));
#endif

    QNetworkReply* reply = m_networkAccessManager->get(req);
    QObject::connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        using namespace libzippp;
        using namespace std;
        QByteArray download = reply->readAll();

        auto* archive = ZipArchive::fromBuffer(download.data(), download.size(), ZipArchive::OpenMode::READ_ONLY, true);

        if (archive == nullptr) {
            qDebug() << "null";
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
            emit this->downloadFFMPEGCompleted(false);
            return;
        }

        if (!saveExtractedByteArray(entryFFMPEG, entryFFMPEGPath)) {
            qDebug() << "could not save ffmpeg";
            emit this->downloadFFMPEGCompleted(false);
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
            emit this->downloadFFMPEGCompleted(false);
            return;
        }

        if (!saveExtractedByteArray(entryFFPROBE, entryFFPROBEPath)) {
            qDebug() << "could not save ffprobe";
            emit this->downloadFFMPEGCompleted(false);
            return;
        }

        emit this->downloadFFMPEGCompleted(true);
    });
}

bool QMLUtilities::saveExtractedByteArray(libzippp::ZipEntry& entry, std::string& absolutePathAndName)
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
