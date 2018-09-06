#pragma once

#include <QApplication>
#include <QDir>
#include <QEasingCurve>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLocalSocket>
#include <QObject>
#include <QPropertyAnimation>
#include <QQmlContext>
#include <QQmlEngine>
#include <QScreen>
#include <QSharedPointer>
#include <QWindow>
#include <QtGlobal>
#include <QtQuick/QQuickView>
#include <QtQuick/QQuickWindow>

#ifdef Q_OS_WIN
#include <qt_windows.h>
#endif


class MainWindow : public QWindow {
    Q_OBJECT

public:
    explicit MainWindow(int i, QString projectPath, QString id, QString decoder, QString volume, QString fillmode, QScreen* parent = nullptr);
    ~MainWindow();

    Q_PROPERTY(QVector<int> screenNumber READ screenNumber WRITE setScreenNumber NOTIFY screenNumberChanged)
    Q_PROPERTY(QString projectConfig READ projectConfig WRITE setProjectConfig NOTIFY projectConfigChanged)
    Q_PROPERTY(QString appID READ name WRITE setname NOTIFY nameChanged)
    Q_PROPERTY(QString type READ type WRITE setType NOTIFY typeChanged)

    Q_PROPERTY(QString fillMode READ fillMode WRITE setFillMode NOTIFY fillModeChanged)
    Q_PROPERTY(bool loops READ loops WRITE setLoops NOTIFY loopsChanged)
    Q_PROPERTY(float volume READ volume WRITE setVolume NOTIFY volumeChanged)
    Q_PROPERTY(QString fullContentPath READ fullContentPath WRITE setFullContentPath NOTIFY fullContentPathChanged)
    Q_PROPERTY(bool isPlaying READ isPlaying WRITE setIsPlaying NOTIFY isPlayingChanged)

    QString projectConfig() const
    {
        return m_projectConfig;
    }

    QString name() const
    {
        return m_appID;
    }

    QVector<int> screenNumber() const
    {
        return m_screenNumber;
    }

    QUrl projectPath() const;
    void setProjectPath(const QUrl& projectPath);

    QString type() const
    {
        return m_type;
    }

    void setScreenNumber(const QVector<int>& screenNumber);

    QString fillMode() const
    {
        return m_fillMode;
    }

    bool loops() const
    {
        return m_loops;
    }

    float volume() const
    {
        return m_volume;
    }

    QString fullContentPath() const
    {
        return m_fullContentPath;
    }

    bool isPlaying() const
    {
        return m_isPlaying;
    }
public slots:
    void connected();
    void disconnected();
    void bytesWritten(qint64 bytes);
    void readyRead();
    void error(QLocalSocket::LocalSocketError socketError);
    void destroyThis();
    void init();

    void setProjectConfig(QString projectConfig)
    {
        if (m_projectConfig == projectConfig)
            return;

        m_projectConfig = projectConfig;
        emit projectConfigChanged(m_projectConfig);
    }

    void setname(QString appID)
    {
        if (m_appID == appID)
            return;

        m_appID = appID;
        emit nameChanged(m_appID);
    }

    void setType(QString type)
    {
        if (m_type == type)
            return;

        m_type = type;
        emit typeChanged(m_type);
    }

    void setFillMode(QString fillMode)
    {
        if (m_fillMode == fillMode)
            return;

        m_fillMode = fillMode;
        emit fillModeChanged(m_fillMode);
    }

    void setLoops(bool loops)
    {
        if (m_loops == loops)
            return;

        m_loops = loops;
        emit loopsChanged(m_loops);
    }

    void setVolume(float volume)
    {
        qWarning("Floating point comparison needs context sanity check");
        if (qFuzzyCompare(m_volume, volume))
            return;

        m_volume = volume;
        emit volumeChanged(m_volume);
    }

    QString getApplicationPath();

    void setFullContentPath(QString fullContentPath)
    {
        if (m_fullContentPath == fullContentPath)
            return;

        m_fullContentPath = fullContentPath;
        emit fullContentPathChanged(m_fullContentPath);
    }

    void setIsPlaying(bool isPlaying)
    {
        if (m_isPlaying == isPlaying)
            return;

        m_isPlaying = isPlaying;
        emit isPlayingChanged(m_isPlaying);
    }

signals:
    void playVideo(QString path);
    void playQmlScene(QString file);
    void projectConfigChanged(QString projectConfig);
    void nameChanged(QString appID);
    void screenNumberChanged(QVector<int> screenNumber);
    void typeChanged(QString type);
    void fillModeChanged(QString fillMode);
    void loopsChanged(bool loops);
    void volumeChanged(float volume);
    void fullContentPathChanged(QString fullContentPath);

    void isPlayingChanged(bool isPlaying);

private:
#ifdef Q_OS_WIN
    HWND m_hwnd;
    HWND m_worker_hwnd;
#endif
    QSharedPointer<QQuickView> m_quickRenderer = nullptr;
    QUrl m_projectPath;
    QString m_projectFile;
    QJsonObject m_project;
    QString m_projectConfig;
    QSharedPointer<QLocalSocket> m_socket;

    QString m_appID;
    QVector<int> m_screenNumber;
    QString m_type;
    QString m_fillMode;
    bool m_loops;
    float m_volume;
    QString m_fullContentPath;
    bool m_isPlaying;
};
