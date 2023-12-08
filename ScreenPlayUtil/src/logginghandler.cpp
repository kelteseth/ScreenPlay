#include "ScreenPlayUtil/logginghandler.h"
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QMutex>
#include <QStandardPaths>
#include <QSysInfo>
#include <QUrl>
#include <fmt/color.h>

#include "ScreenPlayUtil/CMakeVariables.h"

#ifdef Q_OS_WINDOWS
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>
#endif

/*!
    \class ScreenPlayUtil::LoggingHandler
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
    // This is a QtCreator workaround and crashes std::fmt
    // when running standalone.
    if (!SCREENPLAY_DEPLOY_VERSION) {
        // QtCreator has issues with fmt prints
        // https://bugreports.qt.io/browse/QTCREATORBUG-3994
        setvbuf(stdout, NULL, _IONBF, 0);
        qInfo() << "Setting setvbuf(stdout, NULL, _IONBF, 0); This unbuffered output causes crashes in release with threaded fmt!";
    }
#endif

    qSetMessagePattern("[%{time dd.MM.yyyy h:mm:ss.zzz} %{if-debug}Debug%{endif}%{if-info}Info%{endif}%{if-warning}Warning%{endif}%{if-critical}Critical%{endif}%{if-fatal}Fatal%{endif}] %{file}:%{line} - %{message}");
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
void LoggingHandler::writeToConsole(QtMsgType type, const QMessageLogContext& context, const QString& message)
{
    constexpr auto darkMode = true;
    auto color = fmt::color::black;
    QString typeIndicator;

    switch (type) {
    case QtDebugMsg:
        color = fmt::color::green;
        typeIndicator = "Debug";
        break;
    case QtWarningMsg:
        color = fmt::color::orange;
        typeIndicator = "Warning";
        break;
    case QtCriticalMsg:
        color = fmt::color::magenta;
        typeIndicator = "Critical";
        break;
    case QtFatalMsg:
        color = fmt::color::red;
        typeIndicator = "Fatal";
        break;
    default:
        color = darkMode ? fmt::color::gray : fmt::color::black;
        typeIndicator = "Info"; // Assuming default is info
        break;
    }

    const auto now = QDateTime::currentDateTime().toString("dd.MM.yyyy h:mm:ss.zzz");
    const auto filename = extractFileName(context);
    const auto function = extractFunction(context);
    const auto line = context.line;
    fmt::print(
        "[{}] {} {}:{} - {}\n",
        fmt::styled(now.toStdString(), fmt::emphasis::bold),
        fmt::styled(typeIndicator.toStdString(), fg(color)),
        function.toStdString(), // Replace with context.file when QTCREATORBUG-24353 is fixed
        line,
        message.toStdString());
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
    LoggingHandler::writeToConsole(type, context, message);
    LoggingHandler::writeToFile(line);
}
}
