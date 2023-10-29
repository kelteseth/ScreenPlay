#include "ScreenPlayUtil/logginghandler.h"
#include "sysinfo.h"
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QMutex>
#include <QStandardPaths>
#include <QSysInfo>
#include <QUrl>
#include <fmt/color.h>


/*!
    \class ScreenPlayUtil::LoggingHandler
    \inmodule core
    \brief The LoggingHandler class writes logs to the console and a log file.
*/

namespace ScreenPlayUtil {
QString LoggingHandler::m_logFileName;

LoggingHandler::LoggingHandler(const QString& logFileName)
    : QObject()
{
#ifdef Q_OS_WINDOWS
    // Enable UTF-8 support
    SetConsoleOutputCP(CP_UTF8);

    // QtCreator has issues with fmt prints
    // https://bugreports.qt.io/browse/QTCREATORBUG-3994
    setbuf(stdout, NULL);

#endif

    qInstallMessageHandler(LoggingHandler::loggingMessageHandler);
    const auto lock = std::lock_guard(logFileMutex());
    m_logFileName = logFileName;
    start();
}

LoggingHandler::~LoggingHandler()
{
    const auto lock = std::lock_guard(logFileMutex());
    stop();
}

void LoggingHandler::start()
{
    Q_ASSERT(!m_logFileName.isEmpty());

    // Use hardcoded path for now because QStandardpaths gives us: 'C:/ProgramData/K3000'
    QDir directory;
    QString cacheDir = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
    directory = QDir(cacheDir + "/ScreenPlay/Logs");
    if (!directory.exists()) {
        if (!directory.mkpath(directory.path())) {
            qCritical() << "Unable to create logging path at:" << directory.path();
            return;
        }
    }

    QString filePath;
    for (++logFileCounter(); logFileCounter() < 1000; ++logFileCounter()) {
        const auto dateTime = QDateTime::currentDateTime().toString("yy-MM-dd");
        const auto fileName = QString { "%1-%2-%3.log" }.arg(dateTime).arg(m_logFileName).arg(QString::number(logFileCounter()));
        filePath = directory.path() + "/" + fileName;
        auto file = QFile { filePath };
        if (!file.exists())
            break;
        if (file.size() <= m_maxFileSize)
            break;
    }

    logFile().setFileName(filePath);
    const auto isOpen = logFile().open(QIODevice::Append | QIODevice::Text);
    if (!isOpen) {
        qCritical() << "Unable to open log file" << logFile().fileName();
        return;
    }

    
    SysInfo sysInfo;
    QTextStream stream(&logFile());
    stream << "Start ScreenPlay logging " << QDateTime::currentDateTime().toString("dd.MM.yyyy-hh:mm:ss.zzz") << "\n";
    stream << "version: 1.0\n";
    stream << "buildAbi: " << QSysInfo::buildAbi() << "\n";
    stream << "buildCpuArchitecture: " << QSysInfo::buildCpuArchitecture() << "\n";
    stream << "currentCpuArchitecture: " << QSysInfo::currentCpuArchitecture() << "\n";
    stream << "kernelType: " << QSysInfo::kernelType() << "\n";
    stream << "kernelVersion: " << QSysInfo::kernelVersion() << "\n";
    stream << "machineHostName: " << QSysInfo::machineHostName() << "\n";
    stream << "machineUniqueId: " << QSysInfo::machineUniqueId() << "\n";
    stream << "prettyProductName: " << QSysInfo::prettyProductName() << "\n";
    stream << "productType: " << QSysInfo::productType() << "\n";
    stream << "productVersion: " << QSysInfo::productVersion() << "\n";
    stream << "GPU Name: " << sysInfo.gpu()->name() << "\n";
    stream << "GPU Vendor: " << sysInfo.gpu()->vendor() << "\n";
    stream << "GPU ramSize: " << QString::number(sysInfo.gpu()->ramSize()) << "\n";
    stream << "GPU cacheSize: " << QString::number(sysInfo.gpu()->cacheSize()) << "\n";
    stream << "GPU maxFrequency: " << QString::number(sysInfo.gpu()->maxFrequency()) << "\n";
    stream << QString("Uptime: %1 days %2 hours %3 minutes %4 seconds").arg(sysInfo.uptime()->days()).arg(sysInfo.uptime()->hours()).arg(sysInfo.uptime()->minutes()).arg(sysInfo.uptime()->seconds()) << "\n";
    

    const auto now = QLocale().toString(QDateTime::currentDateTime(), QLocale::FormatType::ShortFormat);
    const auto message = QString { "[%1] Start logging\n" }.arg(now);
    logFile().write(message.toStdString().c_str());
}

void LoggingHandler::stop()
{
    const auto now = QLocale().toString(QDateTime::currentDateTime(), QLocale::FormatType::ShortFormat);
    const auto message = QString { "[%1] Stop logging\n\n" }.arg(now);
    logFile().write(message.toStdString().c_str());
    logFile().close();
}

std::mutex& LoggingHandler::logFileMutex()
{
    static std::mutex mutex;
    return mutex;
}

size_t& LoggingHandler::logFileCounter()
{
    static size_t counter = 0;
    return counter;
}

QFile& LoggingHandler::logFile()
{
    static QFile file;
    return file;
}

/*!
    \brief Returns a formatted logging line.

    Format: "[<DATETIME>]|<CATEGORY>.<TYPE>|<FILENAME>:<LINENUMBER>|<FUNCTION>|<MESSAGE>\n"

    If possible, this format should not be changed, otherwise the subsequent parsing of the log file will be very difficult.
 */
QString LoggingHandler::logLine(QtMsgType type, const QMessageLogContext& context, const QString& message)
{
    const auto now = QLocale().toString(QDateTime::currentDateTime(), QLocale::FormatType::ShortFormat);
    const auto filename = extractFileName(context);
    const auto function = extractFunction(context);
    auto category = QString(context.category);
    if (category == "default")
        category = "";

    return QString("[%1] %2.%3 | %4:%5 | %6 | %7\n").arg(now).arg(category).arg(toString(type)).arg(filename).arg(context.line).arg(function).arg(message);
}

QString LoggingHandler::toString(QtMsgType type)
{
    switch (type) {
    case QtDebugMsg:
        return "Debug";
    case QtWarningMsg:
        return "Warning";
    case QtCriticalMsg:
        return "Critical";
    case QtFatalMsg:
        return "Fatal";
    case QtInfoMsg:
        return "Info";
    default:
        return "?";
    }
}

QString LoggingHandler::extractFileName(const QMessageLogContext& context)
{
    auto file = QString(context.file);
    return file.mid(file.lastIndexOf("\\")).remove("\\");
}

/*!
    \brief Removes all the unreadable type stuff from the function information in \a context.
 */
QString LoggingHandler::extractFunction(const QMessageLogContext& context)
{
    auto func = QString(context.function);

    auto from = -1;
    if (from < 0) {
        static const QString search { "__thiscall" };
        from = func.indexOf(search);
        if (from >= 0) {
            from += search.length();
        }
    }
    if (from < 0) {
        static const QString search { "__cdecl" };
        from = func.indexOf(search);
        if (from >= 0) {
            from += search.length();
        }
    }
    if (from < 0) {
        from = 0;
    }

    auto to = func.indexOf("(", from);
    if (to < 0)
        return func;

    auto length = to - from;
    return func.mid(from, length).trimmed();
}

/*!
 * \brief LoggingHandler::writeToConsole
 * std::flush is used to fix QtCreator not printing output.
 */
void LoggingHandler::writeToConsole(const QString& line, QtMsgType type)
{
    constexpr auto darkMode = true;
    auto color = fmt::color::black;

    switch (type) {
    case QtDebugMsg:
        color = fmt::color::green;
        break;
    case QtWarningMsg:
        color = fmt::color::orange;
        break;
    case QtCriticalMsg:
        color = fmt::color::magenta;
        break;
    case QtFatalMsg:
        color = fmt::color::red;
        break;
    default:
        color = darkMode ? fmt::color::gray : fmt::color::black;
        break;
    }
    fmt::print("{}", fmt::styled(line.toStdString(), fg(color)));
}

void LoggingHandler::writeToFile(const QString& line)
{
    std::lock_guard lock(logFileMutex());
    if (logFile().isOpen()) {
        checkLogRotation();
        logFile().write(line.toStdString().c_str());
    }
}

/*!
    \brief Opens a new log file if the current log file becomes too large.
 */
void LoggingHandler::checkLogRotation()
{
    if (logFile().size() <= m_maxFileSize)
        return;

    logFile().write("Maximum file size reached. A new log file is used\n");
    qInfo() << "Rotate Log file";
    stop();
    start();
}

void LoggingHandler::loggingMessageHandler(QtMsgType type, const QMessageLogContext& context, const QString& message)
{
    const QString line = LoggingHandler::logLine(type, context, message);
    LoggingHandler::writeToConsole(line, type);
    LoggingHandler::writeToFile(line);
}
}
