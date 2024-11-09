#pragma once
#include <QFile>
#include <QLoggingCategory>
#include <QObject>
#include <mutex>

namespace ScreenPlayCore {
class LoggingHandler : public QObject {
    Q_OBJECT

public:
    explicit LoggingHandler(const QString& logFileName);
    ~LoggingHandler() override;

private:
    static void start();
    static void stop();
    static std::mutex& logFileMutex();
    static QFile& logFile();
    static size_t& logFileCounter();
    static QString logLine(QtMsgType type, const QMessageLogContext& context, const QString& message);
    static QString toString(QtMsgType type);
    static QString extractFileName(const QMessageLogContext& context);
    static QString extractFunction(const QMessageLogContext& context);
    static void writeToConsole(QtMsgType type, const QMessageLogContext& context, const QString& message);
    static void writeToFile(const QString& line);
    static void checkLogRotation();
    static void loggingMessageHandler(QtMsgType type, const QMessageLogContext& context, const QString& message);

private:
    // When increasing the allowed file size, the code that selects the correct log file must be adjusted.
    // Otherwise already completed log files of the day will be expanded.
    static constexpr qint64 m_maxFileSize = 2000000000; // 2GB
    static QString m_logFileName;
};
}
